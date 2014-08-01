#ifndef EFE_OPENAL_SOUND_CHANNEL_H
#define EFE_OPENAL_SOUND_CHANNEL_H

#include "sound/SoundChannel.h"
#include "sound/SoundData.h"

#include "OALWrapper/OAL_Funcs.h"

namespace efe
{
	class cOpenALSoundChannel : public iSoundChannel
	{
	public:
		cOpenALSoundChannel(iSoundData *a_pData, int a_lChannel, cSoundManager *a_pSoundManager);
		~cOpenALSoundChannel();

		void Play();
		void Stop();

		void SetPaused(bool a_bX);
		void SetSpeed(float a_fSpeed);
		void SetVolume(float a_fVolume);
		void SetLooping(bool a_bLoop);
		void SetPan(float a_fPan);
		void Set3D(bool a_b3D);

		void SetPriority(int a_lX);
		int GetPriority();

		void SetPositionRelative(bool a_bRelative);
		void SetPosition(const cVector3f &a_vPos);
		void SetVelocity(const cVector3f &a_vVel);

		void SetMinDistance(float a_fMin);
		void SetMaxDistance(float a_fMax);

		bool IsPlaying();

		bool IsBufferUnderrun() {return OAL_Source_IsBufferUnderrun(m_lChannel);}
		double GetElapsedTime() {return OAL_Source_GetElapsedTime(m_lChannel);}
		double GetTotalTime() {return OAL_Source_GetTotalTime(m_lChannel);}

		iSoundData *GetData(){return m_pData;}

		void SetAffectedByEnv(bool a_bX);
		void SetFiltering(bool a_bEnabled, int a_lFlags);
		void SetFilterGain(float a_fGain);
		void SetFilterGainHF(float a_fGainHF);

	private:
		int m_lChannel;
		int m_lDefaultFreq;

		float m_fPosition[3];
		float m_fVelocity[3];
	};
};

#endif