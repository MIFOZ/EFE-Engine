#ifndef EFE_LOWLEVELSOUND_OPENAL_H
#define EFE_LOWLEVELSOUND_OPENAL_H

#include "sound/LowLevelSound.h"

class cOAL_Effect_Reverb;

namespace efe
{
	class cLowLevelSoundOpenAL : public iLowLevelSound
	{
	public:
		cLowLevelSoundOpenAL();
		~cLowLevelSoundOpenAL();

		void GetSupportedFormats(tStringList &a_lstFormats);

		iSoundData *LoadSoundData(const tString &a_sName, const tString &a_sFilePath,
			const tString &a_sType, bool a_bStream, bool a_bLoopStream);

		void UpdateSound(float a_fTimeStep);

		void SetListenerAttributes(const cVector3f &a_vPos, const cVector3f &a_vVel, 
			const cVector3f &a_vFoward, const cVector3f &a_vUp);

		void SetListenerPosition(const cVector3f &a_vPos);

		void SetListenerAttenuation(bool a_bEnabled);

		void SetRollOffFactor(float a_fFactor);

		void SetVolume(float a_fVolume);

		void Init(bool a_bUseHardware, bool a_bForceGeneric, bool a_bUseEnvAudio, int a_lMaxChannels,
			int a_lStreamUpdateFreq, bool a_bUseThreading, bool a_bUseVoiceManagement,
			int a_lMaxMonoSourceHint, int a_lMaxStereoSourceHint, 
			int a_lStreamingBufferSize, int a_lStreamingBufferCount, bool a_bEnableLowLevelLog, tString a_sDeviceName);

		void SetEnvVolume(float a_fEnvVolume);

		iSoundEnvironment *LoadSoundEnvironment(const tString &a_sFilePath);

		void SetSoundEnvironment(iSoundEnvironment *a_pSoundEnv);
		void FadeSoundEnvironment(iSoundEnvironment *a_pSourceSoundEnv, iSoundEnvironment *a_pDestSoundEnv, float a_fT);

	private:
		tString m_vFormats[30];
		bool m_bLogSounds;
		bool m_bInitialized;
		int m_lEffectSlotId;
		bool m_bNullEffectAttached;

		cOAL_Effect_Reverb *m_pEffect;
	};
};
#endif