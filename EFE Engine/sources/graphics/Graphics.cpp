#include "graphics/Graphics.h"

#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/LowLevelResources.h"
#include "graphics/GraphicsDrawer.h"
#include "graphics/Renderer2D.h"
#include "graphics/Renderer3D.h"
#include "graphics/RenderList.h"
#include "graphics/MaterialHandler.h"
#include "graphics/MeshCreator.h"
#include "game/Updateable.h"
#include "resources/Resources.h"

//2D Materials
#include "graphics/Material_Diffuse2D.h"
#include "graphics/Material_FontNormal.h"

//3D Materials
#include "graphics/Material_Diffuse.h"

#include "graphics/Material_Bump.h"

namespace efe
{
	cGraphics::cGraphics(iLowLevelGraphics *a_pLowLevelGraphics, iLowLevelResources *a_pLowLevelResources)
	{
		m_pLowLevelGraphics = a_pLowLevelGraphics;
		m_pLowLevelResources = a_pLowLevelResources;

		m_pDrawer = NULL;
		m_pMeshCreator = NULL;
		m_pMaterialHandler = NULL;
		m_pRenderer2D = NULL;
		m_pRenderer3D = NULL;
		//m_RendererPostEffects = NULL;
	}

	cGraphics::~cGraphics()
	{
		Log("Exiting Graphics Module\n");
		Log("-----------------------------------------------------\n");

		efeDelete(m_pRenderer2D);
		efeDelete(m_pRenderer3D);
		//efeDelete(m_RendererPostEffects);
		efeDelete(m_pDrawer);
		efeDelete(m_pMeshCreator);
		efeDelete(m_pMaterialHandler);
		efeDelete(m_pRenderList);

		Log("-----------------------------------------------------\n");

	}

	bool cGraphics::Init(int a_iWidth, int a_iHeight, int a_iBpp, bool a_bFullscreen, int a_lMultiSampling,
			const tString &a_sWindowCaption, cResources *a_pResources)
	{
		Log("Initializing Graphics Module\n");
		Log("-----------------------------------------------------\n");

		a_pResources->AddResourceDir("core/programs");
		a_pResources->AddResourceDir("core/textures");

		Log(" Initializing Low Level Graphics\n");
		m_pLowLevelGraphics->Init(a_iWidth, a_iHeight, a_iBpp, a_bFullscreen, a_lMultiSampling, a_sWindowCaption);

		Log(" Creating graphics systems\n");
		m_pMaterialHandler = efeNew(cMaterialHandler, (this, a_pResources));
		m_pDrawer = efeNew(cGraphicsDrawer, (m_pLowLevelGraphics, m_pMaterialHandler, a_pResources));
		m_pRenderer2D = efeNew(cRenderer2D, (m_pLowLevelGraphics, a_pResources, m_pDrawer));
		m_pRenderList = efeNew(cRenderList,(this));
		m_pMeshCreator = efeNew(cMeshCreator, (m_pLowLevelGraphics, a_pResources));
		m_pRenderer3D = efeNew(cRenderer3D,(m_pLowLevelGraphics, a_pResources, m_pMeshCreator, m_pRenderList));

		Log(" Adding engine materials\n");
		//2D
		m_pMaterialHandler->Add(efeNew(cMaterialType_Diffuse2D, ()));
		m_pMaterialHandler->Add(efeNew(cMaterialType_FontNormal, ()));

		//3D
		m_pMaterialHandler->Add(efeNew(cMaterialType_Diffuse, ()));
		//m_pMaterialHandler->Add(efeNew(cMaterialType_Bump, ()));

		Log("-----------------------------------------------------\n\n");

		return true;
	}

	iLowLevelGraphics *cGraphics::GetLowLevel()
	{
		return m_pLowLevelGraphics;
	}

	cGraphicsDrawer *cGraphics::GetDrawer()
	{
		return m_pDrawer;
	}

	cRenderer2D *cGraphics::GetRenderer2D()
	{
		return m_pRenderer2D;
	}
}