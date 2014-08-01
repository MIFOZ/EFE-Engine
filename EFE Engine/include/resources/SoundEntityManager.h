#ifndef EFE_SOUND_ENTITY_MANAGER_H
#define EFE_SOUND_ENTITY_MANAGER_H

#include "resources/ResourceManager.h"

namespace efe
{
	class cSound;
	class cResources;
	class cSoundEntityData;

	class cSoundEntityManager : public iResourceManager
	{
	public:
		cSoundEntityManager(cSound *a_pSound, cResources *a_pResources);
		~cSoundEntityManager();

		void Preload(const tString &a_sFile);

		iResourceBase *Create(const tString &a_sName);
		cSoundEntityData *CreateSoundEntity(const tString &a_sName);

		void Destroy(iResourceBase *a_pResource);
		void Unload(iResourceBase *a_pResource);

	private:
		cSound *m_pSound;
		cResources *m_pResources;
	};
};

#endif