#include "sound/SoundHandler.h"
#include "resources/Resources.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "math/Math.h"
#include "sound/LowLevelSound.h"
#include "sound/SoundChannel.h"
#include "resources/SoundManager.h"
#include "sound/SoundData.h"

#include "scene/World3D.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSoundHandler::cSoundHandler(iLowLevelSound *a_pLowLevelSound, cResources *a_pResources)
	{
		m_pLowLevelSound = a_pLowLevelSound;
		m_pResources = a_pResources;

		m_fSpeed = 1;
		m_fNewSpeed = 1;
		m_fSpeedRate = 0;
		m_fVolume = 1;
		m_fNewVolume = 1;
		m_fVolumeRate = 0;

		m_pWorld3D = NULL;

		m_lCount = 0;

		m_bSilent = false;

		m_AffectedBySpeed = eSoundDest_World;
		m_AffectedByVolume = eSoundDest_World;
	}

	//--------------------------------------------------------------

	cSoundHandler::~cSoundHandler()
	{
		tSoundEntryListIt it;
		it = m_lstGuiSounds.begin();
		while (it != m_lstGuiSounds.end())
		{
			it->m_pSound->Stop();
			efeDelete(it->m_pSound);
			it = m_lstGuiSounds.erase(it);
		}

		it = m_lstWorldSounds.begin();
		while (it != m_lstWorldSounds.end())
		{
			it->m_pSound->Stop();
			efeDelete(it->m_pSound);
			it = m_lstWorldSounds.erase(it);
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cSoundRayCallback::Reset()
	{
		m_bHasCollided = false;
	}

	bool cSoundRayCallback::BeforeIntersect(iPhysicsBody *a_pBody)
	{
		if (a_pBody->GetBlocksSound()) return true;

		return false;
	}

	bool cSoundRayCallback::OnIntersect(iPhysicsBody *a_pBody, cPhysicsRayParams *a_pParams)
	{
		m_bHasCollided = true;
		return false;
	}

	//--------------------------------------------------------------

	void cSoundEntry::Update(float a_fTimeStep)
	{
		if (m_fNormalVolumeMul != m_fNormalVolumeFadeDest)
		{
			m_fNormalVolumeMul += m_fNormalVolumeFadeSpeed*a_fTimeStep;

			if (m_fNormalVolumeMul < 0) m_fNormalVolumeMul = 0;
			if (m_fNormalVolumeMul > 1) m_fNormalVolumeMul = 1;

			if (m_fNormalVolumeFadeSpeed < 0)
			{
				if (m_fNormalVolumeMul <= m_fNormalVolumeFadeDest)
					m_fNormalVolumeMul= m_fNormalVolumeFadeDest;
			}
			else
			{
				if (m_fNormalVolumeMul >= m_fNormalVolumeFadeDest)
					m_fNormalVolumeMul = m_fNormalVolumeFadeDest;
			}
		}

		if (std::abs(m_fNormalVolumeFadeDest) < 0.001f && std::abs(m_fNormalVolumeMul) < 0.001f
			&& m_fNormalVolumeFadeSpeed <= 0)
			m_pSound->Stop();
	}

	//--------------------------------------------------------------

	iSoundChannel *cSoundHandler::Play(const tString &a_sName, bool a_bLoop, float a_fVolume, const cVector3f &a_vPos,
			float a_fMinDist, float a_fMaxDist, eSoundDest a_Type, bool a_bRelative, bool a_b3D,
			int a_lPriorityModifier, eSoundDest a_EffectType)
	{
		if (a_sName == "")
			return NULL;

		//Calculate priority
		int lPrio = 255;
		if (a_Type == eSoundDest_World && !a_bRelative)
		{
			lPrio = a_lPriorityModifier;

			float fDist = cMath::Vector3DDist(a_vPos, m_pLowLevelSound->GetListenerPosition());
			if (fDist >= a_fMaxDist)
				lPrio += 0;
			else if (fDist >= a_fMinDist)
				lPrio += 10;
			else
				lPrio += 100;
		}

		//Create sound channel
		iSoundChannel *pSound = CreateChannel(a_sName, lPrio);
		if (pSound == NULL)
		{
			Warning("Can't find sound '%s' (may also be due too many sounds playing)\n", a_sName.c_str());
			return NULL;
		}

		//Set up channel
		pSound->SetLooping(a_bLoop);
		pSound->SetMinDistance(a_fMinDist);
		pSound->SetMaxDistance(a_fMaxDist);
		pSound->Set3D(a_b3D);
		pSound->SetPriority(lPrio);

		if (a_EffectType == eSoundDest_World)
			pSound->SetAffectedByEnv(true);

		if (a_EffectType == eSoundDest_Gui)
		{
			pSound->SetPositionRelative(true);
			pSound->SetRelPosition(a_vPos);

			cVector3f vPos = cMath::MatrixMul(m_pLowLevelSound->GetListenerMatrix(), a_vPos);
			pSound->SetPosition(a_vPos);
		}
		else
		{
			pSound->SetPositionRelative(a_bRelative);
			if (a_bRelative)
			{
				pSound->SetRelPosition(a_vPos);
				cVector3f vPos = cMath::MatrixMul(m_pLowLevelSound->GetListenerMatrix(), a_vPos);
				pSound->SetPosition(a_vPos);
			}
			else
				pSound->SetPosition(a_vPos);
		}

		pSound->SetId(m_lIdCount);

		cSoundEntry Entry;
		Entry.m_pSound = pSound;
		Entry.m_fNormalVolume = a_fVolume;
		Entry.m_sName = a_sName;
		Entry.m_fNormalSpeed = 1.0f;

		Entry.m_fBlockFadeDest = 1;
		Entry.m_fBlockFadeSpeed = 1;
		Entry.m_fBlockMul = 1;
		Entry.m_bFirstTime = true;

		Entry.m_EffectType = a_EffectType;

		if (a_Type == eSoundDest_Gui)
			pSound->SetVolume(a_fVolume);
		else
			pSound->SetVolume(0);

		if (m_bSilent)
		{
			pSound->SetLooping(false);
			pSound->Stop();
		}
		else
			pSound->Play();

		if (a_Type == eSoundDest_Gui)
			m_lstGuiSounds.push_back(Entry);
		else
			m_lstWorldSounds.push_back(Entry);

		m_lIdCount++;

		return pSound;
	}

	//--------------------------------------------------------------

	iSoundChannel *cSoundHandler::PlayStream(const tString &a_sFileName, bool a_bLoop, float a_fVolume, bool a_b3D,
			eSoundDest a_EffectType)
	{
		if (a_sFileName == "")
			return NULL;

		iSoundData *pData = m_pResources->GetSoundManager()->CreateSoundData(a_sFileName, true,  a_bLoop);
		if (pData == NULL)
		{
			Error("Couldn't load stream '%s'\n", a_sFileName.c_str());
			return NULL;
		}

		iSoundChannel *pSound = pData->CreateChannel(256);
		if (pSound == NULL)
		{
			Error("Can't create sound channel for '%s'\n", a_sFileName.c_str());
			return NULL;
		}

		if (m_bSilent)
			pSound->Stop();
		else
			pSound->Play();

		pSound->SetId(m_lIdCount);

		pSound->Set3D(a_b3D);

		cSoundEntry Entry;
		Entry.m_pSound = pSound;
		Entry.m_fNormalVolume = a_fVolume;
		Entry.m_sName = a_sFileName;
		Entry.m_fNormalSpeed = 1.0f;

		Entry.m_fBlockFadeDest = 1;
		Entry.m_fBlockFadeSpeed = 1;
		Entry.m_fBlockMul = 1;
		Entry.m_bFirstTime = true;

		Entry.m_bStream = true;

		Entry.m_EffectType = a_EffectType;

		pSound->SetPositionRelative(true);
		pSound->SetRelPosition(cVector3f(0,0,1));
		cVector3f vPos = cMath::MatrixMul(m_pLowLevelSound->GetListenerMatrix(), pSound->GetRelPosition());
		pSound->SetPosition(vPos);

		m_lstGuiSounds.push_back(Entry);

		m_lIdCount++;

		return pSound;
	}

	//--------------------------------------------------------------

	iSoundChannel *cSoundHandler::PlayGui(const tString &a_sName, bool a_bLoop, float a_fVolume, const cVector3f &a_vPos,
			eSoundDest a_EffectType)
	{
		return Play(a_sName, a_bLoop, a_fVolume, a_vPos, 1.0f, 1000.0f, eSoundDest_Gui, true, false, 0, a_EffectType);
	}

	//--------------------------------------------------------------

	bool cSoundHandler::Stop(const tString &a_sName)
	{
		cSoundEntry *pEntry = GetEntry(a_sName);
		if (pEntry)
			pEntry->m_pSound->Stop();
		else
			return false;

		return true;
	}

	//--------------------------------------------------------------

	bool cSoundHandler::StopAllExcept(const tString &a_sName)
	{
		return false;
	}

	//--------------------------------------------------------------

	void cSoundHandler::StopAll(tFlag a_Types)
	{
		tSoundEntryListIt it;
		if (a_Types & eSoundDest_Gui)
		{
			it = m_lstGuiSounds.begin();
			while (it != m_lstGuiSounds.end())
			{
				it->m_pSound->SetPaused(false);
				it->m_pSound->Stop();
				it++;
			}
		}

		if (a_Types & eSoundDest_World)
		{
			it = m_lstWorldSounds.begin();
			while (it != m_lstWorldSounds.end())
			{
				it->m_pSound->SetPaused(false);
				it->m_pSound->Stop();
				it++;
			}
		}
	}

	//--------------------------------------------------------------

	void cSoundHandler::PauseAll(tFlag a_Types)
	{
		tSoundEntryListIt it;
		if (a_Types & eSoundDest_Gui)
		{
			it = m_lstGuiSounds.begin();
			while (it != m_lstGuiSounds.end())
			{
				it->m_pSound->SetPaused(true);
				it++;
			}
		}

		if (a_Types & eSoundDest_World)
		{
			it = m_lstWorldSounds.begin();
			while (it != m_lstWorldSounds.end())
			{
				it->m_pSound->SetPaused(true);
				it++;
			}
		}
	}

	//--------------------------------------------------------------

	void cSoundHandler::ResumeAll(tFlag a_Types)
	{
		tSoundEntryListIt it;
		if (a_Types & eSoundDest_Gui)
		{
			it = m_lstGuiSounds.begin();
			while (it != m_lstGuiSounds.end())
			{
				it->m_pSound->SetPaused(false);
				it++;
			}
		}

		if (a_Types & eSoundDest_World)
		{
			it = m_lstWorldSounds.begin();
			while (it != m_lstWorldSounds.end())
			{
				it->m_pSound->SetPaused(false);
				it++;
			}
		}
	}

	//--------------------------------------------------------------
	
	bool cSoundHandler::IsPlaying(const tString &a_sName)
	{
		cSoundEntry *pEntry = GetEntry(a_sName);

		if (pEntry) return pEntry->m_pSound->IsPlaying();

		return false;
	}

	//--------------------------------------------------------------

	bool cSoundHandler::IsValid(iSoundChannel *a_pChannel)
	{
		tSoundEntryListIt it;
		it = m_lstWorldSounds.begin();
		while (it != m_lstWorldSounds.end())
		{
			if (it->m_pSound == a_pChannel) return true;
			it++;
		}

		it = m_lstGuiSounds.begin();
		while (it != m_lstGuiSounds.end())
		{
			if (it->m_pSound == a_pChannel) return true;
			it++;
		}

		return false;
	}

	//--------------------------------------------------------------

	bool cSoundHandler::IsValidId(iSoundChannel *a_pChannel, int a_lId)
	{
		if (a_pChannel == NULL) return false;

		tSoundEntryListIt it;
		it = m_lstWorldSounds.begin();
		while (it != m_lstWorldSounds.end())
		{
			if (it->m_pSound == a_pChannel && it->m_pSound->GetId() == a_lId) return true;
			it++;
		}

		it = m_lstGuiSounds.begin();
		while (it != m_lstGuiSounds.end())
		{
			if (it->m_pSound == a_pChannel && it->m_pSound->GetId() == a_lId) return true;
			it++;
		}

		return false;
	}

	//--------------------------------------------------------------

	void cSoundHandler::Update(float a_fTimeStep)
	{
		if (m_fNewSpeed != m_fSpeed)
		{
			m_fSpeed += m_fSpeedRate;
			if (m_fSpeedRate<0 && m_fSpeed<m_fNewSpeed) m_fSpeed = m_fNewSpeed;
			if (m_fSpeedRate>0 && m_fSpeed>m_fNewSpeed) m_fSpeed = m_fNewSpeed;

			m_fVolume += m_fVolumeRate;
			if (m_fVolumeRate<0 && m_fVolume<m_fNewVolume) m_fVolume = m_fNewVolume;
			if (m_fVolumeRate>0 && m_fVolume>m_fNewVolume) m_fVolume = m_fNewVolume;
		}

		tSoundEntryListIt it;
		it = m_lstGuiSounds.begin();
		while (it != m_lstGuiSounds.end())
		{
			if (UpdateEntry(&(*it), a_fTimeStep, eSoundDest_Gui) == false)
				it = m_lstGuiSounds.erase(it);
			else
				++it;
		}

		it = m_lstWorldSounds.begin();
		while (it != m_lstWorldSounds.end())
		{
			if (UpdateEntry(&(*it), a_fTimeStep, eSoundDest_World) == false)
				it = m_lstWorldSounds.erase(it);
			else
				++it;
		}

		m_lCount++;
	}

	//--------------------------------------------------------------

	void cSoundHandler::SetSpeed(float a_fSpeed, float a_fRate, tFlag a_Types)
	{
		m_fNewSpeed = a_fSpeed;

		if (m_fNewSpeed>m_fSpeed && a_fRate<0) a_fRate = -a_fRate;
		if (m_fNewSpeed<m_fSpeed && a_fRate>0) a_fRate = -a_fRate;

		m_fSpeedRate = a_fRate;

		m_AffectedBySpeed = a_Types;

		if (a_fRate == 0) m_fSpeed = m_fNewSpeed;
	}

	//--------------------------------------------------------------

	void cSoundHandler::SetVolume(float a_fVolume, float a_fRate, tFlag a_Types)
	{
		m_fNewVolume = a_fVolume;

		if (m_fNewVolume>m_fVolume && a_fRate<0) a_fRate = -a_fRate;
		if (m_fNewVolume<m_fVolume && a_fRate>0) a_fRate = -a_fRate;

		m_fVolumeRate = a_fRate;

		m_AffectedByVolume = a_Types;

		if (a_fRate == 0) m_fVolume = m_fNewVolume;
	}

	//--------------------------------------------------------------

	void cSoundHandler::SetWorld3D(cWorld3D *a_pWorld3D)
	{
		m_pWorld3D = a_pWorld3D;
	}

	//--------------------------------------------------------------

	cSoundEntry *cSoundHandler::GetEntryFromSound(iSoundChannel *a_pChannel)
	{
		tSoundEntryListIt it = m_lstGuiSounds.begin();
		while (it != m_lstGuiSounds.end())
		{
			if (it->m_pSound == a_pChannel) 
				return &(*it);
			it++;
		}

		it = m_lstWorldSounds.begin();
		while (it != m_lstWorldSounds.end())
		{
			if (it->m_pSound == a_pChannel) 
				return &(*it);
			it++;
		}

		return NULL;
	}

	//--------------------------------------------------------------

	iSoundChannel *cSoundHandler::CreateChannel(const tString &a_sName, int a_lPriority)
	{
		int lNum = cString::ToInt(cString::GetLastChar(a_sName).c_str(), 0);
		iSoundChannel *pSound = NULL;
		iSoundData *pData = NULL;
		tString sName;
		tString sBaseName = a_sName;

		// Try loading it from the buffer
		if (lNum >= 1 && lNum <= 9)
			pData = m_pResources->GetSoundManager()->CreateSoundData(a_sName, false);
		else
		{
			int lCount = 0;
			int lLastNum = -1;

			tPlayedSoundNumMapIt SoundIt = m_mapPlayedSound.find(sBaseName);
			if (SoundIt == m_mapPlayedSound.end())
			{
				m_mapPlayedSound.insert(tPlayedSoundNumMap::value_type(sBaseName, 0));
				SoundIt = m_mapPlayedSound.find(sBaseName);
			}
			else
				lLastNum = SoundIt->second;

			sName = sBaseName + cString::ToString(lCount+1);
			pData = m_pResources->GetSoundManager()->CreateSoundData(sName, false);
			while (pData)
			{
				lCount++;
				sName = sBaseName +  cString::ToString(lCount+1);
				pData = m_pResources->GetSoundManager()->CreateSoundData(sName, false);
			}

			if (lCount > 0)
			{
				int lNum = cMath::RandRectl(1, lCount);

				if (lCount > 2)
				{
					while (lLastNum == lNum)
					{
						lNum = cMath::RandRectl(1, lCount);
					}

					SoundIt->second = lNum;

					sName = sBaseName +  cString::ToString(lNum);

					pData = m_pResources->GetSoundManager()->CreateSoundData(sName, false);
				}
			}
			else
				pData = NULL;
		}

		//Try to stream it
		if (pData == NULL)
		{
			sName = "stream_" + sBaseName;

			pData = m_pResources->GetSoundManager()->CreateSoundData(sName, true);
			if (pData == NULL)
			{
				Error("Couldn't stream sound '%s'\n", a_sName.c_str());
				return NULL;
			}
		}

		pSound = pData->CreateChannel(a_lPriority);
		
		return pSound;
	}

	//--------------------------------------------------------------

	tSoundEntryList *cSoundHandler::GetWorldEntryList()
	{
		return &m_lstWorldSounds;
	}

	tSoundEntryList *cSoundHandler::GetGuiEntryList()
	{
		return &m_lstGuiSounds;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSoundEntry *cSoundHandler::GetEntry(const tString &a_sName)
	{
		tSoundEntryListIt it = m_lstGuiSounds.begin();
		while (it != m_lstGuiSounds.end())
		{
			if (cString::ToLowerCase(it->m_sName) == cString::ToLowerCase(a_sName)) 
				return &(*it);
			it++;
		}

		it = m_lstWorldSounds.begin();
		while (it != m_lstWorldSounds.end())
		{
			if (cString::ToLowerCase(it->m_sName) == cString::ToLowerCase(a_sName)) 
				return &(*it);
			it++;
		}

		return NULL;
	}

	//--------------------------------------------------------------

	bool cSoundHandler::UpdateEntry(cSoundEntry *a_pEntry, float a_fTimeStep, tFlag a_Types)
	{
		a_pEntry->Update(a_fTimeStep);

		if (!a_pEntry->m_pSound->IsPlaying() && !a_pEntry->m_pSound->GetPaused())
		{
			iSoundChannel *pSound = a_pEntry->m_pSound;

			if (pSound->GetPaused()==false && pSound->GetCallback() && pSound->GetLooping() 
				&& a_pEntry->m_fNormalVolumeFadeDest != 0)
				pSound->GetCallback()->OnPriorityRelease();

			pSound->Stop();
			efeDelete(pSound);

			return false;
		}
		else
		{
			if (m_AffectedBySpeed & a_pEntry->m_EffectType)
			{
				float fSpeed = m_fSpeed * a_pEntry->m_fNormalSpeed;
				if (a_pEntry->m_pSound->GetSpeed() != fSpeed)
					a_pEntry->m_pSound->SetSpeed(fSpeed);
			}
			else
			{
				if (a_pEntry->m_pSound->GetSpeed() != a_pEntry->m_fNormalSpeed)
					a_pEntry->m_pSound->SetSpeed(a_pEntry->m_fNormalSpeed);
			}

			if (a_pEntry->m_fBlockMul != a_pEntry->m_fBlockFadeDest)
			{
				a_pEntry->m_fBlockMul += a_pEntry->m_fBlockFadeSpeed * a_fTimeStep;
				if (a_pEntry->m_fBlockFadeSpeed < 0)
				{
					if (a_pEntry->m_fBlockMul < a_pEntry->m_fBlockFadeDest)
						a_pEntry->m_fBlockMul = a_pEntry->m_fBlockFadeDest;
				}
				else
				{
					if (a_pEntry->m_fBlockMul > a_pEntry->m_fBlockFadeDest)
						a_pEntry->m_fBlockMul = a_pEntry->m_fBlockFadeDest;
				}
			}

			if (a_pEntry->m_bStream)
				a_pEntry->m_pSound->SetVolume(a_pEntry->m_fNormalVolume * a_pEntry->m_fNormalVolumeMul * m_fVolume);

			if (a_pEntry->m_pSound->Get3D())
				UpdateDistanceVolume3D(a_pEntry, a_fTimeStep, a_pEntry->m_bFirstTime ? false : true, a_Types);
			else
			{
				if (a_pEntry->m_pSound->GetPositionRelative())
				{
					iSoundChannel *pSound = a_pEntry->m_pSound;

					cVector3f vPos = cMath::MatrixMul(m_pLowLevelSound->GetListenerMatrix(), pSound->GetRelPosition());
					pSound->SetPosition(vPos);

					if (a_pEntry->m_EffectType & m_AffectedByVolume)
						pSound->SetVolume(a_pEntry->m_fNormalVolume * a_pEntry->m_fNormalVolumeMul * m_fVolume);
					else
						pSound->SetVolume(a_pEntry->m_fNormalVolume * a_pEntry->m_fNormalVolumeMul);
				}
				else
				{
					iSoundChannel *pSound = a_pEntry->m_pSound;
					float fDX = pSound->GetPosition().x - m_pLowLevelSound->GetListenerPosition().x;
					float fDY = pSound->GetPosition().y - m_pLowLevelSound->GetListenerPosition().y;
					
					float fDist = sqrt(fDX*fDX + fDY*fDY);

					if (fDist >= pSound->GetMaxDistance())
						pSound->SetVolume(0);
					else
					{
						if (fDist < pSound->GetMinDistance())
							pSound->SetVolume(a_pEntry->m_fNormalVolume);
						else
						{
							float fVolume = 1 - ((fDist - pSound->GetMinDistance()) /
								(pSound->GetMaxDistance() - pSound->GetMinDistance()));
							fVolume *= a_pEntry->m_fNormalVolume;

							pSound->SetVolume(fVolume);
						}
						float fPan = 1 - (0.5f - 0.4f * (fDX / pSound->GetMaxDistance()));
						pSound->SetPan(fPan);
					}
				}
			}
		}
		
		a_pEntry->m_bFirstTime = false;

		return true;
	}

	//--------------------------------------------------------------

	void cSoundHandler::UpdateDistanceVolume3D(cSoundEntry *a_pEntry, float a_fTimeStep, bool a_bFade, tFlag a_Types)
	{
		if (m_pWorld3D == NULL)
			return;

		if (a_pEntry->m_pSound->GetPositionRelative())
		{
			iSoundChannel *pSound = a_pEntry->m_pSound;

			cVector3f vPos = cMath::MatrixMul(m_pLowLevelSound->GetListenerMatrix(), pSound->GetRelPosition());
			pSound->SetPosition(vPos);

			if (a_pEntry->m_EffectType & m_AffectedByVolume)
				pSound->SetVolume(a_pEntry->m_fNormalVolume * a_pEntry->m_fNormalVolumeMul * m_fVolume);
			else
				pSound->SetVolume(a_pEntry->m_fNormalVolume * a_pEntry->m_fNormalVolumeMul);
		}
		else
		{
			iSoundChannel *pSound = a_pEntry->m_pSound;
			float fDist = cMath::Vector3DDist(pSound->GetPosition(), m_pLowLevelSound->GetListenerPosition());

			if (fDist >= pSound->GetMaxDistance())
			{
				pSound->SetVolume(0);

				pSound->SetPriority(0);
			}
			else
			{
				float fVolume = 0;
				bool bBlocked = false;

				if (pSound->GetBlockable() && m_pWorld3D && m_pWorld3D->GetPhysicsWorld() && (a_pEntry->m_lCount % 30) == 0)
				{
					iPhysicsWorld *pPhysicsWorld = m_pWorld3D->GetPhysicsWorld();

					m_SoundRayCallback.Reset();

					pPhysicsWorld->CastRay(&m_SoundRayCallback, pSound->GetPosition(),
						m_pLowLevelSound->GetListenerPosition(),
						false,false,false,true);

					if (m_SoundRayCallback.HasCollided())
					{
						a_pEntry->m_fBlockFadeDest = 0.0f;
						a_pEntry->m_fBlockFadeSpeed = -1.0f / 0.55f;

						if (a_bFade == false)
							a_pEntry->m_fBlockMul = 0.0f;

						pSound->SetFiltering(true, 0xF);
						bBlocked = true;
					}
					else
					{
						a_pEntry->m_fBlockFadeDest = 1;
						a_pEntry->m_fBlockFadeSpeed = 1.0f / 0.2f;

						if (a_bFade == false)
							a_pEntry->m_fBlockMul = 1.0f;
					}
				}

				++a_pEntry->m_lCount;

				if (fDist < pSound->GetMinDistance())
				{
					pSound->SetPriority(100);

					fVolume = a_pEntry->m_fNormalVolume;
				}
				else
				{
					pSound->SetPriority(10);

					float fDelta = fDist - pSound->GetMinDistance();
					float fMaxDelta = pSound->GetMaxDistance() - pSound->GetMinDistance();

					fVolume = 1 - (fDelta/fMaxDelta);
					float fSqr = fVolume * fVolume;

					fVolume = fVolume * a_pEntry->m_fBlockMul + (1.0f - a_pEntry->m_fBlockMul)*fSqr;

					fVolume *= a_pEntry->m_fNormalVolume;
				}

				float fBlock = pSound->GetBlockVolumeMul() + 
					a_pEntry->m_fBlockMul * (1 - pSound->GetBlockVolumeMul());

				if (a_Types & a_pEntry->m_EffectType)
					pSound->SetVolume(fBlock * fVolume * a_pEntry->m_fNormalVolumeMul * m_fVolume);
				else
					pSound->SetVolume(fBlock * fVolume * a_pEntry->m_fNormalVolumeMul);
			}
		}
	}
}