#include "impl/RenderPipelineSystem/RenderStages/Programmable/DX11ProgramStageState.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cDX11ProgramStageState::cDX11ProgramStageState()
		: m_Program(0),
		m_ConstantBuffers(NULL),
		m_SamplerStates(NULL),
		m_ShaderResourceViews(NULL),
		m_UnorderedAccessViews(NULL),
		m_UAVInitialCounts(-1),
		m_pSisterState(NULL)
	{
		ClearState();
	}

	//--------------------------------------------------------------

	cDX11ProgramStageState::~cDX11ProgramStageState()
	{

	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cDX11ProgramStageState::ClearState()
	{
		m_Program.InitializeState();
		m_ConstantBuffers.InitializeStates();
		m_SamplerStates.InitializeStates();
		m_ShaderResourceViews.InitializeStates();
		m_UnorderedAccessViews.InitializeStates();
		m_UAVInitialCounts.InitializeStates();
	}

	//--------------------------------------------------------------

	void cDX11ProgramStageState::SetSisterState(cDX11ProgramStageState *a_pSister)
	{
		m_pSisterState = a_pSister;
		m_Program.SetSister(&a_pSister->m_Program);
		m_ConstantBuffers.SetSister(&a_pSister->m_ConstantBuffers);
		m_SamplerStates.SetSister(&a_pSister->m_SamplerStates);
		m_ShaderResourceViews.SetSister(&a_pSister->m_ShaderResourceViews);
		m_UnorderedAccessViews.SetSister(&a_pSister->m_UnorderedAccessViews);
		m_UAVInitialCounts.SetSister(&a_pSister->m_UAVInitialCounts);
	}

	//--------------------------------------------------------------

	void cDX11ProgramStageState::ResetUpdateFlags()
	{
		m_Program.ResetTracking();
		m_ConstantBuffers.ResetTracking();
		m_SamplerStates.ResetTracking();
		m_ShaderResourceViews.ResetTracking();
		m_UnorderedAccessViews.ResetTracking();
		m_UAVInitialCounts.ResetTracking();
	}
}