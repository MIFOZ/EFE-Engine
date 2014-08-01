#ifndef EFE_SOUNDHANDLER_H
#define EFE_SOUNDHANDLER_H

#include <list>
#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "physics/PhysicsWorld.h"

namespace efe
{
	class iLowLevelSound;
	class iSoundChannel;
	class cWorld3D;

	enum eSoundDest
	{
		eSoundDest_World = eFlagBit_0,
		eSoundDest_Gui = eFlagBit_1,
		eSoundDest_All = eFlagBit_All
	};

	class cSoundRayCallback : public iPhysicsRayCallback
	{
	public:
		void Reset();
		bool HasCollided(){return m_bHasCollided;}

		bool BeforeIntersect(iPhysicsBody *a_pBody);
		bool OnIntersect(iPhysicsBody *a_pBody, cPhysicsRayParams *a_pParams);

	private:
		bool m_bHasCollided;
		int m_lCount;
	};

	class cSoundEntry
	{
	public:
		cSoundEntry() : m_fNormalVolume(1), m_fNormalVolumeFadeDest(1), 
			m_fNormalVolumeMul(1), m_fNormalVolumeFadeSpeed(0), m_bStream(false),
			m_lCount(0){}

		void Update(float a_fTimeStep);

		tString m_sName;
		iSoundChannel *m_pSound;

		float m_fNormalVolume;
		float m_fNormalVolumeMul;
		float m_fNormalVolumeFadeDest;
		float m_fNormalVolumeFadeSpeed;

		float m_fNormalSpeed;

		bool m_bFirstTime;

		float m_fBlockMul;
		float m_fBlockFadeDest;
		float m_fBlockFadeSpeed;

		bool m_bStream;

		long int m_lCount;

		eSoundDest m_EffectType;
	};

	typedef std::list<cSoundEntry> tSoundEntryList;
	typedef tSoundEntryList::iterator tSoundEntryListIt;

	typedef cSTLIterator<cSoundEntry, tSoundEntryList, tSoundEntryListIt> tSoundEntryIterator;

	typedef std::map<tString, int> tPlayedSoundNumMap;
	typedef tPlayedSoundNumMap::iterator tPlayedSoundNumMapIt;

	class cResources;

	class cSoundHandler
	{
	public:
		cSoundHandler(iLowLevelSound *a_pLowLevelSound, cResources *a_pResources);
		~cSoundHandler();

		iSoundChannel *Play(const tString &a_sName, bool a_bLoop, float a_fVolume, const cVector3f &a_vPos,
			float a_fMinDist, float a_fMaxDist, eSoundDest a_Type, bool a_bRelative, bool a_b3D = false,
			int a_lPriorityModifier = 0, eSoundDest a_EffectType = eSoundDest_World);

		iSoundChannel *Play3D(const tString &a_sName, bool a_bLoop, float a_fVolume, const cVector3f &a_vPos,
			float a_fMinDist, float a_fMaxDist, eSoundDest a_Type, bool a_bRelative, int a_lPriorityModifier = 0,
			eSoundDest a_EffectType = eSoundDest_World)
		{
			return Play(a_sName, a_bLoop, a_fVolume, a_vPos, a_fMinDist, a_fMaxDist, a_Type, a_bRelative, true,
				a_lPriorityModifier, a_EffectType);
		}

		iSoundChannel *PlayStream(const tString &a_sFileName, bool a_bLoop, float a_fVolume, bool a_b3D = false,
			eSoundDest a_EffectType = eSoundDest_Gui);

		iSoundChannel *PlayGui(const tString &a_sName, bool a_bLoop, float a_fVolume, const cVector3f &a_vPos = cVector3f(0,0,1),
			eSoundDest a_EffectType = eSoundDest_Gui);

		void SetSilent(bool a_bX){m_bSilent = a_bX;}
		bool GetSilent(){return m_bSilent;}

		bool Stop(const tString &a_sName);
		bool StopAllExcept(const tString &a_sName);

		void StopAll(tFlag a_Types);
		void PauseAll(tFlag a_Types);
		void ResumeAll(tFlag a_Types);

		bool IsPlaying(const tString &a_sName);

		bool IsValid(iSoundChannel *a_pChannel);
		bool IsValidId(iSoundChannel *a_pChannel, int a_lId);

		void Update(float a_fTimeStep);

		void SetSpeed(float a_fSpeed, float a_fRate, tFlag a_Types);
		void SetVolume(float a_fVolume, float a_fRate, tFlag a_Types);

		float GetVolume(){return m_fVolume;}

		void SetWorld3D(cWorld3D *a_pWorld3D);

		cSoundEntry *GetEntryFromSound(iSoundChannel *a_pChannel);

		iSoundChannel *CreateChannel(const tString &a_sName, int a_lPriority);

		tSoundEntryList *GetWorldEntryList();
		tSoundEntryList *GetGuiEntryList();

	private:
		iLowLevelSound *m_pLowLevelSound;
		cResources *m_pResources;

		tSoundEntryList m_lstGuiSounds;
		tSoundEntryList m_lstWorldSounds;

		bool m_bSilent;

		float m_fSpeed;
		float m_fNewSpeed;
		float m_fSpeedRate;
		tFlag m_AffectedBySpeed;

		float m_fVolume;
		float m_fNewVolume;
		float m_fVolumeRate;
		tFlag m_AffectedByVolume;

		cWorld3D *m_pWorld3D;

		cSoundRayCallback m_SoundRayCallback;

		tPlayedSoundNumMap m_mapPlayedSound;

		cSoundEntry *GetEntry(const tString &a_sName);
		bool UpdateEntry(cSoundEntry *a_pEntry, float a_fTimeStep, tFlag a_Types);
		void UpdateDistanceVolume3D(cSoundEntry *a_pEntry, float a_fTimeStep, bool a_bFade, tFlag a_Types);

		int m_lCount;
		int m_lIdCount;
	};
};

#endif