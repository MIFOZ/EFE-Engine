#ifndef EFE_DX11_OUTPUTMERGERSTAGE_H
#define EFE_DX11_OUTPUTMERGERSTAGE_H

#include "impl/RenderPipelineSystem/RenderStages/Fixed Stages/DX11OutputMergerStageState.h"

namespace efe
{
	class cDX11OutputMergerStage
	{
	public:
		cDX11OutputMergerStage();
		~cDX11OutputMergerStage();

		void ClearDesiredState();
		void ClearCurrentState();
		void ApplyDesiredRenderTargetStates(ID3D11DeviceContext *a_pDXContext);
		void ApplyDesiredBlendAndDepthStencilStates(ID3D11DeviceContext *a_pDXContext);
		void ApplyDesiredState(ID3D11DeviceContext *a_pDXContext);

		const cDX11OutputMergerStageState &GetCurrentState() const;

		cDX11OutputMergerStageState m_DesiredState;

	private:
		cDX11OutputMergerStageState m_CurrentState;
	};
};
#endif