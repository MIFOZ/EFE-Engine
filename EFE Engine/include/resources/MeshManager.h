#ifndef EFE_MESH_MANAGER_H
#define EFE_MESH_MANAGER_H

#include "resources/ResourceManager.h"

namespace efe
{
	class cGraphics;
	class cResources;
	class cMesh;

	class cMeshManager : public iResourceManager
	{
	public:
		cMeshManager(cGraphics *a_pGraphics, cResources *a_pResources);
		~cMeshManager();

		iResourceBase *Create(const tString &a_sName);
		cMesh *CreateMesh(const tString &a_sName);

		void Destroy(iResourceBase *a_pResource);
		void Unload(iResourceBase *a_pResource);
	private:
		cGraphics *m_pGraphics;
		cResources *m_pResources;
	};
};

#endif