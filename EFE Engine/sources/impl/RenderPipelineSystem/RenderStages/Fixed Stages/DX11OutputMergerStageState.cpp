#include "impl/RenderPipelineSystem/RenderStages/Fixed Stages/DX11OutputMergerStageState.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cDX11OutputMergerStageState::cDX11OutputMergerStageState()
		: m_BlendState(0),
		m_DepthStencilState(0),
		m_StencilRefState(0),
		m_RenderTargetsViews(0),
		m_DepthTargetView(0),
		m_UnorderedAccessViews(0),
		m_UAVInitialCounts(-1),
		m_pSisterState(NULL)
	{
		CleateState();
	}

	//--------------------------------------------------------------

	cDX11OutputMergerStageState::~cDX11OutputMergerStageState()
	{

	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cDX11OutputMergerStageState::CleateState()
	{
		m_BlendState.InitializeState();
		m_DepthStencilState.InitializeState();
		m_StencilRefState.InitializeState();
		m_RenderTargetsViews.InitializeStates();
		m_DepthTargetView.InitializeState();
		m_UnorderedAccessViews.InitializeStates();
		m_UAVInitialCounts.InitializeStates();
	}

	//--------------------------------------------------------------

	void cDX11OutputMergerStageState::SetSisterState(cDX11OutputMergerStageState *a_pSister)
	{
		m_pSisterState = a_pSister;

		m_BlendState.SetSister(&m_pSisterState->m_BlendState);
		m_DepthStencilState.SetSister(&m_pSisterState->m_DepthStencilState);
		m_StencilRefState.SetSister(&m_pSisterState->m_StencilRefState);
		m_RenderTargetsViews.SetSister(&m_pSisterState->m_RenderTargetsViews);
		m_DepthTargetView.SetSister(&m_pSisterState->m_DepthTargetView);
		m_UnorderedAccessViews.SetSister(&m_pSisterState->m_UnorderedAccessViews);
		m_UAVInitialCounts.SetSister(&m_pSisterState->m_UAVInitialCounts);
	}

	//--------------------------------------------------------------

	void cDX11OutputMergerStageState::ResetUpdateFlags()
	{
		m_BlendState.ResetTracking();
		m_DepthStencilState.ResetTracking();
		m_StencilRefState.ResetTracking();
		m_RenderTargetsViews.ResetTracking();
		m_DepthTargetView.ResetTracking();
		m_UnorderedAccessViews.ResetTracking();
		m_UAVInitialCounts.ResetTracking();
	}

	//--------------------------------------------------------------

	int cDX11OutputMergerStageState::GetRenderTargetNum() const
	{
		size_t lCount = 0;

		for (size_t i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		{
			if (m_RenderTargetsViews.GetState(i) != 0)
				lCount++;
		}

		return lCount;
	}
}