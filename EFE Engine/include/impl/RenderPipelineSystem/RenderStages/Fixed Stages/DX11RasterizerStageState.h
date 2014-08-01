#ifndef EFE_DX11_RASTERIZER_STAGE_STATE_H
#define EFE_DX11_RASTERIZER_STAGE_STATE_H

#include "system/StateMonitor.h"

#include "math/MathTypes.h"

#include <d3d11.h>

namespace efe
{
	class cDX11RasterizerStageState
	{
	public:
		cDX11RasterizerStageState();
		~cDX11RasterizerStageState();

		void ClearState();
		void SetSisterState(cDX11RasterizerStageState *a_pSister);
		void ResetUpdateFlags();

		StateMonitorul m_RasterizerState;
		StateMonitorl m_ViewportCount;
		StateArrayMonitor<cRectl, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE> m_Viewports;
		StateMonitorl m_ScissorRectCount;
		StateArrayMonitor<D3D11_RECT, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE> m_ScissorRects;

	private:
		cDX11RasterizerStageState *m_pSisterState;
	};
};

#endif