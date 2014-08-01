#ifndef EFE_RENDER_EFFECT_H
#define EFE_RENDER_EFFECT_H

#include "graphics/GPUProgram.h"
#include "graphics/GpuProgramParameter.h"

#include "graphics/Material.h"


namespace efe
{
	class iBlendState;
	class iDepthStencilState;
	class iRasterizerState;
	class iLowLevelGraphics;
	class cGpuProgramParameterManager;

	class cMaterialRenderEffect
	{
		friend class iMaterial;

	public:
		cMaterialRenderEffect();
		~cMaterialRenderEffect();

		void SetProgram(iGpuProgram *a_pProgram, eGpuProgramType a_Type);
		iGpuProgram *GetProgram(eGpuProgramType a_Type);

		void ConfigurePipeline(iLowLevelGraphics *a_pLowGfx, cGpuProgramParameterManager *a_pParamManager);

		//eMaterialCullMode m_CullMode;
		eMaterialFillMode m_FillMode;

		eMaterialAlphaMode m_AlphaMode;
		eMaterialBlendMode m_BlendMode;

		iBlendState *m_pBlendState;
		iDepthStencilState *m_pDepthStencilState;
		iRasterizerState *m_pRasterizerState;
		unsigned int m_lStencilRef;

	private:
		void UpdateConstantBufferList();

		eFillMode GetFillModeType(eMaterialFillMode a_Mode);

		iGpuProgram *m_vPrograms[eGpuProgramType_LastEnum];

		//tTextureVec m_vTexture;
		//tResourceImageVec m_vImage;

		tGpuProgramParameterVec m_vUniqueConstBuffers;
	};
};

#endif