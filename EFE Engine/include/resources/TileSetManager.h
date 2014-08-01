#ifndef EFE_TILESET_MANAGER_H
#define EFE_TILESET_MANAGER_H

#include "resources/ResourceManager.h"

namespace efe
{
	class cGraphics;
	class cResources;
	class cTileSet;

	class cTileSetManager : public iResourceManager
	{
	public:
		cTileSetManager(cGraphics *a_pGraphics, cResources *a_pResources);
		~cTileSetManager();

		iResourceBase *Create(const tString &a_sName);
		cTileSet *CreateTileSet(const tString &a_sName);

		void Destroy(iResourceBase *a_pResource);
		void Unload(iResourceBase *a_pResource);

	private:
		cGraphics *m_pGraphics;
		cResources *m_pResources;
	};
};
#endif