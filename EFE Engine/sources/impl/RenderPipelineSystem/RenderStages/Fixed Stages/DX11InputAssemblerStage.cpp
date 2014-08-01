#include "impl/RenderPipelineSystem/RenderStages/Fixed Stages/DX11InputAssemblerStage.h"
#include "impl/DX11VertexBuffer.h"

#include "graphics/LowLevelGraphics.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cDX11InputAssemblerStage::cDX11InputAssemblerStage()
	{
		m_DesiredState.SetSisterState(&m_CurrentState);
	}

	//--------------------------------------------------------------

	cDX11InputAssemblerStage::~cDX11InputAssemblerStage()
	{

	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cDX11InputAssemblerStage::ClearDesiredState()
	{
		m_DesiredState.ClearState();
	}

	//--------------------------------------------------------------

	void cDX11InputAssemblerStage::ClearCurrentState()
	{
		m_CurrentState.ClearState();
	}

	//--------------------------------------------------------------

	void cDX11InputAssemblerStage::ApplyDesiredState( ID3D11DeviceContext *a_pDXContext )
	{
		if (m_DesiredState.m_InputLayout.IsUpdateNeeded())
		{
			ID3D11InputLayout *pLayout = (ID3D11InputLayout*)m_DesiredState.m_InputLayout.GetState();

			if (pLayout)
			{
				a_pDXContext->IASetInputLayout(pLayout);
			}
			else
				Error("Trying to bind invalid input layout");
		}

		if (m_DesiredState.m_PrimitiveTopology.IsUpdateNeeded())
		{
			a_pDXContext->IASetPrimitiveTopology(m_DesiredState.m_PrimitiveTopology.GetState());
		}

		if (m_DesiredState.m_VertexBuffers.IsUpdateNeeded() ||
			m_DesiredState.m_VertexBufferStrides.IsUpdateNeeded() ||
			m_DesiredState.m_VertexBufferOffsets.IsUpdateNeeded())
		{
			ID3D11Buffer *vBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {NULL};

			for (int i = 0; i < m_DesiredState.GetAvailableSlotCount(); i++)
			{
				ID3D11Buffer *pBuffer = (ID3D11Buffer *)m_DesiredState.m_VertexBuffers.GetState(i);
				vBuffers[i] = pBuffer;
			}

			UINT lStartSlot = min(m_DesiredState.m_VertexBuffers.GetStartSlot(),
				min(m_DesiredState.m_VertexBufferStrides.GetStartSlot(),
				m_DesiredState.m_VertexBufferOffsets.GetStartSlot()));
			UINT lEndSlot = max(m_DesiredState.m_VertexBuffers.GetEndSlot(),
				max(m_DesiredState.m_VertexBufferStrides.GetEndSlot(),
				m_DesiredState.m_VertexBufferOffsets.GetEndSlot()));

			int Stride = m_DesiredState.m_VertexBufferStrides.GetSlotLocation(lStartSlot)[0];

			a_pDXContext->IASetVertexBuffers(lStartSlot,lEndSlot - lStartSlot + 1,
				vBuffers, m_DesiredState.m_VertexBufferStrides.GetSlotLocation(lStartSlot),
				m_DesiredState.m_VertexBufferOffsets.GetSlotLocation(lStartSlot));
		}

		if (m_DesiredState.m_IndexBuffer.IsUpdateNeeded())
		{
			ID3D11Buffer *pBuffer = (ID3D11Buffer *)m_DesiredState.m_IndexBuffer.GetState();
			a_pDXContext->IASetIndexBuffer(pBuffer, m_DesiredState.m_IndexBufferFormat.GetState(), 0);
		}

		m_DesiredState.ResetUpdateFlags();
		m_CurrentState = m_DesiredState;
	}

	//--------------------------------------------------------------

	const cDX11InputAssemblerStageState &cDX11InputAssemblerStage::GetCurrentState() const
	{
		return m_CurrentState;
	}
}