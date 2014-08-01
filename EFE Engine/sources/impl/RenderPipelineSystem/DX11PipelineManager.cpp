#include "impl/RenderPipelineSystem/DX11PipelineManager.h"

#include "graphics/MaterialRenderEffect.h"

#include "impl/DX11ProgramBuffer.h"
#include "impl/DX11Program.h"
#include "impl/DX11Texture.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cDX11PipelineManager::cDX11PipelineManager(iLowLevelGraphics *a_pLowLevelGraphics, ID3D11DeviceContext *a_pDeviceContext)
		: m_VertexProgramStage(eGpuProgramType_Vertex),
		m_GeomProgramStage(eGpuProgramType_Geometry),
		m_PixelProgramStage(eGpuProgramType_Pixel)
	{
		m_pLowLevelGraphics = a_pLowLevelGraphics;

		m_pContext = a_pDeviceContext;

		m_vProgramStages[eGpuProgramType_Vertex] = &m_VertexProgramStage;
		m_vProgramStages[eGpuProgramType_Geometry] = &m_GeomProgramStage;
		m_vProgramStages[eGpuProgramType_Pixel] = &m_PixelProgramStage;
	}

	//--------------------------------------------------------------

	cDX11PipelineManager::~cDX11PipelineManager()
	{
		if (m_pContext)
		{
			m_pContext->ClearState();
			m_pContext->Flush();
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cDX11PipelineManager::SetDeviceContext(ID3D11DeviceContext *a_pContext)
	{
		m_pContext = a_pContext;
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::BindConstatntBufferParameter(eGpuProgramType a_ProgramType, iGpuProgramParameter *a_pParameter, unsigned int a_lSlot, cGpuProgramParameterManager *a_pParameterManager)
	{
		if (a_pParameter)
		{
			if (a_pParameter->GetType() == eGpuProgramParameterType_ConstantBuffer)
			{
				cGpuProgramConstantBufferParameter *pCBParam = static_cast<cGpuProgramConstantBufferParameter*>(a_pParameter);

				 cDX11ProgramBuffer *pBuffer = static_cast<cDX11ProgramBuffer*>(pCBParam->GetBuffer());
				 if (pBuffer)
					 m_vProgramStages[a_ProgramType]->m_DesiredState.m_ConstantBuffers.SetState(a_lSlot, pBuffer->m_pDXBuffer);
			}
		}
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::BindShaderResourceParameter(eGpuProgramType a_ProgramType, iGpuProgramParameter *a_pParameter, unsigned int a_lSlot, cGpuProgramParameterManager *a_pParameterManager)
	{
		if (a_pParameter)
		{
			if (a_pParameter->GetType() == eGpuProgramParameterType_Texture)
			{
				cDX11Texture *pDXTex = static_cast<cDX11Texture*>(static_cast<cGpuProgramTextureParameter*>(a_pParameter)->GetTexture());
				if (pDXTex != NULL)
				{
					ID3D11ShaderResourceView *pRes = (ID3D11ShaderResourceView*)pDXTex->GetSRV();
					m_vProgramStages[a_ProgramType]->m_DesiredState.m_ShaderResourceViews.SetState(a_lSlot, pRes);
				}
			}
		}
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::BindUnorderedAccessParameter(eGpuProgramType a_ProgramType, iGpuProgramParameter *a_pParameter, unsigned int a_lSlot, cGpuProgramParameterManager *a_pParameterManager)
	{
		
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::BindSamplerStateParameter(eGpuProgramType a_ProgramType, iGpuProgramParameter *a_pParameter, unsigned int a_lSlot, cGpuProgramParameterManager *a_pParameterManager)
	{
		if (a_pParameter)
		{
			if (a_pParameter->GetType() == eGpuProgramParameterType_SamplerState)
			{
				ID3D11SamplerState *pDXSampler = (ID3D11SamplerState*)(static_cast<cGpuProgramSamplerStateParameter*>(a_pParameter)->GetSamplerPtr());
				if (pDXSampler != NULL)
				{
					m_vProgramStages[a_ProgramType]->m_DesiredState.m_SamplerStates.SetState(a_lSlot, pDXSampler);
				}
			}
		}
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::BindProgram( eGpuProgramType a_Type, iGpuProgram *a_pProg, cGpuProgramParameterManager *a_pParameterManager )
	{
		m_vProgramStages[a_Type]->m_DesiredState.m_Program.SetState((unsigned int)a_pProg);

		if (a_pProg)
		{
			cDX11Program *pDXProg = static_cast<cDX11Program*>(a_pProg);
			pDXProg->BindParameters(this, a_pParameterManager);
		}
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::ApplyInputResources()
	{
		m_InputAssemblerStage.ApplyDesiredState(m_pContext);
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::ClearInputResources()
	{
		m_InputAssemblerStage.ClearDesiredState();
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::ApplyPipelineResources()
	{
		for (int i = 0; i < (int)eGpuProgramType_LastEnum; i++)
		{
			m_vProgramStages[i]->ApplyDesiredState(m_pContext);
		}

		m_RasterizerStage.ApplyDesiredState(m_pContext);
		m_OutputMergerStage.ApplyDesiredBlendAndDepthStencilStates(m_pContext);
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::ClearPipelineResources()
	{
		for (int i = 0; i < (int)eGpuProgramType_LastEnum; i++)
		{
			m_vProgramStages[i]->ClearDesiredState();
		}
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::ApplyRenderTargets()
	{
		m_OutputMergerStage.ApplyDesiredRenderTargetStates(m_pContext);
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::ClearRenderTargets()
	{
		m_OutputMergerStage.ClearDesiredState();
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::ClearPipelineState()
	{
		m_InputAssemblerStage.ClearCurrentState();
		m_InputAssemblerStage.ClearDesiredState();

		m_VertexProgramStage.ClearCurrentState();
		m_VertexProgramStage.ClearDesiredState();

		m_GeomProgramStage.ClearCurrentState();
		m_GeomProgramStage.ClearDesiredState();

		m_RasterizerStage.ClearCurrentState();
		m_RasterizerStage.ClearDesiredState();

		m_PixelProgramStage.ClearCurrentState();
		m_PixelProgramStage.ClearDesiredState();

		m_OutputMergerStage.ClearCurrentState();
		m_OutputMergerStage.ClearDesiredState();

		m_pContext->ClearState();
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::ClearPipelineSRVs()
	{
		for (int i = 0; i < (int)eGpuProgramType_LastEnum; i++)
		{
			m_vProgramStages[i]->m_DesiredState.m_ShaderResourceViews.InitializeStates();
		} 
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::Draw( cMaterialRenderEffect *a_pEffect, cSubMesh *a_pMesh, cGpuProgramParameterManager *a_pParameterManager )
	{

	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::Draw( cMaterialRenderEffect *a_pEffect, iGpuProgramBuffer *a_pVertexBuffer, iGpuProgramBuffer *a_pIndexBuffer, iVertexFormat *a_pInputLayout,D3D11_PRIMITIVE_TOPOLOGY a_PrimTopology, unsigned int a_lVertexStride, unsigned int a_lVertexNum, cGpuProgramParameterManager *a_pParameterManager )
	{
		m_InputAssemblerStage.ClearDesiredState();

		m_InputAssemblerStage.m_DesiredState.m_PrimitiveTopology.SetState(a_PrimTopology);

		if (a_pVertexBuffer)
		{
			ID3D11Buffer *pBuffer = static_cast<cDX11ProgramBuffer*>(a_pVertexBuffer)->m_pDXBuffer;
			m_InputAssemblerStage.m_DesiredState.m_VertexBuffers.SetState(0, (int)pBuffer);
			m_InputAssemblerStage.m_DesiredState.m_VertexBufferStrides.SetState(0, a_lVertexStride);
			m_InputAssemblerStage.m_DesiredState.m_VertexBufferOffsets.SetState(0, 0);
		}

		if (a_pIndexBuffer)
		{
			ID3D11Buffer *pBuffer = static_cast<cDX11ProgramBuffer*>(a_pIndexBuffer)->m_pDXBuffer;
			m_InputAssemblerStage.m_DesiredState.m_IndexBuffer.SetState((int)pBuffer);
			m_InputAssemblerStage.m_DesiredState.m_IndexBufferFormat.SetState(DXGI_FORMAT_R32_UINT);
		}

		if (a_pVertexBuffer)
			m_InputAssemblerStage.m_DesiredState.m_InputLayout.SetState((int)a_pInputLayout);

		m_InputAssemblerStage.ApplyDesiredState(m_pContext);

		ClearPipelineResources();
		a_pEffect->ConfigurePipeline(m_pLowLevelGraphics, a_pParameterManager);
		ApplyPipelineResources();

		m_pContext->DrawIndexed(a_lVertexNum, 0, 0);
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::DrawIndexed(UINT a_lIndexCount, UINT a_lStartIndex, int a_lVertexOffset)
	{
		m_pContext->DrawIndexed(a_lIndexCount, a_lStartIndex, a_lVertexOffset);
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::ClearBuffers(const cColor &a_Color, float a_fDepth, unsigned int a_lStencilRef, bool a_bClearColor, bool a_bClearDepth, bool a_bClearStencil)
	{
		if (a_bClearColor)
		{
			ID3D11RenderTargetView *pRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {NULL};
			ID3D11DepthStencilView *pDSV = NULL;

			int lViewNum = m_OutputMergerStage.GetCurrentState().GetRenderTargetNum();
			for (int i = 0; i < lViewNum; i++)
			{
				pRTVs[i] = (ID3D11RenderTargetView*)m_OutputMergerStage.GetCurrentState().m_RenderTargetsViews.GetState(i);
				if (pRTVs[i] != NULL)
				{
					float vColor [] = {a_Color.r, a_Color.g, a_Color.b, a_Color.a};
					m_pContext->ClearRenderTargetView(pRTVs[i], vColor);
				}
			}
		}

		if (a_bClearDepth || a_bClearStencil)
		{
			if (m_OutputMergerStage.GetCurrentState().m_DepthTargetView.GetState() != 0)
			{
				ID3D11DepthStencilView *pDSV = (ID3D11DepthStencilView*)m_OutputMergerStage.GetCurrentState().m_DepthTargetView.GetState();
				UINT flags = 0;
				if (a_bClearDepth) flags |= D3D11_CLEAR_DEPTH;
				if (a_bClearStencil) flags |= D3D11_CLEAR_STENCIL;
				m_pContext->ClearDepthStencilView(pDSV, flags, a_fDepth, a_lStencilRef);
			}
		}
	}

	//--------------------------------------------------------------

	D3D11_MAPPED_SUBRESOURCE cDX11PipelineManager::MapResource(iGpuProgramBuffer *a_pBuffer, UINT a_lSubresource, D3D11_MAP a_Actions, UINT a_lFlags)
	{
		D3D11_MAPPED_SUBRESOURCE Data;
		Data.pData = NULL;
		Data.DepthPitch = Data.RowPitch = 0;

		if (a_pBuffer == NULL)
			return Data;

		ID3D11Buffer *pBuffer = static_cast<cDX11ProgramBuffer*>(a_pBuffer)->GetBuffer();
		if (pBuffer == NULL)
			return Data;

		HRESULT hr = m_pContext->Map(pBuffer, a_lSubresource, a_Actions, a_lFlags, &Data);
		if (FAILED(hr))
			Error("Failed to map a resource!\n");

		return Data;
	}

	//--------------------------------------------------------------

	void cDX11PipelineManager::UnmapResource(iGpuProgramBuffer *a_pBuffer, UINT a_lSubresource)
	{
		if (a_pBuffer == NULL)
			return;

		ID3D11Buffer *pBuffer = static_cast<cDX11ProgramBuffer*>(a_pBuffer)->GetBuffer();
		if (pBuffer == NULL)
			return;

		m_pContext->Unmap(pBuffer, a_lSubresource);
	}
}