#ifndef EFE_DX11PROGRAMSTAGE_H
#define EFE_DX11PROGRAMSTAGE_H

#include "impl/RenderPipelineSystem/RenderStages/Programmable/DX11ProgramStageState.h"

#include "graphics/GPUProgram.h"

namespace efe
{
	class cDX11ProgramStage
	{
	public:
		cDX11ProgramStage(eGpuProgramType a_Type);
		~cDX11ProgramStage();

		void ClearDesiredState();
		void ClearCurrentState();
		void ApplyDesiredState(ID3D11DeviceContext *a_pDXContext);

		eGpuProgramType GetType();

		void BindProgram(ID3D11DeviceContext *a_pDXContext);
		void BindConstantBuffer(ID3D11DeviceContext *a_pDXContext, int a_lCount);
		void BindSamplerStates(ID3D11DeviceContext *a_pDXContext, int a_lCount);
		void BindShaderResourceViews(ID3D11DeviceContext *a_pDXContext, int a_lCount);
		void BindUnorderedAccessViews(ID3D11DeviceContext *a_pDXContext, int a_lCount);

		cDX11ProgramStageState m_DesiredState;

	private:
		eGpuProgramType m_Type;

		cDX11ProgramStageState m_CurrentState;
	};
};
#endif