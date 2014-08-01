#include "sound/Sound.h"
#include "system/LowLevelSystem.h"
#include "sound/LowLevelSound.h"
#include "resources/Resources.h"
#include "sound/SoundHandler.h"
#include "sound/MusicHandler.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSound::cSound(iLowLevelSound *a_pLowLevelSound) : iUpdateable("EFE_Sound")
	{
		m_pLowLevelSound = a_pLowLevelSound;
	}

	cSound::~cSound()
	{
		Log("Exiting Sound Module\n");
		Log("-----------------------------------------------------\n");

		efeDelete(m_pSoundHandler);
		efeDelete(m_pMusicHandler);

		Log("-----------------------------------------------------\n\n");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cSound::Init(cResources *a_pResources, bool a_bUseHardware, bool a_bForceGeneric, bool a_bUseEnvAudio, int a_lMaxChannels,
			int a_lStreamUpdateFreq, bool a_bUseThreading, bool a_bUseVoiceManagement,
			int a_lMaxMonoSourceHint, int a_lMaxStereoSourceHint, 
			int a_lStreamingBufferSize, int a_lStreamingBufferCount, bool a_bEnableLogLevelLog, tString a_sDeviceName)
	{
		m_pResources = a_pResources;

		Log("Initializing Sound Module\n");
		Log("-----------------------------------------------------\n");

		m_pLowLevelSound->Init(a_bUseHardware,a_bForceGeneric,a_bUseEnvAudio, a_lMaxChannels,
			a_lStreamUpdateFreq, a_bUseThreading, a_bUseVoiceManagement, a_lMaxMonoSourceHint,
			a_lMaxStereoSourceHint, a_lStreamingBufferSize, a_lStreamingBufferCount, a_bEnableLogLevelLog, a_sDeviceName);

		m_pSoundHandler = efeNew(cSoundHandler, (m_pLowLevelSound, m_pResources));
		m_pMusicHandler = efeNew(cMusicHandler, (m_pLowLevelSound, m_pResources));

		Log("-----------------------------------------------------\n\n");
	}

	//--------------------------------------------------------------

	void cSound::Update(float a_fTimeStep)
	{
		m_pSoundHandler->Update(a_fTimeStep);
		m_pMusicHandler->Update(a_fTimeStep);

		m_pLowLevelSound->UpdateSound(a_fTimeStep);
	}
}