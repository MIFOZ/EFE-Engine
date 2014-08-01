#ifndef EFE_DX11PROGRAMSTATESTATE_H
#define EFE_DX11PROGRAMSTATESTATE_H

#include "system/StateMonitor.h"

#include <d3d11.h>

namespace efe
{
	class cDX11ProgramStageState
	{
	public:
		cDX11ProgramStageState();
		~cDX11ProgramStageState();

		void ClearState();

		void SetSisterState(cDX11ProgramStageState *a_pSister);
		void ResetUpdateFlags();

		StateMonitorul m_Program;
		StateArrayMonitor<ID3D11Buffer*, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT> m_ConstantBuffers;
		StateArrayMonitor<ID3D11SamplerState*, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT> m_SamplerStates;
		StateArrayMonitor<ID3D11ShaderResourceView*, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT> m_ShaderResourceViews;
		StateArrayMonitor<ID3D11UnorderedAccessView*, D3D11_PS_CS_UAV_REGISTER_COUNT> m_UnorderedAccessViews;
		StateArrayMonitor<unsigned int, D3D11_PS_CS_UAV_REGISTER_COUNT> m_UAVInitialCounts;

	protected:
		cDX11ProgramStageState *m_pSisterState;
	};
};
#endif