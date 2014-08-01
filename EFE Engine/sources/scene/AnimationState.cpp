#include "scene/AnimationState.h"

#include "math/Math.h"
#include "graphics/Animation.h"
#include "resources/AnimationManager.h"

#include "system/LowLevelSystem.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cAnimationState::cAnimationState(cAnimation *a_pAnimation, const tString &a_sName,
			cAnimationManager *a_pAnimationManager)
	{
		m_pAnimation = a_pAnimation;

		m_pAnimationManager = a_pAnimationManager;
		
		m_fLength = m_pAnimation->GetLength();
		m_sName = a_sName;

		m_bActive = false;

		m_fTimePos = 0;
		m_fWeight = 1;
		m_fSpeed = 1.0f;
		m_fBaseSpeed = 1.0f;
		m_fTimePos = 0;
		m_fPrevTimePos = 0;

		m_bLoop = false;
		m_bPaused = false;

		m_fSpecialEventTime = 0;

		m_fFadeStep = 0;
	}

	//--------------------------------------------------------------

	cAnimationState::~cAnimationState()
	{
		STLDeleteAll(m_vEvents);

		if (m_pAnimationManager)
			m_pAnimationManager->Destroy(m_pAnimation);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cAnimationState::Update(float a_fTimeStep)
	{
		//Update animation
		AddTimePosition(a_fTimeStep);

		//Fading
		if (m_fFadeStep != 0)
		{
			m_fWeight += m_fFadeStep * a_fTimeStep;

			if (m_fFadeStep < 0 && m_fWeight <= 0)
			{
				m_fWeight = 0;
				m_bActive = false;
				m_fFadeStep = 0;
			}
			else if (m_fFadeStep > 0 && m_fWeight >= 1)
			{
				m_fWeight = 1;
				m_fFadeStep = 0;
			}
		}
	}

	//--------------------------------------------------------------

	bool cAnimationState::IsFading()
	{
		return m_fFadeStep != 0;
	}

	//--------------------------------------------------------------

	bool cAnimationState::IsOver()
	{
		if (m_bLoop) return false;

		return m_fTimePos >= m_fLength;
	}

	//--------------------------------------------------------------

	void cAnimationState::FadeIn(float a_fTime)
	{
		m_fFadeStep = 1.0f / std::abs(a_fTime);
	}

	void cAnimationState::FadeOut(float a_fTime)
	{
		m_fFadeStep = -1.0f / std::abs(a_fTime);
	}

	//--------------------------------------------------------------

	void cAnimationState::SetLength(float a_fLength)
	{
		m_fLength = a_fLength;
	}
	float cAnimationState::GetLength()
	{
		return m_fLength;
	}

	//--------------------------------------------------------------

	void cAnimationState::SetWeight(float a_fWeight)
	{
		m_fWeight = a_fWeight;
	}
	float cAnimationState::GetWeight()
	{
		return m_fWeight;
	}

	//--------------------------------------------------------------

	void cAnimationState::SetSpeed(float a_fSpeed)
	{
		m_fSpeed = a_fSpeed;
	}
	float cAnimationState::GetSpeed()
	{
		return m_fSpeed;
	}

	//--------------------------------------------------------------

	void cAnimationState::SetBaseSpeed(float a_fSpeed)
	{
		m_fBaseSpeed = a_fSpeed;
	}
	float cAnimationState::GetBaseSpeed()
	{
		return m_fBaseSpeed;
	}

	//--------------------------------------------------------------

	void cAnimationState::SetTimePosition(float a_fPosition)
	{
		if (m_bLoop)
			m_fTimePos = cMath::Wrap(a_fPosition, 0, m_fLength);
		else
			m_fTimePos = cMath::Clamp(a_fPosition, 0, m_fLength);
	}

	float cAnimationState::GetTimePosition()
	{
		return m_fTimePos;
	}

	float cAnimationState::GetPreviousTimePosition()
	{
		return m_fPrevTimePos;
	}

	//--------------------------------------------------------------

	void cAnimationState::SetRelativeTimePosition(float a_fPosition)
	{
		SetTimePosition(a_fPosition * m_fLength);
	}

	float cAnimationState::GetRelativeTimePosition()
	{
		return m_fTimePos / m_fLength;
	}

	//--------------------------------------------------------------

	bool cAnimationState::IsActive()
	{
		return m_bActive;
	}

	void cAnimationState::SetActive(bool a_bActive)
	{
		if (m_bActive == a_bActive) return;

		m_bActive = a_bActive;

		m_bPaused = false;
		m_fFadeStep = 0;
	}

	//--------------------------------------------------------------

	bool cAnimationState::IsLooping()
	{
		return m_bLoop;
	}
	void cAnimationState::SetLoop(bool a_bLoop)
	{
		m_bLoop = a_bLoop;
	}

	//--------------------------------------------------------------

	bool cAnimationState::IsPaused()
	{
		return m_bPaused;
	}
	void cAnimationState::SetPaused(bool a_bPaused)
	{
		m_bPaused = a_bPaused;
	}

	//--------------------------------------------------------------

	bool cAnimationState::IsAfterSpecialEvent()
	{
		return m_fTimePos > m_fSpecialEventTime;
	}
	bool cAnimationState::IsBeforSpecialEvent()
	{
		return m_fTimePos < m_fSpecialEventTime;
	}

	//--------------------------------------------------------------

	void cAnimationState::AddTimePosition(float a_fAdd)
	{
		if (m_bPaused) return;

		m_fPrevTimePos = m_fTimePos;

		m_fTimePos += a_fAdd*m_fSpeed*m_fBaseSpeed;

		SetTimePosition(m_fTimePos);
	}

	//--------------------------------------------------------------

	cAnimation *cAnimationState::GetAnimation()
	{
		return m_pAnimation;
	}

	//--------------------------------------------------------------

	cAnimationEvent *cAnimationState::CreateEvent()
	{
		cAnimationEvent *pEvent = efeNew(cAnimationEvent, ());
		pEvent->m_fTime = 0;
		pEvent->m_Type = eAnimationEventType_LastEnum;
		pEvent->m_sValue = "";
		m_vEvents.push_back(pEvent);

		return pEvent;
	}

	cAnimationEvent *cAnimationState::GetEvent(int a_lIdx)
	{
		return m_vEvents[a_lIdx];
	}

	int cAnimationState::GetEventNum()
	{
		return (int)m_vEvents.size();
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// SAVE OBJECT TYPES
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	kBeginSerializeBase(cSaveData_cAnimationState)
		kSerializeVar(m_sName, eSerializeType_String)

		kSerializeVar(m_sAnimationName, eSerializeType_String)
		kSerializeVar(m_fDefaultSpeed, eSerializeType_Float32)

		kSerializeVar(m_fLength, eSerializeType_String)
		kSerializeVar(m_fWeight, eSerializeType_String)
		kSerializeVar(m_fSpeed, eSerializeType_String)
		kSerializeVar(m_fTimePos, eSerializeType_String)

		kSerializeVar(m_bActive, eSerializeType_Bool)
		kSerializeVar(m_bLoop, eSerializeType_Bool)
		kSerializeVar(m_bPaused, eSerializeType_Bool)
	
		kSerializeVar(m_fFadeStep, eSerializeType_Float32)
	kEndSerialize()

	//--------------------------------------------------------------

	iSaveObject *cSaveData_cAnimationState::CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		return NULL;
	}

	//--------------------------------------------------------------

	int cSaveData_cAnimationState::GetSaveCreatePrio()
	{
		return 2;
	}

	//--------------------------------------------------------------

	iSaveData *cAnimationState::CreateSaveData()
	{
		return efeNew(cSaveData_cAnimationState, ());
	}

	//--------------------------------------------------------------

	void cAnimationState::SaveToSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_SaveToBegin(cAnimationState);

		kSaveData_SaveTo(m_sName);

		pData->m_sAnimationName = m_pAnimation->GetFileName();
		kSaveData_SaveTo(m_fBaseSpeed);

		kSaveData_SaveTo(m_fLength);
		kSaveData_SaveTo(m_fWeight);
		kSaveData_SaveTo(m_fSpeed);
		kSaveData_SaveTo(m_fTimePos);

		kSaveData_SaveTo(m_bActive);
		kSaveData_SaveTo(m_bLoop);
		kSaveData_SaveTo(m_bPaused);

		kSaveData_SaveTo(m_fFadeStep);
	}

	//--------------------------------------------------------------

	void cAnimationState::LoadFromSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_LoadFromBegin(cAnimationState);

		kSaveData_LoadFrom(m_sName);

		kSaveData_LoadFrom(m_fLength);
		kSaveData_LoadFrom(m_fWeight);
		kSaveData_LoadFrom(m_fSpeed);
		kSaveData_LoadFrom(m_fTimePos);

		kSaveData_SaveTo(m_fBaseSpeed);

		kSaveData_LoadFrom(m_bActive);
		kSaveData_LoadFrom(m_bLoop);
		kSaveData_LoadFrom(m_bPaused);

		kSaveData_LoadFrom(m_fFadeStep);
	}

	//--------------------------------------------------------------

	void cAnimationState::SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		kSaveData_SetupBegin(cAnimationState);
	}
}