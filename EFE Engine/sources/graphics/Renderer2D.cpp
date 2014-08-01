#include "graphics/Renderer2D.h"



#include "graphics/RenderObject2D.h"
#include "graphics/GraphicsDrawer.h"
#include "scene/Camera2D.h"
#include "graphics/Mesh2d.h"


#include "resources/Resources.h"
#include "resources/LowLevelResources.h"

#include "resources/TextureManager.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//----------------------------------------------------------

	cRenderer2D::cRenderer2D(iLowLevelGraphics *a_pLowLevelGraphics,cResources *a_pResources,cGraphicsDrawer *a_pGraphicsDrawer)
	{
		Log("  Creating Renderer2D\n");

		m_pLowLevelGraphics = a_pLowLevelGraphics;
		m_pLowLevelResources = a_pResources->GetLowLevel();
		m_pResources = a_pResources;
		m_pGraphicsDrawer = a_pGraphicsDrawer;

		m_pLightMap[0] = m_pResources->GetTextureManager()->Create2D("PointLight2D.bmp", false);

		m_pLightMap[1] = NULL;

		Log("  Renderer2D created\n");
	}

	//--------------------------------------------------------------

	cRenderer2D::~cRenderer2D()
	{
		for (int i=0;i<2;i++)
		{
			if (m_pLightMap[i])
				m_pResources->GetTextureManager()->Destroy(m_pLightMap[i]);
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cRenderObject2DCompare::operator()(const cRenderObject2D &a_ObjectA, const cRenderObject2D &a_ObjectB)
	{
		if (a_ObjectA.GetMaterial()->GetTexture(eMaterialTexture_Diffuse) !=
			a_ObjectB.GetMaterial()->GetTexture(eMaterialTexture_Diffuse))
		{
			return a_ObjectA.GetMaterial()->GetTexture(eMaterialTexture_Diffuse) >
				a_ObjectB.GetMaterial()->GetTexture(eMaterialTexture_Diffuse);
		}
		else if (a_ObjectA.GetMaterial()->GetType(eMaterialRenderType_Perspective) !=
			a_ObjectB.GetMaterial()->GetType(eMaterialRenderType_Perspective))
		{
			return a_ObjectA.GetMaterial()->GetType(eMaterialRenderType_Perspective) >
				a_ObjectB.GetMaterial()->GetType(eMaterialRenderType_Perspective);
		}

		return false;
	}

	//--------------------------------------------------------------

	bool cRenderTransObjectCompare::operator()(const cRenderObject2D &a_ObjectA, const cRenderObject2D &a_ObjectB)
	{
		return false;
	}

	//--------------------------------------------------------------

	void cRenderer2D::RenderObjects(cCamera2D *a_pCamera, cGridMap2D *a_pMapLights, cWorld2D *a_pWorld)
	{
		iMaterial *pMat = NULL;
		iMaterial *pPrevMat = NULL;
		cRectf ClipRect;
		unsigned int lIdxAdd = 0;

		a_pCamera->GetClipRect(ClipRect);

		cRectf TempRect;

		m_pGraphicsDrawer->DrawBackgrounds(TempRect);
	}

	//--------------------------------------------------------------

	void cRenderer2D::AddObject(cRenderObject2D &a_Object)
	{
		if (a_Object.GetMaterial()->IsTransparent())
		{}
		else
			m_mapObject.insert(a_Object);
	}
}