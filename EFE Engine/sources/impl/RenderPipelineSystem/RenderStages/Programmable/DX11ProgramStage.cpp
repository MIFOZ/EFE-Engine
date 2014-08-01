#include "impl/RenderPipelineSystem/RenderStages/Programmable/DX11ProgramStage.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cDX11ProgramStage::cDX11ProgramStage(eGpuProgramType a_Type)
	{
		m_Type = a_Type;

		m_DesiredState.SetSisterState(&m_CurrentState);
	}

	//--------------------------------------------------------------

	cDX11ProgramStage::~cDX11ProgramStage()
	{

	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cDX11ProgramStage::ClearDesiredState()
	{
		m_DesiredState.ClearState();
	}

	//--------------------------------------------------------------

	void cDX11ProgramStage::ClearCurrentState()
	{
		m_CurrentState.ClearState();
	}

	//--------------------------------------------------------------

	void cDX11ProgramStage::ApplyDesiredState(ID3D11DeviceContext *a_pDXContext)
	{
		if (m_DesiredState.m_Program.IsUpdateNeeded())
			BindProgram(a_pDXContext);

		if (m_DesiredState.m_ConstantBuffers.IsUpdateNeeded())
			BindConstantBuffer(a_pDXContext, 0);

		if (m_DesiredState.m_SamplerStates.IsUpdateNeeded())
			BindSamplerStates(a_pDXContext, 0);

		if (m_DesiredState.m_ShaderResourceViews.IsUpdateNeeded())
			BindShaderResourceViews(a_pDXContext, 0);

		if (m_DesiredState.m_UnorderedAccessViews.IsUpdateNeeded() ||
			m_DesiredState.m_UAVInitialCounts.IsUpdateNeeded())
			BindUnorderedAccessViews(a_pDXContext, 1);

		m_DesiredState.ResetUpdateFlags();
		m_CurrentState = m_DesiredState;
	}

	//--------------------------------------------------------------

	eGpuProgramType cDX11ProgramStage::GetType()
	{
		return m_Type;
		iGpuProgram *pProg = (iGpuProgram*)m_DesiredState.m_Program.GetState();
		if (pProg)
			return pProg->GetType();

		FatalError("Getting type form NULL program\n!");
		return eGpuProgramType_LastEnum;
	}

	//--------------------------------------------------------------

	void cDX11ProgramStage::BindProgram(ID3D11DeviceContext *a_pDXContext)
	{
		iGpuProgram *pProg = (iGpuProgram*)m_DesiredState.m_Program.GetState();
		if (pProg)
			pProg->Bind();
	}

	//--------------------------------------------------------------

	void cDX11ProgramStage::BindConstantBuffer(ID3D11DeviceContext *a_pDXContext, int a_lCount)
	{
		UINT lStartSlot = m_DesiredState.m_ConstantBuffers.GetStartSlot();
		UINT lNum = m_DesiredState.m_ConstantBuffers.GetRange();
		ID3D11Buffer **pBuf = m_DesiredState.m_ConstantBuffers.GetFirstSlotLocation();

		switch (m_Type)
		{
		case efe::eGpuProgramType_Vertex:
			a_pDXContext->VSSetConstantBuffers(lStartSlot,lNum,pBuf);
			break;
		case efe::eGpuProgramType_Geometry:
			a_pDXContext->GSSetConstantBuffers(lStartSlot,lNum,pBuf);
			break;
		case efe::eGpuProgramType_Pixel:
			a_pDXContext->PSSetConstantBuffers(lStartSlot,lNum,pBuf);
			break;
		default: break;
		}
	}

	//--------------------------------------------------------------

	void cDX11ProgramStage::BindSamplerStates(ID3D11DeviceContext *a_pDXContext, int a_lCount)
	{
		UINT lStartSlot = m_DesiredState.m_SamplerStates.GetStartSlot();
		UINT lNum = m_DesiredState.m_SamplerStates.GetRange();
		ID3D11SamplerState **pState = m_DesiredState.m_SamplerStates.GetFirstSlotLocation();

		switch (m_Type)
		{
		case efe::eGpuProgramType_Vertex:
			a_pDXContext->PSSetSamplers(lStartSlot,lNum,pState);
			break;
		case efe::eGpuProgramType_Geometry:
			a_pDXContext->PSSetSamplers(lStartSlot,lNum,pState);
			break;
		case efe::eGpuProgramType_Pixel:
			a_pDXContext->PSSetSamplers(lStartSlot,lNum,pState);
			break;
		default: break;
		}
	}

	//--------------------------------------------------------------

	void cDX11ProgramStage::BindShaderResourceViews(ID3D11DeviceContext *a_pDXContext, int a_lCount)
	{
		UINT lStartSlot = m_DesiredState.m_ShaderResourceViews.GetStartSlot();
		UINT lNum = m_DesiredState.m_ShaderResourceViews.GetRange();
		ID3D11ShaderResourceView **pSRV = m_DesiredState.m_ShaderResourceViews.GetFirstSlotLocation();

		switch (m_Type)
		{
		case efe::eGpuProgramType_Vertex:
			a_pDXContext->VSSetShaderResources(lStartSlot,lNum,pSRV);
			break;
		case efe::eGpuProgramType_Geometry:
			a_pDXContext->GSSetShaderResources(lStartSlot,lNum,pSRV);
			break;
		case efe::eGpuProgramType_Pixel:
			a_pDXContext->PSSetShaderResources(lStartSlot,lNum,pSRV);
			break;
		default: break;
		}
	}

	//--------------------------------------------------------------

	void cDX11ProgramStage::BindUnorderedAccessViews(ID3D11DeviceContext *a_pDXContext, int a_lCount)
	{
		
	}
}