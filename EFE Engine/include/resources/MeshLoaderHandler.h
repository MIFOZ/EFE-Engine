#ifndef EFE_MESH_LOADER_HANDLER_H
#define EFE_MESH_LOADER_HANDLER_H

#include <list>

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"

#include "resources/MeshLoader.h"

namespace efe
{
	class cMesh;
	class iMeshLoader;
	class cResources;
	class cWorld3D;
	class cScene;
	class cAnimation;

	typedef std::list<iMeshLoader*> tMeshLoaderList;
	typedef tMeshLoaderList::iterator tMeshLoaderListIt;

	class cMeshLoaderHandler
	{
	public:
		cMeshLoaderHandler(cResources *a_pResources, cScene *a_pScene);
		~cMeshLoaderHandler();

		cMesh *LoadMesh(const tString &a_sFile, tMeshLoadFlag a_Flags);
		bool SaveMesh(cMesh *a_pMesh, const tString &a_sFile);

		cWorld3D *LoadWorld(const tString &a_sFile, tWorldLoadFlag a_Flags);

		void AddLoader(iMeshLoader *a_pLoader);

		tStringVec *GetSupportedTypes(){return &m_vSupportedTypes;}
	private:
		tStringVec m_vSupportedTypes;

		tMeshLoaderList m_lstLoaders;

		cResources *m_pResources;
		cScene *m_pScene;
	};
};

#endif