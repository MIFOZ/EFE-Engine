#ifndef EFE_SOUND_MANAGER_H
#define EFE_SOUND_MANAGER_H

#include "resources/ResourceManager.h"

namespace efe
{
	class cSound;
	class cResources;
	class iSoundData;

	class cSoundManager : public iResourceManager
	{
	public:
		cSoundManager(cSound *a_pSound, cResources *a_pResources);
		~cSoundManager();

		iResourceBase *Create(const tString &a_sName);
		iSoundData *CreateSoundData(const tString &a_sName, bool a_bStream, bool a_bLoopStream = false);

		void Destroy(iResourceBase *a_pResource);
		void Unload(iResourceBase *a_pResource);

		void DestroyAll();
	private:
		cSound *m_pSound;
		cResources *m_pResources;

		tStringList m_lstFileFormats;

		iSoundData *FindData(const tString &a_sName, tString &a_sFilePath);
	};
};
#endif