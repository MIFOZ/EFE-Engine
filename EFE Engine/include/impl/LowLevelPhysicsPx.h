#ifndef EFE_LOWLEVELPHYSICS_PX_H
#define EFE_LOWLEVELPHYSICS_PX_H

#include "physics/LowLevelPhysics.h"

#include <PxPhysicsAPI.h>

namespace efe
{
	class cLowLevelPhysicsPx : public iLowLevelPhysics
	{
	public:
		cLowLevelPhysicsPx();
		~cLowLevelPhysicsPx();

		iPhysicsWorld *CreateWorld();
	};
};
#endif