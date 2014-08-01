#include "resources/TileSetManager.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "graphics/Graphics.h"
#include "scene/TileSet.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	cTileSetManager::cTileSetManager(cGraphics *a_pGraphics, cResources *a_pResources)
		: iResourceManager(a_pResources->GetFileSearcher(), a_pResources->GetLowLevel(),
		a_pResources->GetLowLevelSystem())
	{
		m_pGraphics = a_pGraphics;
		m_pResources = a_pResources;
	}

	cTileSetManager::~cTileSetManager()
	{
		DestroyAll();
		Log(" Done with tilsets\n");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iResourceBase *cTileSetManager::Create(const tString &a_sName)
	{
		tString sPath;
		cTileSet *pTileSet;
		tString sNewName;

		BeginLoad(a_sName);

		sNewName = cString::SetFileExt(a_sName, "tsd");

		pTileSet = static_cast<cTileSet*>(this->FindLoadedResource(sNewName, sPath));

		if (pTileSet==NULL && sPath!="")
		{
			pTileSet = efeNew(cTileSet, (sNewName, m_pGraphics, m_pResources));

			if (pTileSet->CreateFromFile(sPath)==false)
			{
				EndLoad();
				return NULL;
			}

			AddResource(pTileSet);
		}

		if (pTileSet) pTileSet->IncUserCount();
		else Error("Couldn't load tileset '%s'\n", sNewName.c_str());

		EndLoad();
		return pTileSet;
	}

	//--------------------------------------------------------------

	cTileSet *cTileSetManager::CreateTileSet(const tString &a_sName)
	{
		return static_cast<cTileSet*>(Create(a_sName));
	}

	//--------------------------------------------------------------

	void cTileSetManager::Destroy(iResourceBase *a_pResource)
	{
		a_pResource->DecUserCount();

		if (a_pResource->HasUsers()==false)
		{
			RemoveResource(a_pResource);
			efeDelete(a_pResource);
		}
	}

	//--------------------------------------------------------------

	void cTileSetManager::Unload(iResourceBase *a_pResource)
	{
		
	}
}