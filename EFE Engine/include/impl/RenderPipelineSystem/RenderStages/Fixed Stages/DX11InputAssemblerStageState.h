#ifndef EFE_DX11_INPUTASSEMBLER_STAGE_STATE_H
#define EFE_DX11_INPUTASSEMBLER_STAGE_STATE_H

#include "system/StateMonitor.h"

#include <d3d11.h>

namespace efe
{
	class cDX11InputAssemblerStageState
	{
	public:
		cDX11InputAssemblerStageState();
		~cDX11InputAssemblerStageState();

		void ClearState();
		void SetSisterState(cDX11InputAssemblerStageState *a_pSister);
		void ResetUpdateFlags();

		unsigned int GetAvailableSlotCount();

		StateMonitorul m_IndexBuffer;
		StateMonitor<DXGI_FORMAT> m_IndexBufferFormat;
		StateArrayMonitor<unsigned int, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT> m_VertexBuffers;
		StateArrayMonitor<unsigned int, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT> m_VertexBufferStrides;
		StateArrayMonitor<unsigned int, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT> m_VertexBufferOffsets;
		StateMonitorul m_InputLayout;
		StateMonitor<D3D11_PRIMITIVE_TOPOLOGY> m_PrimitiveTopology;

	private:
		cDX11InputAssemblerStageState *m_pSisterState;

		unsigned int m_lAvailableSlotCount;
	};
};
#endif
