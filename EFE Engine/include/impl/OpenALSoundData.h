#ifndef EFE_OPENAL_SOUND_DATA_H
#define EFE_OPENAL_SOUND_DATA_H

#include "sound/SoundData.h"

#include "OALWrapper/OAL_Funcs.h"

namespace efe
{
	class cOpenALSoundData : public iSoundData
	{
	public:
		cOpenALSoundData(tString a_sName, bool a_bStream);
		~cOpenALSoundData();

		bool CreateFromFile(const tString &a_sFile);

		iSoundChannel *CreateChannel(int a_lPriority);

		bool IsStream(){return m_bStream;}

		bool IsStereo();

		cOAL_Sample *GetSample(){return m_pSample;}
		cOAL_Stream *GetStream(){return m_pStream;}

	private:
		cOAL_Sample *m_pSample;
		cOAL_Stream *m_pStream;
	};
};

#endif