#ifndef EFE_LOWLEVELPHYSICS_H
#define EFE_LOWLEVELPHYSICS_H

#include "system/SystemTypes.h"

namespace efe
{
	class iPhysicsWorld;

	class iLowLevelPhysics
	{
	public:
		virtual ~iLowLevelPhysics(){}

		virtual iPhysicsWorld *CreateWorld()=0;
	};
};
#endif