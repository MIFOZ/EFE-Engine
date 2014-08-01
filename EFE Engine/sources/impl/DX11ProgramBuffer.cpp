#include "impl/DX11ProgramBuffer.h"

#include "impl/RenderPipelineSystem/DX11PipelineManager.h"

#include "graphics/GpuProgramParameterManager.h"

namespace efe
{
	void cDX11ProgramBuffer::SetDesiredDescription(D3D11_BUFFER_DESC a_Desc)
	{
		m_DesiredDesc = a_Desc;
	}

	void cDX11ProgramBufferConfig::SetDefaults()
	{
		m_State.ByteWidth = 1;
		m_State.Usage = D3D11_USAGE_DEFAULT;
		m_State.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		m_State.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_State.MiscFlags = 0;
		m_State.StructureByteStride = 0;
	}

	void cDX11ProgramBufferConfig::SetConstantBuffer(unsigned int a_lSize, bool a_bDynamic)
	{
		m_State.ByteWidth = a_lSize;
		m_State.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		m_State.MiscFlags = 0;
		m_State.StructureByteStride = 0;

		if (a_bDynamic)
		{
			m_State.Usage = D3D11_USAGE_DYNAMIC;
			m_State.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else
		{
			m_State.Usage = D3D11_USAGE_IMMUTABLE;
			m_State.CPUAccessFlags = 0;
		}
	}

	void cDX11ProgramBufferConfig::SetVertexBuffer(eBufferUsage a_Usage, unsigned int a_lSize, unsigned int a_lStride)
	{
		m_State.ByteWidth = a_lSize;
		m_State.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_State.MiscFlags = 0;
		m_State.StructureByteStride = a_lStride;

		m_State.Usage = D3D11_USAGE_IMMUTABLE;
		m_State.CPUAccessFlags = 0;
		if (a_Usage == eBufferUsage_Dynamic)
		{
			m_State.Usage = D3D11_USAGE_DYNAMIC;
			m_State.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else if (a_Usage == eBufferUsage_Stream)
		{
			m_State.Usage = D3D11_USAGE_STAGING;
			m_State.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		}
	}

	void cDX11ProgramBufferConfig::SetIndexBuffer(eBufferUsage a_Usage, unsigned int a_lSize, unsigned int a_lStride)
	{
		m_State.ByteWidth = a_lSize;
		m_State.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_State.MiscFlags = 0;
		m_State.StructureByteStride = a_lStride;

		m_State.Usage = D3D11_USAGE_IMMUTABLE;
		m_State.CPUAccessFlags = 0;
		if (a_Usage == eBufferUsage_Dynamic)
		{
			m_State.Usage = D3D11_USAGE_DYNAMIC;
			m_State.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else if (a_Usage == eBufferUsage_Stream)
		{
			m_State.Usage = D3D11_USAGE_STAGING;
			m_State.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		}
	}

	cDX11ProgramConstantBuffer::cDX11ProgramConstantBuffer(ID3D11Buffer *a_pDXBuffer)
		: cDX11ProgramBuffer(eGpuProgramBufferType_Constant, a_pDXBuffer)
	{
		m_bAutoUpdate = true;
	}

	void cDX11ProgramConstantBuffer::AddMapping(cConstantBufferMapping &a_Mapping)
	{
		m_vMappings.push_back(a_Mapping);
	}

	void cDX11ProgramConstantBuffer::ClearMappings()
	{
		m_vMappings.clear();
	}

	void cDX11ProgramConstantBuffer::EvaluateMapping(cDX11PipelineManager *a_pDXPipeline, cGpuProgramParameterManager *a_pParameterManager)
	{
		if (m_pDXBuffer)
		{
			if (GetAutoUpdate())
			{
				bool bNeedsUpdate = false;
				for (unsigned int i = 0; i < m_vMappings.size(); i++)
				{
					if (m_vMappings[i].m_pParameter->GetValueIdx() != m_vMappings[i].m_lValueIdx)
					{
						bNeedsUpdate = true;
						break;
					}
				}

				if (bNeedsUpdate)
				{
					D3D11_MAPPED_SUBRESOURCE Resource = a_pDXPipeline->MapResource(this, 0, D3D11_MAP_WRITE_DISCARD, 0);

					for (cConstantBufferMapping &Mapping : m_vMappings)
					{
						iGpuProgramParameter *pParam = Mapping.m_pParameter;
						unsigned lOffset = Mapping.m_lOffset;
						unsigned lSize = Mapping.m_lSize;
						unsigned lElementsNum = Mapping.m_lElementsNum;
						unsigned lValueIdx = Mapping.m_lValueIdx;

						Mapping.m_lValueIdx = pParam->GetValueIdx();

						if (Mapping.m_VarClass == D3D_SVC_VECTOR)
						{
							cVector4f vValue = a_pParameterManager->GetVectorParameterValue(pParam);
							cVector4f *pBuf = (cVector4f*)((char*)Resource.pData + lOffset);
							*pBuf = vValue;
						}
						else if (Mapping.m_VarClass == D3D_SVC_MATRIX_ROWS ||
							Mapping.m_VarClass == D3D_SVC_MATRIX_COLUMNS)
						{
							if (lElementsNum == 0)
							{
								cMatrixf mtxValue = a_pParameterManager->GetMatrixParameterValue(pParam);
								cMatrixf *pBuf = (cMatrixf*)((char*)Resource.pData + lOffset);
								*pBuf = mtxValue;
							}
							else
							{
								if (lSize == lElementsNum * sizeof(cMatrixf))
								{
									cMatrixf *pMatrices = a_pParameterManager->GetMatrixArrayParameterValue(pParam);
									memcpy((char*)Resource.pData + lOffset, pMatrices, lSize);
								}
							}
						}
					}

					a_pDXPipeline->UnmapResource(this, 0);
				}
			}
		}
	}

	bool cDX11ProgramConstantBuffer::ContainsMapping(unsigned int a_lIdx, const cConstantBufferMapping &a_Mapping)
	{
		bool bRes = false;

		if (a_lIdx < m_vMappings.size())
		{
			cConstantBufferMapping &IntMapping = m_vMappings[a_lIdx];

			if (IntMapping.m_pParameter == a_Mapping.m_pParameter
				&& IntMapping.m_lOffset == a_Mapping.m_lOffset
				&& IntMapping.m_lSize == a_Mapping.m_lSize
				&& IntMapping.m_VarClass == a_Mapping.m_VarClass
				&& IntMapping.m_lElementsNum == a_Mapping.m_lElementsNum)
				bRes = true;
		}

		return bRes;
	}

	void cDX11ProgramConstantBuffer::SetAutoUpdate(bool a_bX)
	{
		m_bAutoUpdate = a_bX;
	}

	bool cDX11ProgramConstantBuffer::GetAutoUpdate()
	{
		return m_bAutoUpdate;
	}
}