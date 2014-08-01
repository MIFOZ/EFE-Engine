#ifndef EFE_SOUND_DATA_H
#define EFE_SOUND_DATA_H

#include "system/SystemTypes.h"
#include "resources/ResourceBase.h"

namespace efe
{
	class cSoundManager;
	class iSoundChannel;

	class iSoundData : public iResourceBase
	{
	public:

		iSoundData(tString a_sName, bool a_bStream) : iResourceBase(a_sName, 0),
			m_pSoundManager(NULL), m_bStream(a_bStream){}

		virtual ~iSoundData(){}

		virtual bool CreateFromFile(const tString &a_sFile)=0;

		virtual iSoundChannel *CreateChannel(int a_lPriority)=0;

		virtual bool IsStereo()=0;

		bool IsStream(){return m_bStream;}
		void SetLoopStream(bool a_bX){m_bLoopStream = a_bX;}
		bool GetLoopStream(){return m_bLoopStream;}

		bool Reload(){return false;}
		void Unload(){}
		void Destroy(){}

		void SetSoundManager(cSoundManager *a_pSoundManager)
		{
			m_pSoundManager = a_pSoundManager;
		}

	protected:
		bool m_bStream;
		bool m_bLoopStream;
		cSoundManager *m_pSoundManager;
	};
};
#endif