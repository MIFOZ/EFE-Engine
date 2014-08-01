#include "scene/Terrain.h"

namespace efe
{
	cTerrain::cTerrain(cMesh *a_pMesh, cMaterialManager *a_pMaterialManager,
			cMeshManager *a_pMeshManager, cAnimationManager *a_pAnimationManager)
		: cMeshEntity("Terrain", a_pMesh, a_pMaterialManager, a_pMeshManager ,a_pAnimationManager)
	{
	}
}