#include "impl/CharacterBodyNewton.h"

#include "physics/CollideShape.h"
#include "physics/PhysicsWorld.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cCharacterBodyNewton::cCharacterBodyNewton(const tString &a_sName, iPhysicsWorld *a_pWorld, const cVector3f a_vSize)
		: iCharacterBody(a_sName, a_pWorld, a_vSize)
	{
	}

	//--------------------------------------------------------------

	cCharacterBodyNewton::~cCharacterBodyNewton()
	{
	}
}