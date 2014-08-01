#include "resources/SoundEntityManager.h"

#include "system/String.h"
#include "sound/Sound.h"
#include "resources/Resources.h"
#include "sound/SoundEntityData.h"
#include "system/LowLevelSystem.h"
#include "sound/SoundHandler.h"
#include "sound/SoundChannel.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSoundEntityManager::cSoundEntityManager(cSound *a_pSound, cResources *a_pResources)
		: iResourceManager(a_pResources->GetFileSearcher(),a_pResources->GetLowLevel(),
		a_pResources->GetLowLevelSystem())
	{
		m_pSound = a_pSound;
		m_pResources = a_pResources;
	}

	//--------------------------------------------------------------

	cSoundEntityManager::~cSoundEntityManager()
	{
		DestroyAll();

		Log(" Done with sound entities\n");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cSoundEntityManager::Preload(const tString &a_sFile)
	{
		cSoundEntityData *pData = CreateSoundEntity(a_sFile);
		if (pData == NULL)
		{
			Warning("Couldn't preload sound '%s'\n", a_sFile);
			return;
		}

		if (pData->GetMainSoundName() != "")
		{
			iSoundChannel *pChannel = m_pSound->GetSoundHandler()->CreateChannel(pData->GetMainSoundName(),0);
			if (pChannel) efeDelete(pChannel);
		}
		if (pData->GetStartSoundName() != "")
		{
			iSoundChannel *pChannel = m_pSound->GetSoundHandler()->CreateChannel(pData->GetStartSoundName(),0);
			if (pChannel) efeDelete(pChannel);
		}
		if (pData->GetStopSoundName() != "")
		{
			iSoundChannel *pChannel = m_pSound->GetSoundHandler()->CreateChannel(pData->GetStopSoundName(),0);
			if (pChannel) efeDelete(pChannel);
		}
	}

	//--------------------------------------------------------------

	iResourceBase *cSoundEntityManager::Create(const tString &a_sName)
	{
		return CreateSoundEntity(a_sName);
	}

	//--------------------------------------------------------------

	cSoundEntityData *cSoundEntityManager::CreateSoundEntity(const tString &a_sName)
	{
		tString sPath;
		cSoundEntityData *pSoundEntity;
		tString sNewName;

		BeginLoad(a_sName);

		sNewName = cString::SetFileExt(a_sName, "snt");

		pSoundEntity = static_cast<cSoundEntityData*>(this->FindLoadedResource(sNewName, sPath));

		if (pSoundEntity == NULL && sPath != "")
		{
			pSoundEntity = efeNew(cSoundEntityData, (sNewName));

			if (pSoundEntity->CreateFromFile(sPath))
				AddResource(pSoundEntity);
			else
			{
				efeDelete(pSoundEntity);
				pSoundEntity = NULL;
			}
		}

		if (pSoundEntity) pSoundEntity->IncUserCount();
		else Error("Couldn't create SoundEntity '%s'\n", sNewName.c_str());

		EndLoad();
		return pSoundEntity;
	}

	//--------------------------------------------------------------

	void cSoundEntityManager::Destroy(iResourceBase *a_pResource)
	{
		a_pResource->DecUserCount();

		if (a_pResource->HasUsers() == false)
		{
			RemoveResource(a_pResource);
			efeDelete(a_pResource);
		}
	}

	//--------------------------------------------------------------

	void cSoundEntityManager::Unload(iResourceBase *a_pResource)
	{
		
	}
}