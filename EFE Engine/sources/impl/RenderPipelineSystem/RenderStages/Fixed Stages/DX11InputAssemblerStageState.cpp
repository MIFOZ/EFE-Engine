#include "impl/RenderPipelineSystem/RenderStages/Fixed Stages/DX11InputAssemblerStageState.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cDX11InputAssemblerStageState::cDX11InputAssemblerStageState()
		: m_IndexBuffer(0),
		m_IndexBufferFormat(DXGI_FORMAT_R32_UINT),
		m_VertexBuffers(0),
		m_VertexBufferStrides(0),
		m_VertexBufferOffsets(0),
		m_InputLayout(0),
		m_PrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED),
		m_pSisterState(NULL),
		m_lAvailableSlotCount(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT)
	{
		ClearState();
	}

	//--------------------------------------------------------------

	cDX11InputAssemblerStageState::~cDX11InputAssemblerStageState()
	{

	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cDX11InputAssemblerStageState::ClearState()
	{
		m_IndexBuffer.InitializeState();
		m_IndexBufferFormat.InitializeState();
		m_VertexBuffers.InitializeStates();
		m_VertexBufferStrides.InitializeStates();
		m_VertexBufferOffsets.InitializeStates();
		m_InputLayout.InitializeState();
		m_PrimitiveTopology.InitializeState();
	}

	//--------------------------------------------------------------

	void cDX11InputAssemblerStageState::SetSisterState(cDX11InputAssemblerStageState *a_pSister)
	{
		m_pSisterState = a_pSister;

		m_IndexBuffer.SetSister(&a_pSister->m_IndexBuffer);
		m_IndexBufferFormat.SetSister(&a_pSister->m_IndexBufferFormat);
		m_VertexBuffers.SetSister(&a_pSister->m_VertexBuffers);
		m_VertexBufferStrides.SetSister(&a_pSister->m_VertexBufferStrides);
		m_VertexBufferOffsets.SetSister(&a_pSister->m_VertexBufferOffsets);
		m_InputLayout.SetSister(&a_pSister->m_InputLayout);
		m_PrimitiveTopology.SetSister(&a_pSister->m_PrimitiveTopology);
	}

	//--------------------------------------------------------------

	void cDX11InputAssemblerStageState::ResetUpdateFlags()
	{
		m_IndexBuffer.ResetTracking();
		m_IndexBufferFormat.ResetTracking();
		m_VertexBuffers.ResetTracking();
		m_VertexBufferStrides.ResetTracking();
		m_VertexBufferOffsets.ResetTracking();
		m_InputLayout.ResetTracking();
		m_PrimitiveTopology.ResetTracking();
	}

	//--------------------------------------------------------------

	unsigned int cDX11InputAssemblerStageState::GetAvailableSlotCount()
	{
		return m_lAvailableSlotCount;
	}
}