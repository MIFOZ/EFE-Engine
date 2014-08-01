#include "impl/DX11VertexBuffer.h"
#include "system/LowLevelSystem.h"
#include "math/Math.h"
#include "impl/LowLevelGraphicsDX11.h"
#include "graphics/MaterialRenderEffect.h"

#include "impl/DX11ProgramBuffer.h"
#include "impl/RenderPipelineSystem/DX11PipelineManager.h"

#include <string.h>

namespace efe
{
	struct cVertex1
	{
		cVector3f pos;
	};

	cDX11VertexBuffer::cDX11VertexBuffer(iLowLevelGraphics *a_pLowLevelGraphics, tVertexFlag a_Flags,
			eVertexBufferDrawType a_DrawType, eVertexBufferUsageType a_UsageType,
			int a_lReserveVtxSize, int a_lReserveIdxSize, cDX11PipelineManager *a_pPipelineManager) :
			iVertexBuffer(a_pLowLevelGraphics, a_Flags, a_DrawType, a_UsageType, a_lReserveVtxSize, a_lReserveIdxSize)
	{
		m_pPipelineManager = a_pPipelineManager;

		m_pVertexBuffer = NULL;
		m_pIndexBuffer = NULL;

		m_lVtxNum = a_lReserveVtxSize;

		if (a_lReserveVtxSize > 0)
		{
			for (int i=0;i < klNumOfVertexFlags;i++)
			{
				if (a_Flags & kvVertexFlags[i])
					m_vVertexArray[i].reserve(a_lReserveVtxSize * kvVertexElements[i]);
			}
		}

		if (a_lReserveIdxSize > 0)
			m_vIndexArray.reserve(a_lReserveIdxSize);

		m_bTangents = false;

		m_bCompiled = false;

		m_bHasShadowDouble = false;

		m_pLowLevelGraphics = a_pLowLevelGraphics;
	}

	cDX11VertexBuffer::~cDX11VertexBuffer()
	{
		for (int i=0;i < klNumOfVertexFlags;i++)
			m_vVertexArray[i].clear();
		m_vIndexArray.clear();

		if (m_pVertexBuffer)
			efeDelete(m_pVertexBuffer);

		if (m_pIndexBuffer)
			efeDelete(m_pIndexBuffer);
	}

	void cDX11VertexBuffer::AddVertex(tVertexFlag a_Type, const cVector3f &a_vVtx)
	{
		int idx = cMath::Log2ToInt((int)a_Type);

		m_vVertexArray[idx].push_back(a_vVtx.x);
		m_vVertexArray[idx].push_back(a_vVtx.y);
		m_vVertexArray[idx].push_back(a_vVtx.z);
		if (kvVertexElements[idx]==4)
			m_vVertexArray[idx].push_back(1);
	}

	void cDX11VertexBuffer::AddColor(tVertexFlag a_Type, const cColor &a_Color)
	{
		int idx = cMath::Log2ToInt((int)a_Type);

		m_vVertexArray[idx].push_back(a_Color.r);
		m_vVertexArray[idx].push_back(a_Color.g);
		m_vVertexArray[idx].push_back(a_Color.b);
		m_vVertexArray[idx].push_back(a_Color.a);
	}

	void cDX11VertexBuffer::AddIndex(unsigned int a_lIndex)
	{
		m_vIndexArray.push_back(a_lIndex);
	}

	void cDX11VertexBuffer::AddTriangle(unsigned int a_lA, unsigned int a_lB, unsigned int a_lC)
	{
		m_vIndexArray.push_back(a_lA);
		m_vIndexArray.push_back(a_lB);
		m_vIndexArray.push_back(a_lC);
	}

	void cDX11VertexBuffer::AddQuad(unsigned int a_lA, unsigned int a_lB, unsigned int a_lC, unsigned int a_lD)
	{
		AddTriangle(a_lA, a_lB, a_lC);
		AddTriangle(a_lA, a_lC, a_lD);
	}

	bool cDX11VertexBuffer::Compile(tVertexCompileFlag a_Flags)
	{
		if (m_bCompiled) return false;
		m_bCompiled = true;

		if (a_Flags & eVertexCompileFlag_CreateTangents)
		{
			m_bTangents = true;

			m_VertexFlags |= eVertexFlag_Tangent;

			int idx = cMath::Log2ToInt((int)eVertexFlag_Tangent);

			int lSize = GetVertexNum()*4;
			m_vVertexArray[idx].resize(lSize);

			cMath::CreateTriTangentVectors(&(m_vVertexArray[cMath::Log2ToInt((int)eVertexFlag_Tangent)][0]),
				&m_vIndexArray[0], GetIndexNum(),

				&(m_vVertexArray[cMath::Log2ToInt((int)eVertexFlag_Position)][0]),
				kvVertexElements[cMath::Log2ToInt((int)eVertexFlag_Position)],

				&(m_vVertexArray[cMath::Log2ToInt((int)eVertexFlag_Texture0)][0]),
				&(m_vVertexArray[cMath::Log2ToInt((int)eVertexFlag_Normal)][0]),
				GetVertexNum());
		}

		int lSizeOfvertex = 0;

		int lNumOfElements = 0;

		for (int i = 0; i < klNumOfVertexFlags; i++)
		{
			if (m_vVertexArray[i].size() != 0)
			{
				//Size of vertex evaluated in 4 bytes
				lSizeOfvertex += kvVertexElements[i];

				//Number of elements in a dest array
				lNumOfElements += m_vVertexArray[i].size();
			}
		}

		if (m_lVtxNum == 0)
			m_lVtxNum = m_vVertexArray[1].size()/kvVertexElements[1];

		m_lVtxSize = lSizeOfvertex;

		tFloatVec vMemBuff;
		vMemBuff.resize(lNumOfElements);

		int lOffset = 0;
		int p = 0;

		Log("\n-------------------------------\n");

		for (int k = 0; k < klNumOfVertexFlags; k++)
		{
			int lSize = m_vVertexArray[k].size();
			int lPadding = lSizeOfvertex - lOffset - kvVertexElements[k];
			for (int j = 0,p = 0; j < lSize; j++)
			{
				if (j%kvVertexElements[k]==0 && j!=0)
					p += lPadding + lOffset;
				vMemBuff[p + lOffset] = m_vVertexArray[k][j];
				
				p++;
			}
			
			if (lSize > 0)
			{
				lOffset += kvVertexElements[k];
				/*for (int w  = 0; w < vMemBuff.size(); w++)
					Log("%.2f, ", vMemBuff[w]);*/
				Log("\n-------------------------------\n");
			}
		}

		cDX11ProgramBufferConfig Config;
		eBufferUsage Usage = eBufferUsage_Static;
		if (m_UsageType == eVertexBufferUsageType_Dynamic) Usage = eBufferUsage_Dynamic;
		else if (m_UsageType == eVertexBufferUsageType_Stream) Usage = eBufferUsage_Stream;
		Config.SetVertexBuffer(Usage, lSizeOfvertex * sizeof(float) * m_lVtxNum, lSizeOfvertex * sizeof(float));

		m_pVertexBuffer = m_pLowLevelGraphics->CreateGpuProgramBuffer(eGpuProgramBufferType_VertexIndex, &Config, vMemBuff.data());
		if (m_pVertexBuffer == NULL)
			return false;

		/*D3D11_BUFFER_DESC descBuffer;
		descBuffer.Usage = usageType;
		descBuffer.ByteWidth = lSizeOfvertex * sizeof(float) * m_lVtxNum;
		descBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		descBuffer.CPUAccessFlags = lFlags;
		descBuffer.MiscFlags = 0;
		descBuffer.StructureByteStride = lSizeOfvertex * sizeof(float);

		D3D11_SUBRESOURCE_DATA descData;

		descData.pSysMem = vMemBuff.data();
		descData.SysMemPitch = 0;
		descData.SysMemSlicePitch = 0;*/

		/*Log("\n--------------------------------------\n");
		Log("Creating Vtx buffer\n");
		for (int i=0;i<32;i++)
			Log("%f, ", ((float *)descData.pSysMem)[i]);
		Log("\n--------------------------------------\n");*/

		Config.SetIndexBuffer(Usage, m_vIndexArray.size() * sizeof(unsigned int), sizeof(unsigned int));

		m_pIndexBuffer = m_pLowLevelGraphics->CreateGpuProgramBuffer(eGpuProgramBufferType_VertexIndex, &Config, m_vIndexArray.data());
		if (m_pIndexBuffer == NULL)
			return false;

		return true;
	}

	//--------------------------------------------------------------

	void cDX11VertexBuffer::UpdateData(tVertexFlag a_Types, bool a_lIndices)
	{
		
	}

	//--------------------------------------------------------------

	void cDX11VertexBuffer::CreateSkyBox()
	{
		int lNumVertices = 100;

		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(cVertex1) * lNumVertices;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = sizeof(cVertex1);

// 		if (FAILED(m_pLowLevelGraphics->GetDevice()->CreateBuffer(&desc, NULL, &m_pVertexBuffer)))
// 			return;
	}

	//--------------------------------------------------------------

	void cDX11VertexBuffer::Transform(const cMatrixf &a_mtxTransform)
	{
		float *pPosArray = GetArray(eVertexFlag_Position);
		float *pNormalArray = GetArray(eVertexFlag_Normal);
		float *pTangentArray = NULL;
		if (m_bTangents) pTangentArray = GetArray(eVertexFlag_Tangent);

		int lVtxNum = GetVertexNum();

		cMatrixf mtxRot = a_mtxTransform.GetRotation();

		int lVtxStride = kvVertexElements[cMath::Log2ToInt(eVertexFlag_Position)];

		int lOffset = GetVertexNum()*4;

		for (int i=0; i<lVtxNum; i++)
		{
			float *pPos = &pPosArray[i*lVtxStride];
			float *pNorm = &pNormalArray[i*3];
			float *pTan = NULL;
			if (m_bTangents) pTan = &pTangentArray[i*4];

			cVector3f vPos = cMath::MatrixMul(a_mtxTransform, cVector3f(pPos[0], pPos[1], pPos[2]));
			pPos[0] = vPos.x; pPos[1] = vPos.y; pPos[2] = vPos.z;

			if (m_bHasShadowDouble)
			{
				float *pExtraPos = &pPosArray[i*lVtxStride + lOffset];
				pExtraPos[0] = vPos.x; pExtraPos[1] = vPos.y; pExtraPos[2] = vPos.z;
			}

			cVector3f vNorm = cMath::MatrixMul(mtxRot, cVector3f(pNorm[0], pNorm[1], pNorm[2]));
			vNorm.Normalize();
			pNorm[0] = vNorm.x; pNorm[1] = vNorm.y; pNorm[2] = vNorm.z;

			if (m_bTangents)
			{
				cVector3f vTan = cMath::MatrixMul(mtxRot, cVector3f(pTan[0], pTan[1], pTan[2]));
				vTan.Normalize();
				pTan[0] = vTan.x; pTan[1] = vTan.y; pTan[2] = vTan.z;
			}
		}

		if (m_bCompiled)
		{
			if (m_bTangents)
				UpdateData(eVertexFlag_Position | eVertexFlag_Normal | eVertexFlag_Tangent, false);
			else
				UpdateData(eVertexFlag_Position | eVertexFlag_Normal, false);
		}
	}

	//--------------------------------------------------------------

	void cDX11VertexBuffer::Draw(cMaterialRenderEffect *a_pEffect, eVertexBufferDrawType a_DrawType)
	{
		m_pPipelineManager->m_InputAssemblerStage.ClearDesiredState();

		eVertexBufferDrawType drawType = a_DrawType == eVertexBufferDrawType_LastEnum ? m_DrawType : a_DrawType;
		m_pPipelineManager->m_InputAssemblerStage.m_DesiredState.m_PrimitiveTopology.SetState(GetDX11DrawTypeEnum(drawType));

		m_pPipelineManager->ClearPipelineResources();
		a_pEffect->ConfigurePipeline(m_pLowLevelGraphics, m_pLowLevelGraphics->GetGpuParameterManager());
		m_pPipelineManager->ApplyPipelineResources();

		Bind();

		m_pPipelineManager->ApplyRenderTargets();

		m_pPipelineManager->DrawIndexed(GetIndexNum(), 0, 0);
	}

	//--------------------------------------------------------------

	void cDX11VertexBuffer::Bind()
	{
		// Input layout
		ID3D11InputLayout *pLayout = static_cast<cLowLevelGraphicsDX11*>(m_pLowLevelGraphics)->GetInputLayout(GetFlags(), (iGpuProgram*)m_pPipelineManager->m_vProgramStages[eGpuProgramType_Vertex]->m_DesiredState.m_Program.GetState());
		m_pPipelineManager->m_InputAssemblerStage.m_DesiredState.m_InputLayout.SetState((unsigned int)pLayout);
		m_pPipelineManager->m_InputAssemblerStage.m_DesiredState.m_PrimitiveTopology.SetState(GetDX11DrawTypeEnum(m_DrawType));

		// Vertex buffer
		ID3D11Buffer *pBuffer = static_cast<cDX11ProgramBuffer*>(m_pVertexBuffer)->GetBuffer();
		m_pPipelineManager->m_InputAssemblerStage.m_DesiredState.m_VertexBuffers.SetState(0, (unsigned int)static_cast<cDX11ProgramBuffer*>(m_pVertexBuffer)->GetBuffer());
		m_pPipelineManager->m_InputAssemblerStage.m_DesiredState.m_VertexBufferStrides.SetState(0, GetVertexSize());
		m_pPipelineManager->m_InputAssemblerStage.m_DesiredState.m_VertexBufferOffsets.SetState(0, 0);
		
		// Index buffer
		pBuffer = static_cast<cDX11ProgramBuffer*>(m_pIndexBuffer)->GetBuffer();
		m_pPipelineManager->m_InputAssemblerStage.m_DesiredState.m_IndexBuffer.SetState((unsigned int)static_cast<cDX11ProgramBuffer*>(m_pIndexBuffer)->GetBuffer());
		m_pPipelineManager->m_InputAssemblerStage.m_DesiredState.m_IndexBufferFormat.SetState(DXGI_FORMAT_R32_UINT);
		
		m_pPipelineManager->ApplyInputResources();
	}

	//--------------------------------------------------------------

	float *cDX11VertexBuffer::GetArray(tVertexFlag a_Type)
	{
		int lIdx = cMath::Log2ToInt((int)a_Type);

		return &m_vVertexArray[lIdx][0];
	}

	//--------------------------------------------------------------

	unsigned int *cDX11VertexBuffer::GetIndices()
	{
		return &m_vIndexArray[0];
	}

	//--------------------------------------------------------------

	void cDX11VertexBuffer::ResizeArray(tVertexFlag a_Type, int a_lSize)
	{
		int lIdx = cMath::Log2ToInt((int)a_Type);

		m_vVertexArray[lIdx].resize(a_lSize);
	}

	//--------------------------------------------------------------

	void cDX11VertexBuffer::ResizeIndices(int a_lSize)
	{
		m_vIndexArray.resize(a_lSize);
	}

	//--------------------------------------------------------------

	int cDX11VertexBuffer::GetVertexNum()
	{
		int idx = cMath::Log2ToInt((int)eVertexFlag_Position);
		int lSize = (int)m_vVertexArray[idx].size()/kvVertexElements[idx];

		if (m_bHasShadowDouble) return lSize / 2;
		else return lSize;
	}

	int cDX11VertexBuffer::GetIndexNum()
	{
		return (int)m_vIndexArray.size();
	}

	//--------------------------------------------------------------

	unsigned int cDX11VertexBuffer::GetVertexSize()
	{
// 		unsigned int lSize = 0;
// 		for (int i = 0; i < klNumOfVertexFlags; i++)
// 		{
// 			if (m_VertexFlags & kvVertexFlags[i])
// 				lSize += kvVertexElements[i];
// 		}

		return m_lVtxSize * sizeof(4);
	}

	D3D11_PRIMITIVE_TOPOLOGY cDX11VertexBuffer::GetDX11DrawTypeEnum( eVertexBufferDrawType a_Type)
	{
		switch (a_Type)
		{
		case efe::eVertexBufferDrawType_LineList:		return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case efe::eVertexBufferDrawType_LineStrip:		return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case efe::eVertexBufferDrawType_TriangleList:	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case efe::eVertexBufferDrawType_TriangleStrip:	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		default:										return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}
	}
}