#include "physics/SurfaceData.h"

#include "physics/Physics.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "system/LowLevelSystem.h"
#include "math/Math.h"

#include "scene/World3D.h"
#include "scene/SoundEntity.h"
#include "sound/Sound.h"

#include "sound/SoundHandler.h"

#include "resources/Resources.h"
#include "resources/SoundEntityManager.h"


namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSurfaceData::cSurfaceData(const tString &a_sName, cPhysics *a_pPhysics, cResources *a_pResources)
	{
		m_sName = a_sName;
		m_pPhysics = a_pPhysics;
		m_pResources = a_pResources;

		m_FrictionMode = ePhysicsMaterialCombMode_Average;
		m_ElasticityMode = ePhysicsMaterialCombMode_Average;

		m_fElasticity = 0.5f;
		m_fStaticFriction = 0.3f;
		m_fKineticFriction = 0.3f;

		m_lPriority = 0;

		m_fMinScrapeSpeed = 0.6f;
		m_fMinScrapeFreq = 0.7f;
		m_fMinScrapeFreqSpeed = 1;
		m_fMaxScrapeFreq = 2;
		m_fMaxScrapeFreqSpeed = 3;
		m_fMiddleScrapeSpeed = 2;
		m_sScrapeSoundName = "";
		m_sRollSoundName = "";
	}

	//--------------------------------------------------------------

	cSurfaceData::~cSurfaceData()
	{
		STLDeleteAll(m_vImpactData);
		STLDeleteAll(m_vHitData);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cSurfaceData::OnImpact(float a_fSpeed, const cVector3f &a_vPos, int a_lContacts, iPhysicsBody *a_pBody)
	{
		if (m_pPhysics->CanPlayImpact() == false)
			return;

		a_pBody->SetHasImpact(true);

		cWorld3D *pWorld = m_pPhysics->GetGameWorld();
		if (pWorld == NULL)
			return;

		cSurfaceImpactData *pData = NULL;
		for (size_t i = 0; i < m_vImpactData.size(); i++)
		{
			if (m_vImpactData[i]->GetMinSpeed() <= a_fSpeed)
			{
				pData = m_vImpactData[i];
				break;
			}
		}

		if (pData == NULL)
			return;

		if (pData->GetSoundName() != "")
		{
			m_pPhysics->AddImpact();

			cSoundEntity *pEntity = pWorld->CreateSoundEntity("Impact", pData->GetSoundName(), true);
			if (pEntity)
				pEntity->SetPosition(a_vPos);
		}
	}

	void cSurfaceData::OnSlide(float a_fSpeed, const cVector3f &a_vPos, int a_lContacts, iPhysicsBody *a_pBody,
		iPhysicsBody *a_pSlideAgainstBody)
	{
		if (a_lContacts < m_lMinScrapeContacts) return;

		if (a_pBody->GetScrapeBody() != NULL &&
			a_pSlideAgainstBody != a_pBody->GetScrapeBody())
			return;

		cWorld3D *pWorld = m_pPhysics->GetGameWorld();
		if (pWorld == NULL)	return;

		cSoundHandler *pSoundHandler = pWorld->GetSound()->GetSoundHandler();
		if (pSoundHandler->GetSilent()) return;

		if (pWorld->SoundEntityExists(a_pBody->GetScrapeSoundEntity()) == false)
			a_pBody->SetScrapeSoundEntity(NULL);

		if (a_pBody->GetMass() != 0)
		{
			if (a_pBody->GetPrevScrapeMatrix() == a_pBody->GetLocalMatrix())
				a_fSpeed = 0;

			a_pBody->SetPrevScrapeMatrix(a_pBody->GetLocalMatrix());
		}

		if (a_pBody->GetScrapeSoundEntity() != NULL)
		{
			float fMin = cMath::Max(m_fMinScrapeSpeed-0.7f, 0.02f);
			if (std::abs(a_fSpeed) < fMin)
			{
				a_pBody->GetScrapeSoundEntity()->FadeOut(4.3f);
				a_pBody->SetScrapeSoundEntity(NULL);
				a_pBody->SetScrapeBody(NULL);
			}
			else
			{
				a_pBody->SetHasSlide(true);

				float fAbsSpeed = std::abs(a_fSpeed);
				float fFreq = 1;

				if (fAbsSpeed >= m_fMiddleScrapeSpeed)
				{
					if (fAbsSpeed >= m_fMaxScrapeFreqSpeed)
						fFreq = m_fMaxScrapeFreq;
					else
					{
						float fT = (fAbsSpeed - m_fMiddleScrapeSpeed) /
							(m_fMaxScrapeFreqSpeed - m_fMiddleScrapeSpeed);

						fFreq = (1 - fT) + fT * m_fMaxScrapeFreq;
					}
				}
				else
				{
					if (fAbsSpeed <= m_fMinScrapeFreqSpeed)
						fFreq = m_fMinScrapeFreq;
					else
					{
						float fT = (m_fMiddleScrapeSpeed - fAbsSpeed) /
							(m_fMiddleScrapeSpeed - m_fMinScrapeFreqSpeed);

						fFreq = (1 - fT) + fT * m_fMinScrapeFreq;
					}
				}

				cSoundEntry *pEntry = a_pBody->GetScrapeSoundEntity()->GetSoundEntry(eSoundEntityType_Main);
				if (pEntry)
				{
					pEntry->m_fNormalSpeed = fFreq;
					a_pBody->GetScrapeSoundEntity()->SetPosition(a_vPos);
				}
			}
		}
		else
		{
			if (m_fMinScrapeSpeed <= std::abs(a_fSpeed) && m_sScrapeSoundName != "")
			{
				a_pBody->SetHasSlide(true);

				cSoundEntity *pEntity = pWorld->CreateSoundEntity("Scrape", m_sScrapeSoundName, true);

				if (pEntity)
				{
					pEntity->FadeIn(3.3f);
					pEntity->SetPosition(a_vPos);

					pEntity->SetIsSaved(false);

					a_pBody->SetScrapeSoundEntity(pEntity);
					a_pBody->SetScrapeBody(a_pSlideAgainstBody);
				}
			}
		}
	}

	//--------------------------------------------------------------

	void cSurfaceData::CreateImpactEffect(float a_fSpeed, const cVector3f &a_vPos, int a_lContacts,
			cSurfaceData *a_pSecondSurface)
	{
		if (a_fSpeed == 0) return;

		cSurfaceImpactData *pDataA = NULL;
		cSurfaceImpactData *pDataB = NULL;

		cWorld3D *pWorld =  m_pPhysics->GetGameWorld();
		if (pWorld == NULL)
			return;

		cSoundHandler *pSoundHandler = pWorld->GetSound()->GetSoundHandler();
		if (pSoundHandler->GetSilent()) return;

		for (size_t i=0; i<m_vImpactData.size(); i++)
		{
			if (m_vImpactData[i]->GetMinSpeed() <= a_fSpeed)
			{
				pDataA = m_vImpactData[i];
				break;
			}
		}

		if (a_pSecondSurface != this && a_pSecondSurface != NULL)
		{
			for (size_t i=0; i<a_pSecondSurface->m_vImpactData.size(); i++)
			{
				if (a_pSecondSurface->m_vImpactData[i]->GetMinSpeed() <= a_fSpeed)
				{
					pDataB = a_pSecondSurface->m_vImpactData[i];
					break;
				}
			}
		}

		tString sPS = "";

		if (pDataA && !pDataB)
			sPS = pDataA->GetPSName();
		else if (!pDataA && pDataB)
			sPS = pDataB->GetPSName();
		else if (pDataA && pDataB)
		{
			if (pDataA->GetPSPrio() >= pDataB->GetPSPrio())
				sPS = pDataA->GetPSName();
			else
				sPS = pDataB->GetPSName();
		}

		if (sPS != "")
		{
			cMatrixf mtxPos = cMath::MatrixTranslate(a_vPos);
			_ASSERT(0);// pWorld->cre
		}
	}

		//--------------------------------------------------------------

	void cSurfaceData::UpdateRollEffect(iPhysicsBody *a_pBody)
	{
		if (m_sRollSoundName == "" || m_RollAxisFlags == 0) return;
	}

	//--------------------------------------------------------------

	void cSurfaceData::SetElasticity(float a_fElasticity)
	{
		m_fElasticity = a_fElasticity;
	}

	float cSurfaceData::GetElasticity()
	{
		return m_fElasticity;
	}

	//--------------------------------------------------------------

	void cSurfaceData::SetStaticFriction(float a_fFriction)
	{
		m_fStaticFriction = a_fFriction;
	}

	float cSurfaceData::GetStaticFriction() const
	{
		return m_fStaticFriction;
	}

	//--------------------------------------------------------------

	void cSurfaceData::SetKineticFriction(float a_fFriction)
	{
		m_fKineticFriction = a_fFriction;
	}

	float cSurfaceData::GetKineticFriction() const
	{
		return m_fKineticFriction;
	}

	//--------------------------------------------------------------

	void cSurfaceData::SetPriority(int a_lPriority)
	{
		m_lPriority = a_lPriority;
	}

	int cSurfaceData::GetPriority() const
	{
		return m_lPriority;
	}

	//--------------------------------------------------------------

	void cSurfaceData::SetFrictionCombMode(ePhysicsMaterialCombMode a_Mode)
	{
		m_FrictionMode = a_Mode;
	}

	ePhysicsMaterialCombMode cSurfaceData::GetFrictionCombMode() const
	{
		return m_FrictionMode;
	}

	//--------------------------------------------------------------

	void cSurfaceData::SetElasticityCombMode(ePhysicsMaterialCombMode a_Mode)
	{
		m_ElasticityMode = a_Mode;
	}

	ePhysicsMaterialCombMode cSurfaceData::GetElasticityCombMode() const
	{
		return m_ElasticityMode;
	}

	//--------------------------------------------------------------

	void cSurfaceData::PreloadData()
	{
		if (m_sRollSoundName != "")m_pResources->GetSoundEntityManager()->Preload(m_sRollSoundName);
		if (m_sScrapeSoundName != "")m_pResources->GetSoundEntityManager()->Preload(m_sScrapeSoundName);

		for (size_t i=0; i<m_vImpactData.size(); ++i)
		{
			if (m_vImpactData[i]->m_sSoundName != "")
				m_pResources->GetSoundEntityManager()->Preload(m_vImpactData[i]->m_sSoundName);
			if (m_vImpactData[i]->m_sPSName != "")
				m_pResources->GetSoundEntityManager()->Preload(m_vImpactData[i]->m_sPSName);
		}
		for (size_t i=0; i < m_vHitData.size(); ++i)
		{
			if (m_vHitData[i]->m_sSoundName != "")
				m_pResources->GetSoundEntityManager()->Preload(m_vHitData[i]->m_sSoundName);
			if (m_vHitData[i]->m_sPSName != "")
				m_pResources->GetSoundEntityManager()->Preload(m_vHitData[i]->m_sPSName);
		}
	}

	//--------------------------------------------------------------

	iPhysicsMaterial *cSurfaceData::ToMaterial(iPhysicsWorld *a_pWorld)
	{
		iPhysicsMaterial *pMat = NULL;

		pMat = a_pWorld->GetMaterialFromName(m_sName);

		if (pMat==NULL)
			pMat = a_pWorld->CreateMaterial(m_sName);

		pMat->SetElasticity(m_fElasticity);
		pMat->SetKineticFriction(m_fKineticFriction);
		pMat->SetStaticFriction(m_fStaticFriction);

		pMat->SetElasticityCombMode(m_ElasticityMode);
		pMat->SetFrictionCombMode(m_FrictionMode);

		pMat->SetSurfaceData(this);

		return pMat;
	}

	//--------------------------------------------------------------

	cSurfaceImpactData *cSurfaceData::CreateImpactData(float a_fMinSpeed)
	{
		cSurfaceImpactData *pData = efeNew(cSurfaceImpactData, ());
		pData->m_fMinSpeed = a_fMinSpeed;

		m_vImpactData.push_back(pData);
		return pData;
	}

	cSurfaceImpactData *cSurfaceData::GetImpactData(int a_lIdx)
	{
		return m_vImpactData[a_lIdx];
	}

	int cSurfaceData::GetImpactDataNum()
	{
		return (int)m_vImpactData.size();
	}

	cSurfaceImpactData *cSurfaceData::GetImpactDataFromSpeed(float a_fSpeed)
	{
		for (size_t i = 0; i < (int)m_vImpactData.size(); ++i)
		{
			if (a_fSpeed >= m_vImpactData[i]->GetMinSpeed())
				return m_vImpactData[i];
		}
		return NULL;
	}

	//--------------------------------------------------------------

	cSurfaceImpactData *cSurfaceData::CreateHitData(float a_fMinSpeed)
	{
		cSurfaceImpactData *pData = efeNew(cSurfaceImpactData, ());
		pData->m_fMinSpeed = a_fMinSpeed;

		m_vHitData.push_back(pData);
		return pData;
	}

	cSurfaceImpactData *cSurfaceData::GetHitData(int a_lIdx)
	{
		return m_vHitData[a_lIdx];
	}

	int cSurfaceData::GetHitDataNum()
	{
		return (int)m_vHitData.size();
	}

	cSurfaceImpactData *cSurfaceData::GetHitDataFromSpeed(float a_fSpeed)
	{
		for (size_t i = 0; i < (int)m_vHitData.size(); ++i)
		{
			if (a_fSpeed >= m_vHitData[i]->GetMinSpeed())
				return m_vHitData[i];
		}
		return NULL;
	}
}