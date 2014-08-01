#ifndef EFE_SOUND_CHANNEL_H
#define EFE_SOUND_CHANNEL_H

#include <list>
#include "math/MathTypes.h"

namespace efe
{
	class iSoundData;
	class cSoundManager;

	class iSoundChannelCallback
	{
	public:
		virtual void OnPriorityRelease()=0;
	};

	class iSoundChannel
	{
	public:
		iSoundChannel(iSoundData *a_pData, cSoundManager *a_pSoundManager) : m_pData(a_pData)
		{
			m_bLooping = false;
			m_bPaused = true;
			m_bPositionRelative = false;

			m_fSpeed = 1;
			m_fVolume = 1;
			m_fMaxDistance = 0;
			m_fMinDistance = 0;

			m_pSoundManager = a_pSoundManager;

			m_vVelocity = cVector3f(0,0,0);
			m_vPosition = cVector3f(0,0,0);
			m_vRelPosition = cVector3f(0,0,0);

			m_bBlockable = false;
			m_fBlockVolumeMul = 1;

			m_bAffectedByEnv = false;

			m_lPriorityModifier = 0;

			m_bStopUsed = false;

			m_pCallback = NULL;
		}

		virtual ~iSoundChannel(){}

		virtual void Play()=0;
		virtual void Stop()=0;

		virtual void SetPaused(bool a_bX)=0;
		virtual void SetSpeed(float a_fSpeed)=0;
		virtual void SetVolume(float a_fVolume)=0;
		virtual void SetLooping(bool a_bLoop)=0;
		virtual void SetPan(float a_fPan)=0;
		virtual void Set3D(bool a_b3D)=0;

		virtual void SetPriority(int a_lX)=0;
		virtual int GetPriority()=0;

		void SetPriorityModifier(int a_lX)
		{
			m_lPriorityModifier = a_lX;
			SetPriority(GetPriority());
		}
		int GetPriorityModifier(){return m_lPriorityModifier;}

		virtual void SetPositionRelative(bool a_bRelative)=0;
		virtual void SetPosition(const cVector3f &a_vPos)=0;
		void SetRelPosition(const cVector3f &a_vPos){m_vRelPosition = a_vPos;}

		virtual void SetVelocity(const cVector3f &a_vVel)=0;

		virtual void SetMinDistance(float a_fMin)=0;
		virtual void SetMaxDistance(float a_fMax)=0;

		virtual bool IsPlaying()=0;
		virtual bool IsBufferUnderrun()=0;
		virtual double GetElapsedTime()=0;
		virtual double GetTotalTime()=0;

		bool GetPaused(){return m_bPaused;}
		float GetSpeed(){return m_fSpeed;}
		float GetVolume(){return m_fVolume;}
		bool GetLooping(){return m_bLooping;}
		float GetPan(){return m_fPan;}
		bool Get3D(){return m_b3D;}

		bool GetStopUsed(){return m_bStopUsed;}

		bool GetBlockable(){return m_bBlockable;}
		void SetBlockable(bool a_bX){m_bBlockable = a_bX;}
		float GetBlockVolumeMul(){return m_fBlockVolumeMul;}
		void SetBlockVolumeMul(float a_fX){m_fBlockVolumeMul = a_fX;}

		bool GetPositionRelative(){return m_bPositionRelative;}

		const cVector3f &GetRelPosition(){return m_vRelPosition;}
		const cVector3f &GetPosition(){return m_vPosition;}
		const cVector3f &GetVelocity(){return m_vVelocity;}

		float GetMinDistance(){return m_fMinDistance;}
		float GetMaxDistance(){return m_fMaxDistance;}

		iSoundChannelCallback *GetCallback(){return m_pCallback;}
		void SetCallback(iSoundChannelCallback *a_pCallback){m_pCallback = a_pCallback;}

		int GetId(){return m_lId;}
		void SetId(int a_lId){m_lId = a_lId;}

		iSoundData *GetData(){return m_pData;}

		virtual void SetAffectedByEnv(bool a_bX){m_bAffectedByEnv = a_bX;}
		virtual void SetFiltering(bool a_bEnabled, int a_lFlags)=0;
		virtual void SetFilterGain(float a_fGain)=0;
		virtual void SetFilterGainHF(float a_fGainHF)=0;

	protected:
		iSoundData *m_pData;
		cSoundManager *m_pSoundManager;

		bool m_bLooping;
		bool m_bPaused;
		bool m_bPositionRelative;
		bool m_b3D;

		float m_fSpeed;
		float m_fVolume;
		float m_fPan;
		float m_fMaxDistance;
		float m_fMinDistance;

		cVector3f m_vVelocity;
		cVector3f m_vPosition;
		cVector3f m_vRelPosition;

		bool m_bBlockable;
		float m_fBlockVolumeMul;

		bool m_bAffectedByEnv;

		int m_lPriority;
		int m_lPriorityModifier;

		bool m_bStopUsed;

		int m_lId;

		iSoundChannelCallback *m_pCallback;
	};

	typedef std::list<iSoundChannel*> tSoundChannelList;
	typedef tSoundChannelList::iterator tSoundChannelListIt;
};
#endif