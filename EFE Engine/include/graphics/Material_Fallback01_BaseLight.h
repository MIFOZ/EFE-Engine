#ifndef EFE_MATERIAL_FALLBACK01_BASE_LIGHT_H
#define EFE_MATERIAL_FALLBACK01_BASE_LIGHT_H

#include <vector>
#include "graphics/Material.h"
#include "scene/Light3D.h"

#include "graphics/Material_BaseLight.h"

namespace efe
{
	/*class iMaterial_Fallback01_BaseLight : public iMaterial
	{
	public:
		iMaterial_Fallback01_BaseLight(bool a_bNormalMap, bool a_bSpecular,
			const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics,
			cImageManager *a_pImageManager, cTextureManager *a_pTextureManager,
			cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
			eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D);

		virtual ~iMaterial_Fallback01_BaseLight();

		tTextureTypeList GetTextureTypes();

		bool UsesTypes(eMaterialRenderType a_Type);

		iGpuProgram *GetVertexProgram(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight);
		bool VertexProgramUsesLight(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight);
		bool VertexProgramUsesEye(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight);

		iGpuProgram *GetPixelProgram(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight);

		eMaterialAlphaMode GetAlphaMode(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight);
		eMaterialBlendMode GetBlendMode(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight);
		eMaterialChannelMode GetChannelMode(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight);

		iTexture *GetTexture(int a_lUnit, eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight);
		eMaterialBlendMode GetTextureBlend(int a_lUnit, eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight);

		int GetNumOfPasses(eMaterialRenderType a_Type, iLight3D *a_pLight);

		void Compile(){}
		bool StartRendering(eMaterialRenderType a_Type, iCamera *a_pCam, iLight *a_pLight){return false;}
		void EndRendering(eMaterialRenderType a_Type){}
		tVtxBatchFlag GetBatchFlags(eMaterialRenderType a_Type){return 0;}
		bool NextPass(eMaterialRenderType a_Type){return false;}
		bool HasMultiplePasses(eMaterialRenderType a_Type){return false;}
		eMaterialType GetType(eMaterialRenderType a_Type){return eMaterialType_Diffuse;}
		void EditVertices(eMaterialRenderType a_Type, iCamera *a_pCam, iLight *a_pLight,
			tVertexVec *a_pVtxVec, cVector3f *a_pTransform, unsigned int a_lIndexAdd){}

	protected:
		iTexture *m_pNormalizationMap;
		iTexture *m_pSpotNegativeRejectMap;

		bool m_bUseSpecular;
		bool m_bUseNormalMap;

		bool m_bUsesTwoPassSpot;

		iTexture *m_pAttenuationMap;

		iGpuProgram *m_vVtxPrograms[eBaseLightProgram_LastEnum];
		iGpuProgram *m_vPxlPrograms[eBaseLightProgram_LastEnum];
	};

	class cDXState_Diffuse : public iDXStateProgram
	{
	public:
		cDXState_Diffuse();

		void Bind();
		void UnBind();
	private:
		void InitData(){}
	};*/
};
#endif