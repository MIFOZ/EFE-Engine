#include "impl/CharacterBodyPx.h"

#include "physics/CollideShape.h"
#include "physics/PhysicsWorld.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cCharacterBodyPx::cCharacterBodyPx(const tString &a_sName, iPhysicsWorld *a_pWorld, const cVector3f &a_vSize)
		: iCharacterBody(a_sName, a_pWorld, a_vSize)
	{
	}

	//--------------------------------------------------------------

	cCharacterBodyPx::~cCharacterBodyPx()
	{
	}
}