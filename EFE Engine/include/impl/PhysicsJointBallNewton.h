#ifndef EFE_PHYSICS_JOINT_BALL_NEWTON_H
#define EFE_PHYSICS_JOINT_BALL_NEWTON_H

#include "physics/PhysicsJointBall.h"
#include "impl/PhysicsJointNewton.h"

namespace efe
{
	class cPhysicsJointBallNewton: public iPhysicsJointNewton<iPhysicsJointBall>
	{
	public:
		cPhysicsJointBallNewton(const tString &a_sName,
			iPhysicsBody *a_pParentBody, iPhysicsBody *a_pChildBody, 
			iPhysicsWorld *a_pWorld, const cVector3f &a_vPivotPoint);
		~cPhysicsJointBallNewton();

		void SetConeLimits(const cVector3f &a_vPin, float a_fMaxConeAngle, float a_fMaxTwistAngle);
		cVector3f GetAngles();

		cVector3f GetVelocity();
		cVector3f GetAngularVelocity();
		cVector3f GetForce();

		float GetDistance();
		float GetAngle();
	};
};
#endif