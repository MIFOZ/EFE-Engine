#ifndef EFE_SOUND_ENTITY_DATA_H
#define EFE_SOUND_ENTITY_DATA_H

#include "system/SystemTypes.h"
#include "resources/ResourceBase.h"

namespace efe
{
	class cSoundEntityData : public iResourceBase
	{
		friend class cSoundEntity;
	public:
		cSoundEntityData(tString a_sName);
		~cSoundEntityData();

		bool CreateFromFile(const tString &a_sFile);

		const tString &GetMainSoundName(){return m_sMainSound;}
		void SetMainSoundName(const tString &a_sName){m_sMainSound = a_sName;}
		const tString &GetStartSoundName(){return m_sStartSound;}
		void SetStartSoundName(const tString &a_sName){m_sStartSound = a_sName;}
		const tString &GetStopSoundName(){return m_sStopSound;}
		void SetMStopSoundName(const tString &a_sName){m_sStopSound = a_sName;}

		bool GetFadeStart(){return m_bFadeStart;}
		void SetFadeStart(bool a_bX){m_bFadeStart = a_bX;}
		bool GetFadeStop(){return m_bFadeStop;}
		void SetFadeStop(bool a_bX){m_bFadeStop = a_bX;}

		float GetVolume(){return m_fVolume;}
		void SetVolume(float a_fX){m_fVolume = a_fX;}
		float GetMaxDistance(){return m_fMaxDistance;}
		void SetMaxDistance(float a_fX){m_fMaxDistance = a_fX;}
		float GetMinDistance(){return m_fMinDistance;}
		void SetMinDistance(float a_fX){m_fMinDistance = a_fX;}

		bool GetStream(){return m_bStream;}
		void SetStream(bool a_bX){m_bStream = a_bX;}
		bool GetLoop(){return m_bLoop;}
		void SetLoop(bool a_bX){m_bLoop = a_bX;}
		bool GetUse3D(){return m_bUse3D;}
		void SetUse3D(bool a_bX){m_bUse3D = a_bX;}

		bool GetBlockable(){return m_bBlockable;}
		void SetBlockable(bool a_bX){m_bBlockable = a_bX;}
		float GetBlockVolumeMul(){return m_fBlockVolumeMul;}
		void SetBlockVolumeMul(float a_fX){m_fBlockVolumeMul = a_fX;}

		float GetRandom(){return m_fRandom;}
		void SetRandom(float a_fX){m_fRandom = a_fX;}
		float GetInterval(){return m_fInterval;}
		void SetInterval(float a_fX){m_fInterval = a_fX;}

		int GetPriority(){return m_lPriority;}
		void SetPriority(int a_lX){m_lPriority = a_lX;}

		// Resource implementation
		bool Reload(){return false;}
		void Unload(){}
		void Destroy(){}

	protected:
		tString m_sMainSound;
		tString m_sStartSound;
		tString m_sStopSound;

		bool m_bFadeStart;
		bool m_bFadeStop;

		float m_fVolume;
		float m_fMaxDistance;
		float m_fMinDistance;

		bool m_bStream;
		bool m_bLoop;
		bool m_bUse3D;

		bool m_bBlockable;
		float m_fBlockVolumeMul;

		float m_fRandom;
		float m_fInterval;

		int m_lPriority;
	};
};

#endif