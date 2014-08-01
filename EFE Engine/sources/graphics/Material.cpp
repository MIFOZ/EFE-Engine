#include "graphics/Material.h"
//#include "graphics/Renderer2D.h"
#include "graphics/Renderer3D.h"
#include "system/LowLevelSystem.h"
#include "resources/TextureManager.h"
#include "resources/ImageManager.h"
#include "graphics/GPUProgram.h"
#include "scene/Camera.h"
#include "system/String.h"
#include "graphics/MaterialRenderEffect.h"

namespace efe
{
	eMaterialQuality iMaterial::m_Quality = eMaterialQuality_High;

	iMaterial::iMaterial(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics,
			cImageManager *a_pImageManager, cTextureManager *a_pTextureManager,
			cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
			eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D)
			: iResourceBase(a_sName, 0)
	{
		if (a_Picture==eMaterialPicture_Image)
		{
			m_vImage.resize(eMaterialTexture_LastEnum);
			m_vImage.assign(m_vImage.size(), NULL);
		}
		else
		{
			m_vTextureParams.resize(eMaterialTexture_LastEnum);
			//m_vTexture.resize(eMaterialTexture_LastEnum);
			m_vTextureParams.assign(m_vTextureParams.size(), cMaterialTextureParam());
			//m_vTexture.assign(m_vTexture.size(), NULL);
		}

		m_Type = eMaterialType_Null;
		m_Picture = a_Picture;

		m_pLowLevelGraphics = a_pLowLevelGraphics;
		m_pImageManager = a_pImageManager;
		m_pTextureManager = a_pTextureManager;
		m_pRenderer = a_pRenderer;
		m_pRenderer3D = a_pRenderer3D;
		//m_pRenderSettings = m_pRenderer3D->getr
		m_pProgramManager = a_pProgramManager;

		for (int i = 0; i < (int)eMaterialRenderType_LastEnum; i++)
		{
			m_vParams[i].m_bRender = false;
			m_vParams[i].m_pEffect = NULL;
		}

		m_bUsesLights = false;
		m_bIsTransparent = false;
		m_bIsGlowing  = false;
		m_bHasAlpha = false;
		m_bDepthTest = true;
		m_fValue = 1;

		for (int i=0;i<2;i++)
			for (int j=0;j<kMaxProgramNum;j++) m_pProgram[i][j]=NULL;

		m_lPassCount = 0;

		m_lId = -1;
	}

	iMaterial::~iMaterial()
	{
		int i;

		for (i=0;i<(int)m_vImage.size();i++)
			if (m_vImage[i])
				m_pImageManager->Destroy(m_vImage[i]);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iGpuProgram *iMaterial::GetVertexProgram(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight)
	{
		if (m_vParams[a_Type].m_pEffect)
			return m_vParams[a_Type].m_pEffect->m_vPrograms[eGpuProgramType_Vertex];
		else
			return NULL;
	}

	//--------------------------------------------------------------

	iGpuProgram *iMaterial::GetPixelProgram(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight)
	{
		if (m_vParams[a_Type].m_pEffect)
			return m_vParams[a_Type].m_pEffect->m_vPrograms[eGpuProgramType_Pixel];
		else
			return NULL;
	}

	//--------------------------------------------------------------

	eMaterialAlphaMode iMaterial::GetAlphaMode(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight)
	{
		if (m_vParams[a_Type].m_pEffect)
			return m_vParams[a_Type].m_pEffect->m_AlphaMode;
		else
			return eMaterialAlphaMode_Solid;
	}

	//--------------------------------------------------------------

	eMaterialBlendMode iMaterial::GetBlendMode(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight)
	{
		if (m_vParams[a_Type].m_pEffect)
			return m_vParams[a_Type].m_pEffect->m_BlendMode;
		else
			return eMaterialBlendMode_Replace;
	}

	//--------------------------------------------------------------

	iTexture *iMaterial::GetTexture(int a_lUnit, eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight)
	{
		return m_vTextureParams[eMaterialTexture_Diffuse].m_pTexture;
	}

	//--------------------------------------------------------------

	cRectf iMaterial::GetTextureOffset(eMaterialTexture a_Type)
	{
		cRectf SizeRect;

		if (m_Picture == eMaterialPicture_Image)
		{
			tVertexVec VtxVec = m_vImage[a_Type]->GetVertexVecCopy(0,0);

			SizeRect.x = VtxVec[0].tex.x;
			SizeRect.y = VtxVec[0].tex.y;
			SizeRect.w = VtxVec[2].tex.x - VtxVec[0].tex.x;
			SizeRect.h = VtxVec[2].tex.y - VtxVec[0].tex.y;
		}
		else
		{
			SizeRect.x = 0;
			SizeRect.y = 0;
			SizeRect.w = 1;
			SizeRect.h = 1;
		}

		return SizeRect;
	}

	//--------------------------------------------------------------

	iTexture *iMaterial::GetTexture(eMaterialTexture a_Type)
	{
		if (m_Picture == eMaterialPicture_Image)
		{
			if (m_vImage[a_Type]==NULL)
			{
				Log("2: %d\n", a_Type);
				return NULL;
			}
			return m_vImage[a_Type]->GetTexture();
		}
		else
			return m_vTextureParams[a_Type].m_pTexture;
	}

	//--------------------------------------------------------------

	cMaterialTextureParam *iMaterial::GetTextureParamsPtr(eMaterialTexture a_Type)
	{
		return &m_vTextureParams[a_Type];
	}

	//--------------------------------------------------------------

	tGpuProgramVec iMaterial::GetAllVertexProgramVec()
	{
		tGpuProgramVec vProgs;

		for (int i =0; i < (int)eMaterialRenderType_LastEnum; i++)
		{
			if (m_vParams[i].m_pEffect != NULL)
			{
				iGpuProgram *pProg = m_vParams[i].m_pEffect->GetProgram(eGpuProgramType_Vertex);
				if (pProg != NULL)
					vProgs.push_back(pProg);
			}
		}

		return vProgs;
	}
}