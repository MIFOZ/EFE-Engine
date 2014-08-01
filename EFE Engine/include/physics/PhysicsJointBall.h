#ifndef EFE_PHYSICS_JOINT_BALL_H
#define EFE_PHYSICS_JOINT_BALL_H

#include "physics/PhysicsJoint.h"

namespace efe
{
	kSaveData_ChildClass(iPhysicsJoint, iPhysicsJointBall)
	{
		kSaveData_ClassInit(iPhysicsJointBall)
	public:
		float m_fMaxConeAngle;
		float m_fMaxTwistAngle;
		cVector3f m_vConePin;

		virtual iSaveObject *CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);
		virtual int GetSaveCreatePrio();
	};

	class iPhysicsJointBall : public iPhysicsJoint
	{
	public:
		iPhysicsJointBall(const tString &a_sName,
			iPhysicsBody *a_pParentBody, iPhysicsBody *a_pChildBody, 
			iPhysicsWorld *a_pWorld, const cVector3f &a_vPivotPoint)
			: iPhysicsJoint(a_sName, a_pParentBody, a_pChildBody, a_pWorld, a_vPivotPoint){}
		virtual ~iPhysicsJointBall(){}

		virtual void SetConeLimits(const cVector3f &a_vPin, float a_fMaxConeAngle, float a_fMaxTwistAngle) = 0;
		virtual cVector3f GetAngles() = 0;

		float GetMaxConeAngle() {return m_fMaxConeAngle;}
		float GetMaxTwistAngle() {return m_fMaxTwistAngle;}
		cVector3f GetConePin() {return m_vConePin;}

		ePhysicsJointType GetType() {return ePhysicsJointType_Ball;}

		//SaveObject implementation
		virtual iSaveData *CreateSaveData();
		virtual void SaveToSaveData(iSaveData *a_pSaveData);
		virtual void LoadFromSaveData(iSaveData *a_pSaveData);
		virtual void SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);

	protected:
		float m_fMaxConeAngle;
		float m_fMaxTwistAngle;
		cVector3f m_vConePin;
	};
};
#endif