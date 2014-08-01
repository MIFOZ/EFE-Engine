#include "resources/Resources.h"

#include "resources/LowLevelResources.h"
#include "resources/FileSearcher.h"
#include "resources/ImageManager.h"
#include "resources/GpuProgramManager.h"
#include "resources/TileSetManager.h"
#include "resources/SoundManager.h"
#include "resources/FontManager.h"
#include "resources/ScriptManager.h"
#include "resources/TextureManager.h"
#include "resources/MaterialManager.h"
#include "resources/MeshManager.h"
#include "resources/MeshLoaderHandler.h"
#include "resources/SoundEntityManager.h"
#include "resources/VideoManager.h"
#include "system/System.h"

#include "system/LowLevelSystem.h"

#include "impl/tinyXML/tinyxml.h"

namespace efe
{
	cResources::cResources(iLowLevelResources *a_pLowLevelResources, iLowLevelGraphics *a_pLowLevelGraphics)
		: iUpdateable("Resources")
	{
		m_pLowLevelResources = a_pLowLevelResources;
		m_pLowLevelGraphics = a_pLowLevelGraphics;

		m_pFileSearcher = efeNew(cFileSearcher, (m_pLowLevelResources));

		m_pDefaultEntity3DLoader = NULL;
		m_pDefaultArea3DLoader = NULL;
	}

	cResources::~cResources()
	{
		Log("Exiting Resources Module\n");
		Log("-----------------------------------------------------\n");

		STLMapDeleteAll(m_mapEntity3DLoaders);
		STLMapDeleteAll(m_mapArea3DLoaders);

		efeDelete(m_pTileSetManager);
		efeDelete(m_pFontManager);
		efeDelete(m_pScriptManager);
		efeDelete(m_pMeshManager);
		efeDelete(m_pMaterialManager);
		efeDelete(m_pGpuProgramManager);
		efeDelete(m_pImageManager);
		efeDelete(m_pTextureManager);
		efeDelete(m_pVideoManager);

		Log(" All resources deleted\n");

		efeDelete(m_pFileSearcher);
		efeDelete(m_pMeshLoaderHandler);

		Log("-----------------------------------------------------\n\n");
	}

	void cResources::Init(cGraphics *a_pGraphics,cSystem *a_pSystem,cSound *a_pSound, cScene *a_pScene, cGui *a_pGui)
	{
		Log("Initializing Resource Module\n");
		Log("-----------------------------------------------------\n");
		m_pLowLevelSystem = a_pSystem->GetLowLevel();

		Log(" Creating resource managers\n");

		m_pImageManager = efeNew(cImageManager, (m_pFileSearcher,m_pLowLevelGraphics,m_pLowLevelResources,m_pLowLevelSystem));
		m_lstManagers.push_back(m_pImageManager);
		m_pGpuProgramManager = efeNew(cGpuProgramManager, (m_pFileSearcher, m_pLowLevelGraphics, m_pLowLevelResources, m_pLowLevelSystem));
		m_lstManagers.push_back(m_pGpuProgramManager);
		m_pTileSetManager = efeNew(cTileSetManager, (a_pGraphics, this));
		m_lstManagers.push_back(m_pTileSetManager);
		m_pSoundManager = efeNew(cSoundManager, (a_pSound, this));
		m_lstManagers.push_back(m_pSoundManager);
		m_pFontManager = efeNew(cFontManager, (a_pGraphics, a_pGui, this));
		m_lstManagers.push_back(m_pFontManager);
		m_pScriptManager = efeNew(cScriptManager, (a_pSystem, this));
		m_lstManagers.push_back(m_pScriptManager);
		m_pMaterialManager = efeNew(cMaterialManager, (a_pGraphics, this));
		m_lstManagers.push_back(m_pMaterialManager);
		m_pTextureManager = efeNew(cTextureManager, (a_pGraphics, this));
		m_lstManagers.push_back(m_pTextureManager);
		m_pMeshManager = efeNew(cMeshManager, (a_pGraphics, this));
		m_lstManagers.push_back(m_pMeshManager);
		m_pSoundEntityManager = efeNew(cSoundEntityManager, (a_pSound, this));
		m_lstManagers.push_back(m_pSoundEntityManager);
		m_pVideoManager = efeNew(cVideoManager, (a_pGraphics, this));
		m_lstManagers.push_back(m_pVideoManager);

		Log("Misc Creation\n");

		m_pMeshLoaderHandler = efeNew(cMeshLoaderHandler, (this, a_pScene));

		m_pLowLevelResources->AddMeshLoaders(m_pMeshLoaderHandler);

		Log("-----------------------------------------------------\n\n");
	}

	void cResources::Update(float a_fTimeStep)
	{
		tResourcesManagerListIt it = m_lstManagers.begin();
		for(;it!=m_lstManagers.end();it++)
		{
			iResourceManager *pManager = *it;

			pManager->Update(a_fTimeStep);
		}

	}

	cFileSearcher *cResources::GetFileSearcher()
	{
		return m_pFileSearcher;
	}

	//--------------------------------------------------------------

	bool cResources::AddResourceDir(const tString &a_sDir, const tString &a_sMask)
	{
		m_pFileSearcher->AddDirectory(a_sDir, a_sMask);
		if (iResourceBase::GetLogCreateAndDelete())
			Log(" Added resource directory '%s'\n", a_sDir.c_str());
		return true;
	}

	void cResources::ClearResourceDirs()
	{
		m_pFileSearcher->ClearDirectories();
	}

	//--------------------------------------------------------------

	iEntity3DLoader *cResources::GetEntity3DLoader(const tString &a_sName)
	{
		tEntity3DLoaderMapIt it = m_mapEntity3DLoaders.find(a_sName);
		if (it == m_mapEntity3DLoaders.end())
		{
			Warning("No loader for type '%s' found!\n", a_sName.c_str());

			if (m_pDefaultEntity3DLoader)
			{
				Log("Using default loader!\n");
				return m_pDefaultEntity3DLoader;
			}
			else
				return NULL;
		}

		return it->second;
	}

	//--------------------------------------------------------------

	void cResources::AddArea3DLoader(iArea3DLoader *a_pLoader, bool a_bSetAsDefault)
	{
		m_mapArea3DLoaders.insert(tArea3DLoaderMap::value_type(a_pLoader->GetName(), a_pLoader));

		if (a_bSetAsDefault)
			m_pDefaultArea3DLoader = a_pLoader;
	}

	iArea3DLoader *cResources::GetArea3DLoader(const tString &a_sName)
	{
		tArea3DLoaderMapIt it = m_mapArea3DLoaders.find(a_sName);
		if (it != m_mapArea3DLoaders.end())
		{
			Warning("No loader for area type '%s' was found!\n", a_sName.c_str());

			if (m_pDefaultArea3DLoader)
			{
				Log("Using default loader!\n");
				return m_pDefaultArea3DLoader;
			}
			else
				return NULL;
		}

		return it->second;
	}

	//--------------------------------------------------------------

	iLowLevelResources *cResources::GetLowLevel()
	{
		return m_pLowLevelResources;
	}

	//--------------------------------------------------------------

	bool cResources::LoadResourceDirsFile(const tString &a_sFile)
	{
		TiXmlDocument *pXmlDoc = efeNew(TiXmlDocument, (a_sFile.c_str()));
		if (pXmlDoc->LoadFile()==false)
		{
			Error("Couldn't load XML file '%s'!\n", a_sFile.c_str());
			efeDelete(pXmlDoc);
			return false;
		}

		TiXmlElement *pRootElem = pXmlDoc->RootElement();

		TiXmlElement *pChildElem = pRootElem->FirstChildElement();
		for (; pChildElem != NULL; pChildElem = pChildElem->NextSiblingElement())
		{
			tString sPath = cString::ToString(pChildElem->Attribute("Path"), "");
			if (sPath=="")
				continue;

			if (sPath[0]=='/' || sPath[0]=='\\') sPath = sPath.substr(1);

			AddResourceDir(sPath);
		}

		efeDelete(pXmlDoc);
		return true;
	}
}