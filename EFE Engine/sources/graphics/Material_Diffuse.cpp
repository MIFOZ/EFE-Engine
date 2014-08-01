#include "graphics/Material_Diffuse.h"

#include "graphics/Material_Fallback01_BaseLight.h"

namespace efe
{
	cMaterial_Diffuse::cMaterial_Diffuse(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics,
		cImageManager *a_pImageManager, cTextureManager *a_pTextureManager,
		cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
		eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D)
		: iMaterial_BaseLight(	"Diffuse_Light_vp.hlsl",
								"Diffuse_Light_pp.hlsl",
			a_sName, a_pLowLevelGraphics, a_pImageManager, a_pTextureManager, a_pRenderer, a_pProgramManager,
			a_Picture, a_pRenderer3D)
	{
		m_vParams[eMaterialRenderType_Perspective].m_pEffect->m_AlphaMode = eMaterialAlphaMode_Solid;
		m_vParams[eMaterialRenderType_Perspective].m_pEffect->m_BlendMode = eMaterialBlendMode_None;

		m_bUseSpecular = false;
		m_bUseNormalMap = true;
	}

	cMaterial_Diffuse::~cMaterial_Diffuse()
	{
	}

	cMaterialType_Diffuse::cMaterialType_Diffuse()
	{
		m_lTechLevel = 0;
	}

	iMaterial *cMaterialType_Diffuse::Create(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics,
			cImageManager *a_pImageManager, cTextureManager *a_pTextureManager,
			cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
			eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D)
	{
		if (iMaterial::GetQuality() >= eMaterialQuality_High)
			return efeNew(cMaterial_Diffuse, (a_sName, a_pLowLevelGraphics,
									a_pImageManager, a_pTextureManager, a_pRenderer, 
									a_pProgramManager, a_Picture, a_pRenderer3D));
	}
}