#include "impl/RenderPipelineSystem/RenderStages/Fixed Stages/DX11OutputMergerStage.h"
#include "impl/LowLevelGraphicsDX11.h"

#include "system/SystemTypes.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cDX11OutputMergerStage::cDX11OutputMergerStage()
	{
		m_DesiredState.SetSisterState(&m_CurrentState);
	}

	//--------------------------------------------------------------

	cDX11OutputMergerStage::~cDX11OutputMergerStage()
	{

	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cDX11OutputMergerStage::ClearDesiredState()
	{
		m_DesiredState.CleateState();
	}
	
	//--------------------------------------------------------------

	void cDX11OutputMergerStage::ClearCurrentState()
	{
		m_CurrentState.CleateState();
	}

	//--------------------------------------------------------------

	void cDX11OutputMergerStage::ApplyDesiredRenderTargetStates(ID3D11DeviceContext *a_pDXContext)
	{
		//assert(0);
		int lRTVCount = 0;
		int lUAVCount = 0;

		if (m_DesiredState.m_RenderTargetsViews.IsUpdateNeeded() ||
			m_DesiredState.m_UnorderedAccessViews.IsUpdateNeeded() ||
			m_DesiredState.m_DepthTargetView.IsUpdateNeeded())
		{
			ID3D11RenderTargetView *vRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
			ID3D11UnorderedAccessView *vUAVs[D3D11_PS_CS_UAV_REGISTER_COUNT];
			ID3D11DepthStencilView *pDSV = NULL;

			for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
			{
				vRTVs[i] = (ID3D11RenderTargetView*)m_DesiredState.m_RenderTargetsViews.GetState(i);

				if (vRTVs[i] == NULL)
					lRTVCount = i+1;
			}

			for (int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++)
			{
				vUAVs[i] = (ID3D11UnorderedAccessView*)m_DesiredState.m_UnorderedAccessViews.GetState(i);

				if (vUAVs[i] == NULL)
					lUAVCount = i+1;
			}

			pDSV = (ID3D11DepthStencilView*)m_DesiredState.m_DepthTargetView.GetState();

			a_pDXContext->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, vRTVs, pDSV);

			for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
			{
				m_CurrentState.m_RenderTargetsViews.SetState(i, m_DesiredState.m_RenderTargetsViews.GetState(i));
			}
			for (int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++)
			{
				m_CurrentState.m_UnorderedAccessViews.SetState(i, m_DesiredState.m_UnorderedAccessViews.GetState(i));
				m_CurrentState.m_UAVInitialCounts.SetState(i, m_DesiredState.m_UAVInitialCounts.GetState(i));
			}

			m_CurrentState.m_DepthTargetView.SetState(m_DesiredState.m_DepthTargetView.GetState());

			m_DesiredState.m_RenderTargetsViews.ResetTracking();
			m_DesiredState.m_UnorderedAccessViews.ResetTracking();
			m_DesiredState.m_UAVInitialCounts.ResetTracking();
			m_DesiredState.m_DepthTargetView.ResetTracking();
		}
	}

	//--------------------------------------------------------------

	void cDX11OutputMergerStage::ApplyDesiredBlendAndDepthStencilStates(ID3D11DeviceContext *a_pDXContext)
	{
		if (m_DesiredState.m_BlendState.IsUpdateNeeded())
		{
			ID3D11BlendState *pBlendState = (ID3D11BlendState*)m_DesiredState.m_BlendState.GetState();

			if (pBlendState)
			{
				float aBlendFactors[] = {1.0f, 1.0f, 1.0f, 1.0f};
				a_pDXContext->OMSetBlendState(pBlendState, aBlendFactors, 0xFFFFFFFF);

				m_CurrentState.m_BlendState.SetState(m_DesiredState.m_BlendState.GetState());
				m_DesiredState.m_BlendState.ResetTracking();
			}
		}

		if (m_DesiredState.m_DepthStencilState.IsUpdateNeeded() || m_DesiredState.m_StencilRefState.IsUpdateNeeded())
		{
			ID3D11DepthStencilState *pDepthStencilState = (ID3D11DepthStencilState*)m_DesiredState.m_DepthStencilState.GetState();

			if (pDepthStencilState)
			{
				a_pDXContext->OMSetDepthStencilState(pDepthStencilState, m_DesiredState.m_StencilRefState.GetState());

				m_CurrentState.m_DepthStencilState.SetState(m_DesiredState.m_DepthStencilState.GetState());
				m_CurrentState.m_StencilRefState.SetState(m_DesiredState.m_StencilRefState.GetState());
				m_DesiredState.m_DepthStencilState.ResetTracking();
				m_DesiredState.m_StencilRefState.ResetTracking();
			}
		}
	}

	//--------------------------------------------------------------

	void cDX11OutputMergerStage::ApplyDesiredState(ID3D11DeviceContext *a_pDXContext)
	{
		ApplyDesiredRenderTargetStates(a_pDXContext);
		ApplyDesiredBlendAndDepthStencilStates(a_pDXContext);
	}

	//--------------------------------------------------------------

	const cDX11OutputMergerStageState &cDX11OutputMergerStage::GetCurrentState() const
	{
		return m_CurrentState;
	}
}