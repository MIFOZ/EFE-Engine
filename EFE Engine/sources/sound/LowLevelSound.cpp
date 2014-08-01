#include "sound/LowLevelSound.h"

#include "sound/SoundEnvironment.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iLowLevelSound::iLowLevelSound()
	{
		m_fVolume = 1;
		m_fEnvVolume = 0;
		m_bListenerAttenuation = true;
		m_bHardwareAcc = false;
		m_bEnvAudioEnabled = false;
	}

	//--------------------------------------------------------------

	iLowLevelSound::~iLowLevelSound()
	{
		STLDeleteAll(m_lstSoundEnv);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iSoundEnvironment *iLowLevelSound::GetSoundEnvironmentFromFileName(const tString &a_sName)
	{
		tString sLowName = cString::ToLowerCase(a_sName);
		for (tSoundEnvListIt it = m_lstSoundEnv.begin(); it != m_lstSoundEnv.end(); ++it)
		{
			iSoundEnvironment *pSoundEnv = *it;

			if (sLowName == pSoundEnv->GetFileName()) return pSoundEnv;
		}
		return NULL;
	}
}