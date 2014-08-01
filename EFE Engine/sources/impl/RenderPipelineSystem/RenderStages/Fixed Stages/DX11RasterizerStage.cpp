#include "impl/RenderPipelineSystem/RenderStages/Fixed Stages/DX11RasterizerStage.h"

#include "impl/LowLevelGraphicsDX11.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cDX11RasterizerStage::cDX11RasterizerStage()
	{
		m_DesiredState.SetSisterState(&m_CurrentState);
	}

	//--------------------------------------------------------------

	cDX11RasterizerStage::~cDX11RasterizerStage()
	{

	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cDX11RasterizerStage::ClearDesiredState()
	{
		m_DesiredState.ClearState();
	}

	//--------------------------------------------------------------

	void cDX11RasterizerStage::ClearCurrentState()
	{
		m_CurrentState.ClearState();
	}

	//--------------------------------------------------------------

	void cDX11RasterizerStage::ApplyDesiredState( ID3D11DeviceContext *a_pDXContext )
	{
		if (m_DesiredState.m_RasterizerState.IsUpdateNeeded())
		{
			cDX11RasterizerState *pRasterizerStage = (cDX11RasterizerState*)m_DesiredState.m_RasterizerState.GetState();

			if (pRasterizerStage)
				a_pDXContext->RSSetState(pRasterizerStage->m_pRasterState);
		}

		if (m_DesiredState.m_ViewportCount.IsUpdateNeeded() ||
			m_DesiredState.m_Viewports.IsUpdateNeeded())
		{
			D3D11_VIEWPORT vViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];

			for (int i = 0; i < m_DesiredState.m_ViewportCount.GetState(); i++)
			{
				cRectl ViewportRect = m_DesiredState.m_Viewports.GetState(i);
				D3D11_VIEWPORT ViewPort = {ViewportRect.x, ViewportRect.y, ViewportRect.w, ViewportRect.h, 0.0f, 1.0f};
				vViewports[i] = ViewPort;
			}

			a_pDXContext->RSSetViewports(m_DesiredState.m_ViewportCount.GetState(), vViewports);
		}

		if (m_DesiredState.m_ScissorRectCount.IsUpdateNeeded() ||
			m_DesiredState.m_ScissorRects.IsUpdateNeeded())
		{
			D3D11_RECT vRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];

			for (int i = 0; i < m_DesiredState.m_ViewportCount.GetState(); i++)
			{
// 				cRectl ViewportRect = m_DesiredState.m_ScissorRects.GetState(i);
// 				D3D11_VIEWPORT ViewPort = {ViewportRect.x, ViewportRect.y, ViewportRect.w, ViewportRect.h, 0.0f, 1.0f};
// 				vViewports[i] = ViewPort;
			}

			//a_pDXContext->RSSetViewports(m_DesiredState.m_ViewportCount.GetState(), vViewports);
		}

		m_DesiredState.ResetUpdateFlags();
		m_CurrentState = m_DesiredState;
	}

	//--------------------------------------------------------------

	const cDX11RasterizerStageState &cDX11RasterizerStage::GetCurrentState() const
	{
		return m_CurrentState;
	}
}