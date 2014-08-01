#ifndef EFE_MUSICHANDLER_H
#define EFE_MUSICHANDLER_H

#include <list>
#include "system/SystemTypes.h"

namespace efe
{
	class iLowLevelSound;
	class iSoundChannel;

	class cMusicLock
	{
	public:
		cMusicLock() : m_sFileName(""), m_fVolume(0) {}

		tString m_sFileName;
		float m_fVolume;
		bool m_bLoop;
	};

	class cMusicEntry
	{
	public:
		cMusicEntry() : m_sFileName(""), m_pStream(NULL), m_fMaxVolume(1),
			m_fVolume(0), m_fVolumeAdd(0.01f) {}

		tString m_sFileName;
		iSoundChannel *m_pStream;
		float m_fMaxVolume;
		float m_fVolume;
		float m_fVolumeAdd;
		bool m_bLoop;
	};

	typedef std::list<cMusicEntry*> tMusicEntryList;
	typedef tMusicEntryList::iterator tMusicEntryListIt;

	class cResources;

	class cMusicHandler
	{
	public:
		cMusicHandler(iLowLevelSound *a_pLowLevelSound, cResources *a_pResources);
		~cMusicHandler();

		bool Play(const tString &a_sFileName, float a_fVolume, float a_fFadeStepSize, bool a_bLoop);

		void Stop(float a_fFadeStepSize);
		void Pause();
		void Resume();

		void Lock(cMusicLock *a_pLock);

		void Unlock();
		tString GetCurrentSongName();
		float GetCurrentSongVolume();

		cMusicEntry *GetCurrentSong();

		void Update(float a_fTimeStep);

	private:
		iLowLevelSound *m_pLowLevelSound;
		cResources *m_pResources;

		tMusicEntryList m_lstFadingSongs;
		cMusicEntry *m_pMainSong;
		cMusicLock *m_pLock;
		bool m_bIsPaused;

		bool LoadAndStart(const tString &a_sFileName, cMusicEntry *a_pSong, float a_fVolume, bool a_bLoop);
	};
};

#endif