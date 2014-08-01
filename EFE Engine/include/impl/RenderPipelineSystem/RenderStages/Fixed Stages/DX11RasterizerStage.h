#ifndef EFE_DX11_RASTERIZER_STAGE_H
#define EFE_DX11_RASTERIZER_STAGE_H

#include "impl/RenderPipelineSystem/RenderStages/Fixed Stages/DX11RasterizerStageState.h"

namespace efe
{
	class cDX11RasterizerStage
	{
	public:
		cDX11RasterizerStage();
		~cDX11RasterizerStage();

		void ClearDesiredState();
		void ClearCurrentState();
		void ApplyDesiredState(ID3D11DeviceContext *a_pDXContext);

		const cDX11RasterizerStageState &GetCurrentState() const;

		cDX11RasterizerStageState m_DesiredState;

	private:
		cDX11RasterizerStageState m_CurrentState;
	};
};
#endif