#include "resources/SoundManager.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "sound/Sound.h"
#include "sound/SoundData.h"
#include "sound/LowLevelSound.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSoundManager::cSoundManager(cSound *a_pSound, cResources *a_pResources)
		: iResourceManager(a_pResources->GetFileSearcher(), a_pResources->GetLowLevel(),
							a_pResources->GetLowLevelSystem())
	{
		m_pSound = a_pSound;
		m_pResources = a_pResources;

		m_pSound->GetLowLevel()->GetSupportedFormats(m_lstFileFormats);
	}

	//--------------------------------------------------------------

	cSoundManager::~cSoundManager()
	{
		DestroyAll();
		Log(" Done with sounds\n");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iResourceBase *cSoundManager::Create(const tString &a_sName)
	{
		return CreateSoundData(a_sName, false);
	}

	//--------------------------------------------------------------

	iSoundData *cSoundManager::CreateSoundData(const tString &a_sName, bool a_bStream, bool a_bLoopStream)
	{
		tString sPath;
		iSoundData *pSound = NULL;

		BeginLoad(a_sName);

		pSound = FindData(a_sName, sPath);

		if (pSound == NULL && sPath != "")
		{
			pSound = m_pSound->GetLowLevel()->LoadSoundData(cString::GetFilePath(a_sName),sPath,"", a_bStream,a_bLoopStream);

			if (pSound)
			{
				AddResource(pSound);
				pSound->SetSoundManager(m_pResources->GetSoundManager());
			}
		}

		EndLoad();
		return pSound;
	}

	//--------------------------------------------------------------

	void cSoundManager::Destroy(iResourceBase *a_pResource)
	{
		if (a_pResource->HasUsers())
		{
			a_pResource->DecUserCount();

			iSoundData *pData = static_cast<iSoundData*>(a_pResource);
			if (pData->IsStream() && pData->HasUsers() == false)
			{
				RemoveResource(pData);
				efeDelete(pData);
			}
		}
	}

	//--------------------------------------------------------------

	void cSoundManager::Unload(iResourceBase *a_pResource)
	{
		
	}

	//--------------------------------------------------------------

	void cSoundManager::DestroyAll()
	{
		tResourceHandleMapIt it = m_mapHandleResources.begin();
		while(it != m_mapHandleResources.end())
		{
			iResourceBase *pData = it->second;
			RemoveResource(pData);
			efeDelete(pData);

			it = m_mapHandleResources.begin();
		}
	}

	iSoundData *cSoundManager::FindData(const tString &a_sName, tString &a_sFilePath)
	{
		iSoundData *pData = NULL;

		if (cString::GetFileExt(a_sName) == "")
		{
			for (tStringListIt it = m_lstFileFormats.begin(); it != m_lstFileFormats.end(); ++it)
			{
				tString sNewName = cString::SetFileExt(a_sName, *it);
				pData = static_cast<iSoundData*> (FindLoadedResource(sNewName, a_sFilePath));

				if ((pData==NULL && a_sFilePath != "") || pData!=NULL) break;
			}
		}
		else
			pData = static_cast<iSoundData*> (FindLoadedResource(a_sName, a_sFilePath));

		return pData;
	}
}