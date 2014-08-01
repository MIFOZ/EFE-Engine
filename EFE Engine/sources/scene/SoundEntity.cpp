#include "scene/SoundEntity.h"

#include "sound/SoundHandler.h"
#include "sound/SoundChannel.h"
#include "sound/SoundEntityData.h"
#include "sound/LowLevelSound.h"
#include "sound/Sound.h"
#include "resources/SoundEntityManager.h"
#include "math/Math.h"

#include "game/Game.h"
#include "scene/Scene.h"
#include "scene/World3D.h"

namespace efe
{
	tSoundEntityGlobalCallbackList cSoundEntity::m_lstGlobalCallbacks;

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSoundEntity::cSoundEntity(const tString &a_sName, cSoundEntityData *a_pData,
			cSoundEntityManager *a_pSoundEntityManager,
			cWorld3D *a_pWorld,
			cSoundHandler *a_pSoundHandler, bool a_bRemoveWhenOver)
			: iEntity3D(a_sName)
	{
		m_bRemoveWhenOver = a_bRemoveWhenOver;

		m_pSoundHandler = a_pSoundHandler;
		m_pSoundEntityManager = a_pSoundEntityManager;
		m_pWorld = a_pWorld;

		m_pData = a_pData;

		m_BoundingVolume.SetSize(m_pData->GetMaxDistance()*2);

		for (int i=0;i<3;i++)
		{
			m_vSounds[i] = NULL;
			m_vSoundId[i] = -1;
		}

		m_fIntervalCount = m_pData->GetInterval();

		m_fVolume = m_pData->GetVolume();

		m_bStopped = false;
		m_bStarted = false;

		m_bSkipStartEnd = false;

		m_bPrioRemove = false;

		m_bFadingOut = false;

		m_bOutOfRange = false;

		m_bLog = false;

		m_fSleepCount = 0;

		m_pSoundCallback = efeNew(cSoundEnityChannelCallback, ());
		m_pSoundCallback->m_pEntity = this;

		if (m_pSoundHandler->GetSilent())
		{
			m_bStarted = true;
			m_bStopped = true;
			m_fVolume = 0;
			m_bSkipStartEnd = true;
			m_bRemoveWhenOver = true;
		}
	}

	//--------------------------------------------------------------

	cSoundEntity::~cSoundEntity()
	{
		if (m_bLog) Log("Delete entity start...");
		for (int i=0; i<3; i++)
		{
			if (m_pSoundHandler->IsValid(m_vSounds[i]))
				m_vSounds[i]->Stop();
		}

		efeDelete(m_pSoundCallback);

		if (m_bLog) Log("end\n");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cSoundEnityChannelCallback::OnPriorityRelease()
	{
		if (m_pEntity->m_bStopped || m_pEntity->m_bFadingOut) return;

		m_pEntity->m_bPrioRemove = true;
	}

	//--------------------------------------------------------------

	void cSoundEntity::Play(bool a_bPlayStart)
	{
		if (m_pSoundHandler->GetSilent())
			return;

		if (m_bLog) Log("Play entity start...");
		m_bStopped = false;
		m_bOutOfRange = false;
		m_bFadingOut = false;

		if (a_bPlayStart && m_bSkipStartEnd == false)
		{
			if (m_pData->GetLoop() && m_pData->GetStartSoundName() != "")
			{
				PlaySound(m_pData->GetStartSoundName(), false, eSoundEntityType_Start);
				m_bStarted = false;
			}
		}

		if (m_vSounds[eSoundEntityType_Main]==NULL && m_vSounds[eSoundEntityType_Start]==NULL
			&& (m_pData->GetLoop()==false || m_pData->GetInterval()==0))
		{
			PlaySound(m_pData->GetMainSoundName(), m_pData->GetLoop(), eSoundEntityType_Main);
			m_bStarted = true;
		}

		if (m_bLog) Log("end\n");
	}

	//--------------------------------------------------------------

	void cSoundEntity::Stop(bool a_bPlayEnd)
	{
		if (m_bStopped)
			return;

		m_bStopped = true;
		m_bOutOfRange = false;

		if (m_vSounds[eSoundEntityType_Main]==NULL) return;

		if (m_bLog) Log("Stop entity start...");

		if (m_pData->GetLoop())
		{
			if (a_bPlayEnd && m_bSkipStartEnd == false)
			{
				PlaySound(m_pData->GetStopSoundName(), false, eSoundEntityType_Stop);
			}

			if (m_pSoundHandler->IsValid(m_vSounds[eSoundEntityType_Main]))
				m_vSounds[eSoundEntityType_Main]->Stop();
		}
		else
		{
			if (m_pSoundHandler->IsValid(m_vSounds[eSoundEntityType_Main]))
				m_vSounds[eSoundEntityType_Main]->Stop();
		}

		m_vSounds[eSoundEntityType_Main] = NULL;

		if (m_bLog) Log("end\n");
	}

	//--------------------------------------------------------------

	void cSoundEntity::FadeIn(float a_fSpeed)
	{
		if (m_pSoundHandler->GetSilent())
			return;

		if (m_bLog) Log("Fade in entity start...");

		Play(false);
		cSoundEntry *pEntry = GetSoundEntry(eSoundEntityType_Main);
		if (pEntry)
		{
			pEntry->m_fNormalVolumeMul = 0;
			pEntry->m_fNormalVolumeFadeDest = 1;
			pEntry->m_fNormalVolumeFadeSpeed = a_fSpeed;
			if (pEntry->m_fNormalVolumeFadeSpeed < 0)
				pEntry->m_fNormalVolumeFadeSpeed = -pEntry->m_fNormalVolumeFadeSpeed;
		}

		if (m_bLog) Log("end\n");
	}

	//--------------------------------------------------------------

	void cSoundEntity::FadeOut(float a_fSpeed)
	{
		m_bFadingOut = true;
		if (m_pSoundHandler->GetSilent())
			return;

		if (m_bLog) Log("Fade out entity start...");

		if (m_vSounds[eSoundEntityType_Main]==NULL)
		{
			m_bStopped = true;
			return;
		}

		cSoundEntry *pEntry = GetSoundEntry(eSoundEntityType_Main);
		if (pEntry)
		{
			pEntry->m_fNormalVolumeFadeDest = 0;
			pEntry->m_fNormalVolumeFadeSpeed = a_fSpeed;
			if (pEntry->m_fNormalVolumeFadeSpeed > 0)
				pEntry->m_fNormalVolumeFadeSpeed = -pEntry->m_fNormalVolumeFadeSpeed;
		}
		else
			m_bStopped = true;

		if (m_bLog) Log("end\n");
	}

	//--------------------------------------------------------------

	bool cSoundEntity::IsStopped()
	{
		if (m_bStopped && m_vSounds[eSoundEntityType_Stop] == NULL)
			return true;

		return false;
	}

	//--------------------------------------------------------------

	bool cSoundEntity::IsFadingOut()
	{
		return m_bFadingOut;
	}

	//--------------------------------------------------------------

	bool cSoundEntity::GetRemoveWhenOver()
	{
		return m_bRemoveWhenOver;
	}

	//--------------------------------------------------------------

	cSoundEntry *cSoundEntity::GetSoundEntry(eSoundEntityType a_Type)
	{
		if (m_vSounds[a_Type] == NULL) return NULL;

		return m_pSoundHandler->GetEntryFromSound(m_vSounds[a_Type]);
	}

	//--------------------------------------------------------------

	void cSoundEntity::UpdateLogic(float a_fTimeStep)
	{
		if (m_pSoundHandler->GetSilent())
			return;

		if (m_fSleepCount > 0)
		{
			m_fSleepCount -= a_fTimeStep;
			return;
		}

		if (m_bOutOfRange && m_bStopped == false)
		{
			float fRange = GetListenerSqrLength();
			float fMaxRange = m_pData->GetMaxDistance();
			fMaxRange = fMaxRange * fMaxRange;

			if (fRange < fMaxRange)
			{
				Play(false);
				m_bOutOfRange = false;
			}
			else
				return;
		}

		if (m_bLog) Log("Update entity start...");

		for (int i=0; i<3; i++)
		{
			if (m_vSounds[i])
			{
				if (m_pSoundHandler->IsValidId(m_vSounds[i], m_vSoundId[i]) == false)
				{
					m_vSounds[i] = NULL;

					if (m_bPrioRemove && i == (int)eSoundEntityType_Main)
					{
						if (m_bStopped == false)
						{
							m_bStarted = false;
							m_fSleepCount = 0.3f;
						}
					}
					else if ((m_pData->GetInterval()==0 || m_pData->GetLoop()==false) &&
						m_bStarted)
					{
						m_bStopped = true;
					}
				}
				else
				{
					if (m_vSounds[i] && !m_vSounds[i]->GetPositionRelative())
						m_vSounds[i]->SetPosition(GetWorldPosition());
				}
			}
		}

		if (m_bStopped == false)
		{
			if (m_vSounds[eSoundEntityType_Main]==NULL && m_vSounds[eSoundEntityType_Start]==NULL
				&& (m_pData->GetLoop() == false || m_pData->GetInterval() == 0))
			{
				if (PlaySound(m_pData->GetMainSoundName(), m_pData->GetLoop(), eSoundEntityType_Main))
				{
					m_bStarted = true;
					m_bPrioRemove = false;

					tSoundEntityGlobalCallbackListIt it = m_lstGlobalCallbacks.begin();
					for (; it != m_lstGlobalCallbacks.end(); ++it)
					{
						iSoundEntityGlobalCallback *pCallback = *it;
						pCallback->OnStart(this);
					}
				}
				else if (m_bPrioRemove)
					m_fSleepCount = 0.3f;
			}

			if (m_pData->GetLoop() && m_pData->GetInterval() > 0 && m_vSounds[eSoundEntityType_Start]==NULL
				&& m_vSounds[eSoundEntityType_Main]==NULL)
			{
				m_fIntervalCount += a_fTimeStep;

				if (m_fIntervalCount >= m_pData->GetInterval())
				{
					if (cMath::RandRectf(0,1) <= m_pData->GetRandom() ||
						m_pData->GetRandom() == 0)
						PlaySound(m_pData->GetMainSoundName(), false, eSoundEntityType_Main);
					m_fIntervalCount = 0;
				}
			}

			if (m_vSounds[eSoundEntityType_Start]==NULL && m_pData->GetLoop() && m_pData->GetUse3D())
			{
				float fRange = GetListenerSqrLength();
				float fMaxRange = m_pData->GetMaxDistance() + 1.0f;
				fMaxRange = fMaxRange * fMaxRange;

				if (fRange > fMaxRange)
				{
					if (m_vSounds[eSoundEntityType_Main])
					{
						if (m_pSoundHandler->IsValid(m_vSounds[eSoundEntityType_Main]))
							m_vSounds[eSoundEntityType_Main]->Stop();

						m_vSounds[eSoundEntityType_Main] = NULL;
					}

					m_bOutOfRange = true;
				}
			}
		}

		if (m_bLog) Log("end\n");
	}

	//--------------------------------------------------------------

	void cSoundEntity::AddGlobalCallback(iSoundEntityGlobalCallback *a_pCallback)
	{
		m_lstGlobalCallbacks.push_back(a_pCallback);
	}

	//--------------------------------------------------------------

	void cSoundEntity::RemoveGlobalCallback(iSoundEntityGlobalCallback *a_pCallback)
	{
		tSoundEntityGlobalCallbackListIt it = m_lstGlobalCallbacks.begin();
		for (; it != m_lstGlobalCallbacks.end(); ++it)
		{
			if (a_pCallback == *it)
			{
				m_lstGlobalCallbacks.erase(it);
				return;
			}
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cSoundEntity::PlaySound(const tString &a_sName, bool a_bLoop, eSoundEntityType a_Type)
	{
		if (a_sName == "") return false;

		if (m_pData->GetUse3D())
		{
			m_vSounds[a_Type] = m_pSoundHandler->Play3D(a_sName, a_bLoop,
				m_fVolume, GetWorldPosition(), m_pData->GetMinDistance(),
				m_pData->GetMaxDistance(), eSoundDest_World, false, m_pData->GetPriority());
		}
		else
		{
			m_vSounds[a_Type] = m_pSoundHandler->Play3D(a_sName, a_bLoop,
				m_fVolume, cVector3f(0,0,1), m_pData->GetMinDistance(),
				m_pData->GetMaxDistance(), eSoundDest_World, true, m_pData->GetPriority());
		}

		if (m_vSounds[a_Type] == NULL)
		{
			if (!m_bPrioRemove)
				Error("Couldn't play sound '%s' for sound entity %s\n", a_sName.c_str(), m_sName.c_str());

			if (a_Type == eSoundEntityType_Main && !m_bPrioRemove)
				m_bStopped = true;

			return false;
		}
		else
		{
			m_vSounds[a_Type]->SetBlockable(m_pData->GetBlockable());
			m_vSounds[a_Type]->SetBlockVolumeMul(m_pData->GetBlockVolumeMul());
			m_vSounds[a_Type]->SetPriorityModifier(m_pData->GetPriority());

			m_vSoundId[a_Type] = m_vSounds[a_Type]->GetId();

			if (a_Type == eSoundEntityType_Main)
				m_vSounds[a_Type]->SetCallback(m_pSoundCallback);
		}

		return true;
	}

	//--------------------------------------------------------------

	float cSoundEntity::GetListenerSqrLength()
	{
		cVector3f vListenerPos = m_pWorld->GetSound()->GetLowLevel()->GetListenerPosition();
		return cMath::Vector3DistSqr(vListenerPos, GetWorldPosition());
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	kBeginSerialize(cSaveData_cSoundEntity,cSaveData_iEntity3D)
		kSerializeVar(m_sData, eSerializeType_String)
		kSerializeVar(m_bStopped, eSerializeType_Bool)
		kSerializeVar(m_bRemoveWhenOver, eSerializeType_Bool)
		kSerializeVar(m_bStarted, eSerializeType_Bool)
		kSerializeVar(m_bFadingOut, eSerializeType_Bool)
		kSerializeVar(m_fVolume, eSerializeType_Float32)
		kEndSerialize()

	//--------------------------------------------------------------

	iSaveObject *cSaveData_cSoundEntity::CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		cWorld3D *pWorld = a_pGame->GetScene()->GetWorld3D();

		if (m_bStopped && m_bRemoveWhenOver) return NULL;

		cSoundEntity *pEntity = pWorld->CreateSoundEntity(m_sName, m_sData, m_bRemoveWhenOver);

		if (m_bFadingOut) pEntity->Stop(false);

		return pEntity;
	}

	//--------------------------------------------------------------

	int cSaveData_cSoundEntity::GetSaveCreatePrio()
	{
		return 3;
	}

	//--------------------------------------------------------------

	iSaveData *cSoundEntity::CreateSaveData()
	{
		return efeNew(cSaveData_cSoundEntity, ());
	}

	//--------------------------------------------------------------

	void cSoundEntity::SaveToSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_SaveToBegin(cSoundEntity);

		pData->m_sData = m_pData == NULL ? "" : m_pData->GetName();

		kSaveData_SaveTo(m_bStopped);
		kSaveData_SaveTo(m_bRemoveWhenOver);
		kSaveData_SaveTo(m_bStarted);
		kSaveData_SaveTo(m_bFadingOut);
		kSaveData_SaveTo(m_fVolume);
	}

	//--------------------------------------------------------------

	void cSoundEntity::LoadFromSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_LoadFromBegin(cSoundEntity);

		kSaveData_LoadFrom(m_fVolume);
		kSaveData_LoadFrom(m_bStopped);
		kSaveData_LoadFrom(m_bRemoveWhenOver);
	}

	//--------------------------------------------------------------

	void cSoundEntity::SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		kSaveData_SetupBegin(cSoundEntity);
	}
}