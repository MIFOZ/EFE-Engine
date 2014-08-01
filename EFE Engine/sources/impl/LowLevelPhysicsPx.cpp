#pragma comment(lib, "FoundationDEBUG.lib")
#pragma comment(lib, "PhysX3CommonDEBUG.lib")
#pragma comment(lib, "PhysX3_x86.lib")
#pragma comment(lib, "PhysX3CookingCHECKED_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")

#include "impl/LowLevelPhysicsPx.h"

#include "impl/PhysicsWorldPx.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	cLowLevelPhysicsPx::cLowLevelPhysicsPx()
	{
		
	}

	cLowLevelPhysicsPx::~cLowLevelPhysicsPx()
	{
		
	}

	iPhysicsWorld *cLowLevelPhysicsPx::CreateWorld()
	{
		cPhysicsWorldPx *pWorld = efeNew(cPhysicsWorldPx, ());
		return pWorld;
	}
}