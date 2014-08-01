#include "graphics/Material_FontNormal.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMaterial_FontNormal::cMaterial_FontNormal(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics,
		cImageManager *a_pImageManager, cTextureManager *a_pTextureManger,
		cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
		eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D)
		: iMaterial(a_sName,a_pLowLevelGraphics,a_pImageManager,a_pTextureManger,a_pRenderer,a_pProgramManager,
		a_Picture,a_pRenderer3D)
	{
		m_pBS = m_pLowLevelGraphics->CreateBlendState(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
	}


	cMaterial_FontNormal::~cMaterial_FontNormal()
	{
		efeDelete(m_pBS);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cMaterial_FontNormal::Compile()
	{
	
	}

	//--------------------------------------------------------------

	bool cMaterial_FontNormal::StartRendering(eMaterialRenderType a_Type, iCamera *a_pCam, iLight *a_pLight)
	{
		if (a_Type == eMaterialRenderType_Perspective)
		{
			m_pLowLevelGraphics->SetBlendState(m_pBS);

			return true;
		}

		return false;
	}

	//--------------------------------------------------------------

	eMaterialType cMaterial_FontNormal::GetType(eMaterialRenderType a_Type)
	{
		return m_Type;
	}
}