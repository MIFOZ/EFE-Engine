#ifndef EFE_CHARACTER_BODY_NEWTON_H
#define EFE_CHARACTER_BODY_NEWTON_H

#include "physics/CharacterBody.h"

#include <Newton.h>

namespace efe
{
	class iPhysicsWorld;

	class cCharacterBodyNewton : public iCharacterBody
	{
	public:
		cCharacterBodyNewton(const tString &a_sName, iPhysicsWorld *a_pWorld, const cVector3f a_vSize);
		~cCharacterBodyNewton();
	};
};

#endif