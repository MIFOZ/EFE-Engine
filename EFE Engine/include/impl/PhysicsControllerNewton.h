#ifndef EFE_PHYSICS_CONTROLLER_NEWTON_H
#define EFE_PHYSICS_CONTROLLER_NEWTON_H

#include "physics/PhysicsController.h"

namespace efe
{
	class iPhysicsWorld;

	class cPhysicsControllerNewton : public iPhysicsController
	{
	public:
		cPhysicsControllerNewton(const tString &a_sName, iPhysicsWorld *a_pWorld);
		~cPhysicsControllerNewton();
	};
};
#endif