#include "graphics/Material_Diffuse2D.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMaterial_Diffuse2D::cMaterial_Diffuse2D(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics,
		cImageManager *a_pImageManager, cTextureManager *a_pTextureManger,
		cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
		eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D)
		: iMaterial(a_sName,a_pLowLevelGraphics,a_pImageManager,a_pTextureManger,a_pRenderer,a_pProgramManager,
		a_Picture,a_pRenderer3D)
	{
		m_bIsTransparent = false;
		m_bIsGlowing = false;

		m_Type = eMaterialType_Diffuse;
	}

	//--------------------------------------------------------------

	cMaterial_Diffuse2D::~cMaterial_Diffuse2D()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cMaterial_Diffuse2D::Compile()
	{
		
	}

	//--------------------------------------------------------------

	bool cMaterial_Diffuse2D::StartRendering(eMaterialRenderType a_Type, iCamera *a_pCam, iLight *a_pLight)
	{
		if (a_Type == eMaterialRenderType_Perspective)
		{
			//m_pLowLevelGraphics->SetTexture("Base", GetTexture(eMaterialTexture_Diffuse));
			return true;
		}
		return true;
	}

	//--------------------------------------------------------------

	eMaterialType cMaterial_Diffuse2D::GetType(eMaterialRenderType a_Type)
	{
		return m_Type;
	}
}