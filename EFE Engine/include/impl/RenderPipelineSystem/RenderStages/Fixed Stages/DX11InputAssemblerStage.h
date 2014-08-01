#ifndef EFE_DX11_INPUTASSEMBLER_STAGE_H
#define EFE_DX11_INPUTASSEMBLER_STAGE_H

#include "impl/RenderPipelineSystem/RenderStages/Fixed Stages/DX11InputAssemblerStageState.h"

namespace efe
{
	class cDX11InputAssemblerStage
	{
	public:
		cDX11InputAssemblerStage();
		~cDX11InputAssemblerStage();

		void ClearDesiredState();
		void ClearCurrentState();
		void ApplyDesiredState(ID3D11DeviceContext *a_pDXContext);

		const cDX11InputAssemblerStageState &GetCurrentState() const;

		cDX11InputAssemblerStageState m_DesiredState;

	private:
		cDX11InputAssemblerStageState m_CurrentState;
	};
};
#endif