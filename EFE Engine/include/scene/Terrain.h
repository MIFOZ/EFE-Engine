#ifndef EFE_TERRAIN_H
#define EFE_TERRAIN_H

#include "scene/MeshEntity.h"

class cMesh;
class iPhysicsBody;

namespace efe
{
	class cTerrain : public cMeshEntity
	{
	public:
		cTerrain(cMesh *a_pMesh, cMaterialManager *a_pMaterialManager,
			cMeshManager *a_pMeshManager, cAnimationManager *a_pAnimationManager);

	private:
	};
};
#endif