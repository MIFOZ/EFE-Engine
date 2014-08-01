#ifndef EFE_LOWLEVELPHYSICS_NEWTON_H
#define  EFE_LOWLEVELPHYSICS_NEWTON_H

#include "physics/LowLevelPhysics.h"

#include <Newton.h>

namespace efe
{
	class cLowLevelPhysicsNewton : public iLowLevelPhysics
	{
	public:
		cLowLevelPhysicsNewton();
		~cLowLevelPhysicsNewton();

		iPhysicsWorld *CreateWorld();
	};
};

#endif