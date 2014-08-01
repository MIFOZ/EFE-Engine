#ifndef EFE_MATERIAL_BASE_LIGHT_H
#define EFE_MATERIAL_BASE_LIGHT_H

#include <vector>
#include "graphics/Material.h"
#include "scene/Light3D.h"

#include "graphics/MaterialRenderEffect.h"

enum eBaseLightProgram
{
	eBaseLightProgram_Point1 = 0,
	eBaseLightProgram_Point2 = 1,
	eBaseLightProgram_Point3 = 2,
	eBaseLightProgram_Spot1 = 3,
	eBaseLightProgram_Spot2 = 4,
	eBaseLightProgram_Spot3 = 5,
	eBaseLightProgram_LastEnum = 6
};

namespace efe
{
	class iMaterial_BaseLight : public iMaterial
	{
	public:
		iMaterial_BaseLight(const tString &a_sLightVertexProgram,
							const tString &a_sLightPixelProgram,
							
			const tString &a_sName,iLowLevelGraphics *a_pLowLevelGraphics,
			cImageManager *a_pImageManager, cTextureManager *a_pTextureManager,
			cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
			eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D);

		virtual ~iMaterial_BaseLight();

		tTextureTypeList GetTextureTypes();

		bool UsesType(eMaterialRenderType a_Type);

		int GetNumOfPasses(eMaterialRenderType a_Type, iLight3D *a_pLight);

		void Compile(){}
		bool StartRendering(eMaterialRenderType a_Type, iCamera *a_pCam, iLight *a_pLight){return false;}
		eMaterialType GetType(eMaterialRenderType a_Type){return eMaterialType_Diffuse;}
	protected:
		iTexture *m_pNormalizationMap;
		iTexture *m_pSpotNegativeRejectMap;

		bool m_bUsesTwoPassSpot;

		bool m_bUseSpecular;
		bool m_bUseNormalMap;
		bool m_bUseColorSpecular;

		iGpuProgram *m_pSimpleFP;
		iGpuProgram *m_pAmbientFP;

		iGpuProgram *m_vVtxPrograms[eBaseLightProgram_LastEnum];
		iGpuProgram *m_vPxlPrograms[eBaseLightProgram_LastEnum];
	};
};

#endif