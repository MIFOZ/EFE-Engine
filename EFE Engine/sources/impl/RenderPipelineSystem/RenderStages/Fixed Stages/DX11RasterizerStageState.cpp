#include "impl/RenderPipelineSystem/RenderStages/Fixed Stages/DX11RasterizerStageState.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cDX11RasterizerStageState::cDX11RasterizerStageState()
		: m_RasterizerState(0),
		m_ViewportCount(0),
		m_Viewports(cRectl()),
		m_ScissorRectCount(0),
		m_ScissorRects(D3D11_RECT()),
		m_pSisterState(NULL)
	{
		ClearState();
	}

	//--------------------------------------------------------------

	cDX11RasterizerStageState::~cDX11RasterizerStageState()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cDX11RasterizerStageState::ClearState()
	{
		m_RasterizerState.InitializeState();
		m_ViewportCount.InitializeState();
		m_Viewports.InitializeStates();
		m_ScissorRectCount.InitializeState();
		m_ScissorRects.InitializeStates();
	}

	//--------------------------------------------------------------

	void cDX11RasterizerStageState::SetSisterState(cDX11RasterizerStageState *a_pSister)
	{
		m_pSisterState = a_pSister;

		m_RasterizerState.SetSister(&a_pSister->m_RasterizerState);
		m_ViewportCount.SetSister(&a_pSister->m_ViewportCount);
		m_Viewports.SetSister(&a_pSister->m_Viewports);
		m_ScissorRectCount.SetSister(&a_pSister->m_ScissorRectCount);
		m_ScissorRects.SetSister(&a_pSister->m_ScissorRects);
	}

	//--------------------------------------------------------------

	void cDX11RasterizerStageState::ResetUpdateFlags()
	{
		m_RasterizerState.ResetTracking();
		m_ViewportCount.ResetTracking();
		m_Viewports.ResetTracking();
		m_ScissorRectCount.ResetTracking();
		m_ScissorRects.ResetTracking();
	}
}