#ifndef EFE_DX11OUTPUTMERGERSTAGE_STATE_H
#define EFE_DX11OUTPUTMERGERSTAGE_STATE_H

#include "system/StateMonitor.h"
#include <d3d11.h>

namespace efe
{
	class cDX11OutputMergerStageState
	{
	public:
		cDX11OutputMergerStageState();
		~cDX11OutputMergerStageState();

		void CleateState();
		void SetSisterState(cDX11OutputMergerStageState *a_pSister);
		void ResetUpdateFlags();

		int GetRenderTargetNum() const;

		StateMonitorl m_BlendState;
		StateMonitorl m_DepthStencilState;
		StateMonitorul m_StencilRefState;
		StateArrayMonitor<int, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> m_RenderTargetsViews;
		StateMonitorl m_DepthTargetView;
		StateArrayMonitor<int, D3D11_PS_CS_UAV_REGISTER_COUNT> m_UnorderedAccessViews;
		StateArrayMonitor<unsigned int, D3D11_PS_CS_UAV_REGISTER_COUNT> m_UAVInitialCounts;

	protected:
		cDX11OutputMergerStageState *m_pSisterState;
	};
};
#endif