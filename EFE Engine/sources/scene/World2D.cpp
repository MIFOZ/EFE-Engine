#include "scene/World2D.h"

#include "graphics/Graphics.h"
#include "resources/Resources.h"
#include "system/String.h"
#include "math/Math.h"
#include "math/MathTypes.h"
#include "graphics/Mesh2d.h"
#include "system/LowLevelSystem.h"
#include "scene/Camera.h"
#include "scene/TileMap.h"
#include "scene/Node2D.h"


#include "scene/GridMap2D.h"
#include "scene/ImageEntity.h"
#include "impl/tinyXml/tinyxml.h"



#include "resources/FileSearcher.h"
//#include "graphics/ImageEntityData.h"

#include "graphics/Renderer2D.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-----------------------------------------------------------

	cWorld2D::cWorld2D(tString a_sName, cGraphics *a_pGraphics, cResources *a_pResources, cSound *a_pSound, cCollider2D *a_pCollider)
	{
		m_pGraphics = a_pGraphics;
		m_pResorces = a_pResources;
		m_pSound = a_pSound;
		m_pCollider = a_pCollider;

		m_pRootNode = efeNew(cNode2D, ());
		m_pMapLights = NULL;
		m_pMapImageEnities = NULL;
		m_pMapBodies = NULL;

		m_pScirpt = NULL;

		m_sName = a_sName;

		m_fLightZ = 10;
		m_AmbientColor = cColor(0,0);

		m_lBodyIDCount = 0;
	}

	//--------------------------------------------------------------

	cWorld2D::~cWorld2D()
	{
		if (m_pTileMap) efeDelete(m_pTileMap);
		if (m_pMapImageEnities) efeDelete(m_pMapImageEnities);
		if (m_pMapBodies) efeDelete(m_pMapBodies);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cWorld2D::CreateFromFile(tString a_sFile)
	{
		a_sFile = cString::SetFileExt(a_sFile, "efe");
		tString sPath = m_pResorces->GetFileSearcher()->GetFilePath(a_sFile);

		if (sPath=="")
		{
			FatalError("Couldn't find map '%s'!\n", a_sFile.c_str());
			return false;
		}

		TiXmlDocument *pDoc = efeNew(TiXmlDocument, (sPath.c_str()));
		if (!pDoc->LoadFile())
		{
			FatalError("Couldn't load map '%s'!\n", a_sFile.c_str());
			return false;
		}


	}

	//--------------------------------------------------------------

	void cWorld2D::Render(cCamera2D *a_pCamera)
	{
		m_pTileMap->Render(a_pCamera);
	}

	//--------------------------------------------------------------

	cGridMap2D *cWorld2D::GetGridMapLights()
	{
		return m_pMapLights;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWorld2D::RenderImagesEntites(cCamera2D *a_pCamera)
	{
		cRectf ClipRect;
		a_pCamera->GetClipRect(ClipRect);

		iGridMap2DIt *pEntityIt = m_pMapImageEnities->GetRectIterator(ClipRect);

		while (pEntityIt->HasNext())
		{
			cImageEntity *pEntity = static_cast<cImageEntity*>(pEntityIt->Next());

			if (pEntity->IsActive())
				pEntity->Render();
		}

		efeDelete(pEntityIt);
	}
}
