#ifndef EFE_CHARACTER_BODY_PX_H
#define EFE_CHARACTER_BODY_PX_H

#include "physics/CharacterBody.h"

#include <PxPhysicsAPI.h>

namespace efe
{
	class iPhysicsWorld;

	class cCharacterBodyPx : public iCharacterBody
	{
	public:
		cCharacterBodyPx(const tString &a_sName, iPhysicsWorld *a_pWorld, const cVector3f &a_vSize);
		~cCharacterBodyPx();
	};
};

#endif