#ifndef EFE_SOUND_ENTITY_H
#define EFE_SOUND_ENTITY_H

#include "scene/Entity3D.h"
#include "sound/SoundChannel.h"

namespace efe
{
	class cSoundHandler;
	class cSoundEntityData;
	class cSoundEntityManager;
	class cWorld3D;
	class cSoundEntry;

	enum eSoundEntityType
	{
		eSoundEntityType_Main,
		eSoundEntityType_Start,
		eSoundEntityType_Stop,
		eSoundEntityType_LastEnum
	};

	class cSoundEntity;

	class cSoundEnityChannelCallback : public iSoundChannelCallback
	{
	public:
		void OnPriorityRelease();

		cSoundEntity *m_pEntity;
	};

	kSaveData_ChildClass(iEntity3D, cSoundEntity)
	{
		kSaveData_ClassInit(cSoundEntity)
	public:
		tString m_sData;
		bool m_bStopped;
		bool m_bRemoveWhenOver;
		bool m_bStarted;
		bool m_bFadingOut;
		float m_fVolume;

		virtual iSaveObject *CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);
		virtual int GetSaveCreatePrio();
	};

	class iSoundEntityGlobalCallback
	{
	public:
		virtual void OnStart(cSoundEntity *a_pSoundEntity)=0;
	};

	typedef std::list<iSoundEntityGlobalCallback*> tSoundEntityGlobalCallbackList;
	typedef tSoundEntityGlobalCallbackList::iterator tSoundEntityGlobalCallbackListIt;

	class cSoundEntity : public iEntity3D
	{
		friend class cSoundEnityChannelCallback;
	public:
		cSoundEntity(const tString &a_sName, cSoundEntityData *a_pData,
			cSoundEntityManager *a_pSoundEntityManager,
			cWorld3D *a_pWorld,
			cSoundHandler *a_pSoundHandler, bool a_bRemoveWhenOver);
		~cSoundEntity();

		void Play(bool a_bPlayStart = true);
		void Stop(bool a_bPlayEnd = true);

		void FadeIn(float a_fSpeed);
		void FadeOut(float a_fSpeed);

		bool IsStopped();
		bool IsFadingOut();
		bool GetRemoveWhenOver();

		void SetVolume(float a_fVolume) {m_fVolume = a_fVolume;}
		float GetVolume() {return m_fVolume;}

		iSoundChannel *GetSound(eSoundEntityType a_Type) {return m_vSounds[a_Type];}
		cSoundEntry *GetSoundEntry(eSoundEntityType a_Type);

		//Entity implementation
		void UpdateLogic(float a_fTimeStep);

		tString GetEntityType(){return "SoundEntity";}

		cSoundEntityData *GetData(){return m_pData;}

		static void AddGlobalCallback(iSoundEntityGlobalCallback *a_pCallback);
		static void RemoveGlobalCallback(iSoundEntityGlobalCallback *a_pCallback);

		//SaveObject implementation
		virtual iSaveData *CreateSaveData();
		virtual void SaveToSaveData(iSaveData *a_pSaveData);
		virtual void LoadFromSaveData(iSaveData *a_pSaveData);
		virtual void SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);

	private:
		bool PlaySound(const tString &a_sName, bool a_bLoop, eSoundEntityType a_Type);

		float GetListenerSqrLength();

		cSoundEntityManager *m_pSoundEntityManager;
		cSoundHandler *m_pSoundHandler;
		cSoundEntityData *m_pData;
		cWorld3D *m_pWorld;

		iSoundChannel *m_vSounds[3];
		int m_vSoundId[3];

		bool m_bStopped;
		bool m_bRemoveWhenOver;

		bool m_bOutOfRange;

		float m_fIntervalCount;

		cSoundEnityChannelCallback *m_pSoundCallback;

		bool m_bStarted;

		bool m_bFadingOut;

		bool m_bLog;

		float m_fVolume;

		bool m_bPrioRemove;

		bool m_bSkipStartEnd;

		float m_fSleepCount;

		static tSoundEntityGlobalCallbackList m_lstGlobalCallbacks;
	};
};

#endif