#ifndef EFE_SOUND_H
#define EFE_SOUND_H

#include "game/Updateable.h"

namespace efe
{
	class iLowLevelSound;
	class cResources;
	class cSoundHandler;
	class cMusicHandler;

	class cSound : public iUpdateable
	{
	public:
		cSound(iLowLevelSound *a_pLowLevelSound);
		~cSound();

		void Init(cResources *a_pResources, bool a_bUseHardware, bool a_bForceGeneric, bool a_bUseEnvAudio, int a_lMaxChannels,
			int a_lStreamUpdateFreq, bool a_bUseThreading, bool a_bUseVoiceManagement,
			int a_lMaxMonoSourceHint, int a_lMaxStereoSourceHint, 
			int a_lStreamingBufferSize, int a_lStreamingBufferCount, bool a_bEnableLogLevelLog, tString a_sDeviceName);

		void Update(float a_fTimeStep);

		iLowLevelSound *GetLowLevel(){return m_pLowLevelSound;}
		cSoundHandler *GetSoundHandler(){return m_pSoundHandler;}
		cMusicHandler *GetMusicHandler(){return m_pMusicHandler;}
	private:
		iLowLevelSound *m_pLowLevelSound;
		cResources *m_pResources;
		cSoundHandler *m_pSoundHandler;
		cMusicHandler *m_pMusicHandler;
	};
};
#endif