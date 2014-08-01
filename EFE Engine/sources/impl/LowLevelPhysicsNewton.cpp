#pragma comment(lib, "newton_d.lib")

#include "impl/LowLevelPhysicsNewton.h"

#include "impl/PhysicsWorldNewton.h"


namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cLowLevelPhysicsNewton::cLowLevelPhysicsNewton()
	{

	}

	//--------------------------------------------------------------

	cLowLevelPhysicsNewton::~cLowLevelPhysicsNewton()
	{

	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iPhysicsWorld *cLowLevelPhysicsNewton::CreateWorld()
	{
		cPhysicsWorldNewton *pWorld = efeNew(cPhysicsWorldNewton, ());
		return pWorld;
	}
}