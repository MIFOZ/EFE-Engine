#include "graphics/Material_BaseLight.h"
#include "graphics/Renderer2D.h"

#include "resources/GpuProgramManager.h"

namespace efe
{
	iMaterial_BaseLight::iMaterial_BaseLight(const tString &a_sLightVertexProgram,
							const tString &a_sLightPixelProgram,
							
			const tString &a_sName,iLowLevelGraphics *a_pLowLevelGraphics,
			cImageManager *a_pImageManager, cTextureManager *a_pTextureManager,
			cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
			eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D)
			: iMaterial(a_sName, a_pLowLevelGraphics, a_pImageManager, a_pTextureManager, a_pRenderer, a_pProgramManager,
			a_Picture, a_pRenderer3D)
	{
		m_vParams[eMaterialRenderType_Perspective].m_bRender = true;
		m_vParams[eMaterialRenderType_Perspective].m_pEffect = new cMaterialRenderEffect();

		m_vParams[eMaterialRenderType_Perspective].m_pEffect->m_FillMode = eMaterialFillMode_Solid;

		m_bIsTransparent = false;
		m_bIsGlowing = false;
		m_bUsesLights = false;
		m_bUseColorSpecular = false;

		for (int i=0;i<eBaseLightProgram_LastEnum;i++)
		{
			m_vVtxPrograms[i] = NULL;
			m_vPxlPrograms[i] = NULL;
		}

		m_vVtxPrograms[0] = m_pProgramManager->CreateProgram("Diffuse_Color_vp.hlsl", "main", eGpuProgramType_Vertex);
		m_vPxlPrograms[0] = m_pProgramManager->CreateProgram("Diffuse_Color_pp.hlsl", "main", eGpuProgramType_Pixel);

		m_vParams[eMaterialRenderType_Perspective].m_pEffect->SetProgram(m_vVtxPrograms[0], eGpuProgramType_Vertex);
		m_vParams[eMaterialRenderType_Perspective].m_pEffect->SetProgram(m_vPxlPrograms[0], eGpuProgramType_Pixel);
	}

	iMaterial_BaseLight::~iMaterial_BaseLight()
	{
		
	}

	tTextureTypeList iMaterial_BaseLight::GetTextureTypes()
	{
		tTextureTypeList vTypes;
		vTypes.push_back(cTextureType("", eMaterialTexture_Diffuse));
		if (m_bUseNormalMap)
			vTypes.push_back(cTextureType("_bump", eMaterialTexture_NMap));

		return vTypes;
	}

	bool iMaterial_BaseLight::UsesType(eMaterialRenderType a_Type)
	{
		if (a_Type == eMaterialRenderType_Perspective)
			return true;
		return false;
	}

	int iMaterial_BaseLight::GetNumOfPasses(eMaterialRenderType a_Type, iLight3D *a_pLight)
	{
		return 1;
	}
}