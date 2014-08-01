#include "impl/PhysicsJointBallNewton.h"

#include "impl/PhysicsBodyNewton.h"
#include "impl/PhysicsWorldNewton.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPhysicsJointBallNewton::cPhysicsJointBallNewton(const tString &a_sName,
		iPhysicsBody *a_pParentBody, iPhysicsBody *a_pChildBody, 
		iPhysicsWorld *a_pWorld, const cVector3f &a_vPivotPoint)
		: iPhysicsJointNewton<iPhysicsJointBall>(a_sName, a_pParentBody, a_pChildBody, a_pWorld, a_vPivotPoint)
	{
		m_pNewtonJoint = NewtonConstraintCreateBall(m_pNewtonWorld, a_vPivotPoint.v,
			m_pNewtonChildBody, m_pNewtonParentBody);

		m_vPinDir = cVector3f(0,0,0);
		m_vPivotPoint = a_vPivotPoint;

		m_fMaxConeAngle = 0;
		m_fMaxTwistAngle = 0;
		m_vConePin = m_vPinDir;
	}

	//--------------------------------------------------------------

	cPhysicsJointBallNewton::~cPhysicsJointBallNewton()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cPhysicsJointBallNewton::SetConeLimits(const cVector3f &a_vPin, float a_fMaxConeAngle, float a_fMaxTwistAngle)
	{
		NewtonBallSetConeLimits(m_pNewtonJoint, a_vPin.v, a_fMaxConeAngle, a_fMaxTwistAngle);
		m_vConePin = a_vPin;
		m_vPinDir = m_vConePin;
		m_fMaxConeAngle = a_fMaxConeAngle;
		m_fMaxTwistAngle = a_fMaxTwistAngle;
	}

	cVector3f cPhysicsJointBallNewton::GetAngles()
	{
		cVector3f vAngles;
		NewtonBallGetJointAngle(m_pNewtonJoint, &vAngles.v[0]);
		return vAngles;
	}

	//--------------------------------------------------------------

	cVector3f cPhysicsJointBallNewton::GetVelocity()
	{
		return cVector3f(0,0,0);
	}
	cVector3f cPhysicsJointBallNewton::GetAngularVelocity()
	{
		cVector3f vVel;
		NewtonBallGetJointOmega(m_pNewtonJoint, &vVel.v[0]);
		return vVel;
	}
	cVector3f cPhysicsJointBallNewton::GetForce()
	{
		cVector3f vForce;
		NewtonBallGetJointForce(m_pNewtonJoint, &vForce.v[0]);
		return vForce;
	}

	//--------------------------------------------------------------

	float cPhysicsJointBallNewton::GetDistance()
	{
		return 0;
	}
	float cPhysicsJointBallNewton::GetAngle()
	{
		return 0;
	}
}