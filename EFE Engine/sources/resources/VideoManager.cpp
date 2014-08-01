#include "resources/VideoManager.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "resources/Resources.h"
#include "resources/FileSearcher.h"
#include "graphics/VideoStream.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cVideoManager::cVideoManager(cGraphics *a_pGraphics, cResources *a_pResources)
		: iResourceManager(a_pResources->GetFileSearcher(), a_pResources->GetLowLevel(),
							a_pResources->GetLowLevelSystem())
	{
		m_pGraphics = a_pGraphics;
		m_pResources = a_pResources;
	}

	cVideoManager::~cVideoManager()
	{
		STLDeleteAll(m_lstVideoLoaders);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iResourceBase *cVideoManager::Create(const tString &a_sName)
	{
		return CreateVideo(a_sName);
	}

	//--------------------------------------------------------------

	iVideoStream *cVideoManager::CreateVideo(const tString &a_sName)
	{
		BeginLoad(a_sName);

		tString sPath = m_pFileSearcher->GetFilePath(a_sName);
		if (sPath == "")
		{
			EndLoad();
			Error("Video file '%s' could not found!\n", a_sName.c_str());
			return NULL;
		}

		iVideoStreamLoader *pLoader = GetLoader(a_sName);
		if (pLoader==NULL)
		{
			Error("Could not find a loader for '%s'\n", a_sName.c_str());
			return NULL;
		}

		iVideoStream *pVideo = pLoader->Create(a_sName);

		if (pVideo->LoadFromFile(sPath)==false)
		{
			EndLoad();
			efeDelete(pVideo);
			Error("Could not load video '%s'\n", a_sName.c_str());
			return NULL;
		}

		AddResource(pVideo);

		EndLoad();
		return pVideo;
	}

	//--------------------------------------------------------------

	void cVideoManager::AddVideoLoader(iVideoStreamLoader *a_pLoader)
	{
		m_lstVideoLoaders.push_back(a_pLoader);
	}

	//--------------------------------------------------------------

	void cVideoManager::Destroy(iResourceBase *a_pResource)
	{
		if (a_pResource)
		{
			RemoveResource(a_pResource);
			efeDelete(a_pResource);
		}
	}

	//--------------------------------------------------------------

	void cVideoManager::Unload(iResourceBase *a_pResource)
	{
		
	}

	//--------------------------------------------------------------

	void cVideoManager::Update(float a_fTimeStep)
	{
		tResourceHandleMapIt it = m_mapHandleResources.begin();
		for (; it != m_mapHandleResources.end(); ++it)
		{
			iResourceBase *pBase = it->second;
			iVideoStream *pVideo = static_cast<iVideoStream*>(pBase);

			pVideo->Update(a_fTimeStep);
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iVideoStreamLoader *cVideoManager::GetLoader(const tString &a_sFileName)
	{
		tString sExt = cString::ToLowerCase(cString::GetFileExt(a_sFileName));

		tVideoStreamLoaderListIt it = m_lstVideoLoaders.begin();
		for (; it != m_lstVideoLoaders.end(); ++it)
		{
			iVideoStreamLoader *pLoader = *it;

			tStringVec &vExt = pLoader->GetExtensions();
			for (size_t i=0; i<vExt.size(); ++i)
			{
				if (vExt[i] == sExt)
					return pLoader;
			}
		}

		return NULL;
	}
}