#include "impl/OpenALSoundData.h"
#include "system/LowLevelSystem.h"
#include "impl/OpenALSoundChannel.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cOpenALSoundData::cOpenALSoundData(tString a_sName, bool a_bStream) 
		: iSoundData(a_sName, a_bStream)
	{
		m_pSample = NULL;
		m_pStream = NULL;
	}

	//--------------------------------------------------------------

	cOpenALSoundData::~cOpenALSoundData()
	{
		if (m_bStream)
		{
			if (m_pStream)
				OAL_Stream_Unload(m_pStream);
		}
		else
		{
			if (m_pSample)
				OAL_Sample_Unload(m_pSample);
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cOpenALSoundData::CreateFromFile(const tString &a_sFile)
	{
		int lFlags = 0;

		unsigned int lCaps = 0;

		if (m_bStream)
		{
			m_pStream = OAL_Stream_Load(a_sFile.c_str());

			if (m_pStream == NULL)
			{
				Error("Couldn't load sound stream '%s'\n", a_sFile.c_str());
				return false;
			}
			else
				OAL_Stream_SetLoop(m_pStream, m_bLoopStream);
		}
		else
		{
			m_pSample = OAL_Sample_Load(a_sFile.c_str());

			if (m_pSample == NULL)
			{
				Error("Couldn't load sound data '%s'\n", a_sFile.c_str());
				return false;
			}
			else
				OAL_Sample_SetLoop(m_pSample, true);
		}

		return true;
	}

	//--------------------------------------------------------------

	iSoundChannel *cOpenALSoundData::CreateChannel(int a_lPriority)
	{
		if ((m_pSample == NULL) && (m_pStream == NULL)) return NULL;

		int lHandle;
		iSoundChannel *pSoundChannel = NULL;
		if (m_bStream)
		{
			lHandle = OAL_Stream_Play(OAL_FREE, GetStream(), 1.0f, true);
			if (lHandle == -1) return NULL;

			pSoundChannel = efeNew(cOpenALSoundChannel, (this, lHandle, m_pSoundManager));
			IncUserCount();
		}
		else
		{
			lHandle = OAL_Sample_Play(OAL_FREE, GetSample(), 1.0f, true, a_lPriority);
			if (lHandle == -1) return NULL;

			pSoundChannel = efeNew(cOpenALSoundChannel, (this, lHandle, m_pSoundManager));
			IncUserCount();
		}

		return pSoundChannel;
	}

	//--------------------------------------------------------------

	bool cOpenALSoundData::IsStereo()
	{
		if (m_bStream)
			return (OAL_Stream_GetChannels(m_pStream) == 2);
		if (m_pSample)
			return (OAL_Sample_GetChannels(m_pSample) == 2);

		return false;
	}
}