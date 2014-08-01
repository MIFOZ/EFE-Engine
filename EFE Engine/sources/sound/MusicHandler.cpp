#include "sound/MusicHandler.h"
#include "resources/Resources.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "math/Math.h"
#include "sound/LowLevelSound.h"
#include "sound/SoundChannel.h"
#include "sound/SoundData.h"
#include "resources/SoundManager.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMusicHandler::cMusicHandler(iLowLevelSound *a_pLowLevelSound, cResources *a_pResources)
	{
		m_pLowLevelSound = a_pLowLevelSound;
		m_pResources = a_pResources;

		m_pMainSong = NULL;
		m_pLock = NULL;
		m_bIsPaused = false;
	}

	//--------------------------------------------------------------

	cMusicHandler::~cMusicHandler()
	{
		if (m_pMainSong)
		{
			efeDelete(m_pMainSong->m_pStream);
			efeDelete(m_pMainSong);
		}

		tMusicEntryListIt it = m_lstFadingSongs.begin();
		while (it != m_lstFadingSongs.end())
		{
			cMusicEntry *pSong = *it;
			efeDelete(pSong->m_pStream);
			efeDelete(pSong);

			it = m_lstFadingSongs.erase(it);
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cMusicHandler::Play(const tString &a_sFileName, float a_fVolume, float a_fFadeStepSize, bool a_bLoop)
	{
		bool bSongIsPlaying = false;

		if (m_pLock != NULL)
		{
			m_pLock->m_sFileName = a_sFileName;
			m_pLock->m_fVolume = a_fVolume;
			m_pLock->m_bLoop = a_bLoop;
			return true;
		}

		if (m_pMainSong != NULL)
			if (a_sFileName == m_pMainSong->m_sFileName)
				bSongIsPlaying = true;

		if (!bSongIsPlaying)
		{
			if (m_pMainSong != NULL)
			{
				m_pMainSong->m_fVolumeAdd = a_fFadeStepSize;
				m_lstFadingSongs.push_back(m_pMainSong);
			}

			tMusicEntryListIt it = m_lstFadingSongs.begin();
			while (it != m_lstFadingSongs.end())
			{
				cMusicEntry *pSong = *it;
				if (pSong->m_sFileName == a_sFileName)
				{
					pSong->m_fVolume = 0;
					pSong->m_pStream->Stop();
					efeDelete(pSong->m_pStream);
					efeDelete(pSong);

					it = m_lstFadingSongs.erase(it);
				}
				else
					it++;
			}

			m_pMainSong = efeNew(cMusicEntry, ());

			if (LoadAndStart(a_sFileName, m_pMainSong, 0, a_bLoop) == false)
			{
				efeDelete(m_pMainSong);
				m_pMainSong = NULL;
				return false;
			}
		}
		else
		{
			if (m_pMainSong->m_fMaxVolume == a_fVolume)
				return true;
		}

		m_pMainSong->m_fMaxVolume = a_fVolume;
		m_pMainSong->m_bLoop = a_bLoop;

		if (m_pMainSong->m_fMaxVolume > m_pMainSong->m_fVolume)
			m_pMainSong->m_fVolumeAdd = a_fFadeStepSize;
		else
			m_pMainSong->m_fVolumeAdd = -a_fFadeStepSize;

		return true;
	}

	//--------------------------------------------------------------

	void cMusicHandler::Stop(float a_fFadeStepSize)
	{
		if (m_pMainSong == NULL) return;

		if (a_fFadeStepSize < 0) a_fFadeStepSize = -a_fFadeStepSize;

		m_pMainSong->m_fVolumeAdd = a_fFadeStepSize;
		if (a_fFadeStepSize == 0)
		{
			m_pMainSong->m_pStream->SetVolume(0);
			m_pMainSong->m_pStream->Stop();
			m_pMainSong->m_fVolume = 0;
		}

		m_lstFadingSongs.push_back(m_pMainSong);
		m_pMainSong = NULL;
	}

	//--------------------------------------------------------------

	void cMusicHandler::Pause()
	{
		if (m_pMainSong != NULL)
			m_pMainSong->m_pStream->SetPaused(true);

		tMusicEntryListIt it = m_lstFadingSongs.begin();
		while (it != m_lstFadingSongs.end())
		{
			(*it)->m_pStream->SetPaused(true);
			it++;
		}

		m_bIsPaused = true;
	}

	//--------------------------------------------------------------

	void cMusicHandler::Resume()
	{
		if (m_pMainSong != NULL)
			m_pMainSong->m_pStream->SetPaused(false);

		tMusicEntryListIt it = m_lstFadingSongs.begin();
		while (it != m_lstFadingSongs.end())
		{
			(*it)->m_pStream->SetPaused(false);
			it++;
		}

		m_bIsPaused = false;
	}

	//--------------------------------------------------------------

	void cMusicHandler::Lock(cMusicLock *a_pLock)
	{
		m_pLock = a_pLock;
	}

	//--------------------------------------------------------------

	void cMusicHandler::Unlock()
	{
		m_pLock = NULL;
	}

	//--------------------------------------------------------------

	tString cMusicHandler::GetCurrentSongName()
	{
		if (m_pMainSong != NULL)
			return m_pMainSong->m_sFileName;
		else
			return "";
	}

	//--------------------------------------------------------------

	float cMusicHandler::GetCurrentSongVolume()
	{
		if (m_pMainSong != NULL)
			return m_pMainSong->m_fVolume;
		else
			return 0;
	}

	//--------------------------------------------------------------

	cMusicEntry *cMusicHandler::GetCurrentSong()
	{
		return m_pMainSong;
	}

	//--------------------------------------------------------------

	void cMusicHandler::Update(float a_fTimeStep)
	{
		if (m_bIsPaused) return;

		if (m_pMainSong != NULL)
		{
			if (m_pMainSong->m_pStream->IsPlaying() == false)
			{
				efeDelete(m_pMainSong->m_pStream);
				efeDelete(m_pMainSong);
				m_pMainSong = NULL;
			}
			else
			{
				m_pMainSong->m_fVolume += m_pMainSong->m_fVolumeAdd * a_fTimeStep;

				if (m_pMainSong->m_fVolumeAdd > 0)
				{
					if (m_pMainSong->m_fVolume >= m_pMainSong->m_fMaxVolume)
						m_pMainSong->m_fVolume = m_pMainSong->m_fMaxVolume;
				}
				else
				{
					if (m_pMainSong->m_fVolume <= m_pMainSong->m_fMaxVolume)
						m_pMainSong->m_fVolume = m_pMainSong->m_fMaxVolume;
				}

				if (m_pMainSong->m_pStream->GetVolume() != m_pMainSong->m_fVolume)
					m_pMainSong->m_pStream->SetVolume(m_pMainSong->m_fVolume);
			}
		}

		tMusicEntryListIt it = m_lstFadingSongs.begin();
		while (it != m_lstFadingSongs.end())
		{
			cMusicEntry *pSong = *it;
			pSong->m_fVolume -= pSong->m_fVolumeAdd * a_fTimeStep;

			if (pSong->m_fVolume <= 0)
			{
				pSong->m_fVolume = 0;
				pSong->m_pStream->Stop();
				efeDelete(pSong->m_pStream);
				efeDelete(pSong);

				it = m_lstFadingSongs.erase(it);
			}
			else
			{
				pSong->m_pStream->SetVolume(pSong->m_fVolume);
				it++;
			}
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cMusicHandler::LoadAndStart(const tString &a_sFileName, cMusicEntry *a_pSong, float a_fVolume, bool a_bLoop)
	{
		iSoundData *pData = m_pResources->GetSoundManager()->CreateSoundData(a_sFileName, true, a_bLoop);
		if (pData == NULL)
		{
			Error("Couldn't load music '%s'\n", a_sFileName.c_str());
			return false;
		}

		iSoundChannel *pStream = pData->CreateChannel(256);
		if (pStream == NULL)
		{
			Error("Couldn't stream music '%s'\n", a_sFileName.c_str());
			return false;
		}

		a_pSong->m_sFileName = a_sFileName;
		a_pSong->m_pStream = pStream;
		a_pSong->m_pStream->SetVolume(a_fVolume);

		a_pSong->m_pStream->Play();

		return true;
	}
}