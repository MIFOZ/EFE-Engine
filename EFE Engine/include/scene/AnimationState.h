#ifndef EFE_ANIMATION_STATE_H
#define EFE_ANIMATION_STATE_H

#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"

#include "game/SaveGame.h"

namespace efe
{
	class cAnimation;
	class cAnimationManager;

	//--------------------------------------------------------------

	kSaveData_BaseClass(cAnimationState)
	{
		kSaveData_ClassInit(cAnimationState)
	public:
		tString m_sName;

		tString m_sAnimationName;
		float m_fDefaultSpeed;

		float m_fLength;
		float m_fWeight;
		float m_fSpeed;
		float m_fTimePos;

		float m_fBaseSpeed;

		bool m_bActive;
		bool m_bLoop;
		bool m_bPaused;

		float m_fFadeStep;

		float m_fSpecialEventTime;

		virtual iSaveObject *CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);
		virtual int GetSaveCreatePrio();
	};

	//--------------------------------------------------------------

	class cAnimationEvent
	{
	public:
		float m_fTime;
		eAnimationEventType m_Type;
		tString m_sValue;
	};

	class cAnimationState : public iSaveObject
	{
	public:
		//************************************
		// Method:    cAnimationState
		// FullName:  efe::cAnimationState::cAnimationState
		// Access:    public 
		// Returns:   
		// Qualifier:
		// Parameter: cAnimation * a_pAnimation
		// Parameter: const tString & a_sName
		// Parameter: cAnimationManager * a_pAnimationManager
		//************************************
		cAnimationState(cAnimation *a_pAnimation, const tString &a_sName,
			cAnimationManager *a_pAnimationManager);
		~cAnimationState();
		
		const char *GetName(){return m_sName.c_str();};

		void Update(float a_fTimeStep);

		bool IsFading();

		bool IsOver();

		void FadeIn(float a_fTime);
		void FadeOut(float a_fTime);

		void SetLength(float a_fLength);
		float GetLength();

		void SetWeight(float a_fWeight);
		float GetWeight();

		void SetSpeed(float a_fSpeed);
		float GetSpeed();

		void SetBaseSpeed(float a_fSpeed);
		float GetBaseSpeed();

		void SetTimePosition(float a_fPosition);
		float GetTimePosition();
		float GetPreviousTimePosition();

		void SetRelativeTimePosition(float a_fPosition);
		float GetRelativeTimePosition();

		bool IsActive();
		void SetActive(bool a_bActive);

		bool IsLooping();
		void SetLoop(bool a_bLoop);

		bool IsPaused();
		void SetPaused(bool a_bPaused);

		void SetSpecialEventTime(float a_fT) {m_fSpecialEventTime = a_fT;}
		float GetSpecialEventTime() {return m_fSpecialEventTime;}
		bool IsAfterSpecialEvent();
		bool IsBeforSpecialEvent();

		void AddTimePosition(float a_fAdd);

		cAnimation *GetAnimation();

		cAnimationEvent *CreateEvent();
		cAnimationEvent *GetEvent(int a_lIdx);
		int GetEventNum();

		float GetFadeStep() {return m_fFadeStep;}
		void SetFadeStep(float a_fX) {m_fFadeStep = a_fX;}

		//SaveObject implementation
		virtual iSaveData *CreateSaveData();
		virtual void SaveToSaveData(iSaveData *a_pSaveData);
		virtual void LoadFromSaveData(iSaveData *a_pSaveData);
		virtual void SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);

	private:
		tString m_sName;

		cAnimationManager *m_pAnimationManager;

		cAnimation *m_pAnimation;

		std::vector<cAnimationEvent*> m_vEvents;

		float m_fLength;
		float m_fWeight;
		float m_fSpeed;
		float m_fTimePos;
		float m_fPrevTimePos;

		float m_fBaseSpeed;

		float m_fSpecialEventTime;

		bool m_bActive;
		bool m_bLoop;
		bool m_bPaused;

		float m_fFadeStep;
	};
};

#endif