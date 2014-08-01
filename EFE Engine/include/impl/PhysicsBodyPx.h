#ifndef EFE_PHYSICS_BODY_PX_H
#define EFE_PHYSICS_BODY_PX_H

#include <PxPhysicsAPI.h>

#include "physics/PhysicsBody.h"

namespace efe
{
	using namespace physx;
	class cPhysicsBodyPxCallback : public iEntityCallback
	{
		void OnTransformUpdate(iEntity3D *a_pEntity);
	};

	class cPhysicsBodyPx : public iPhysicsBody
	{
		friend class cPhysicsBodyPxCallback;
	public:
		cPhysicsBodyPx(const tString &a_sName, iPhysicsWorld *a_pWorld, iCollideShape *a_pShape);
		~cPhysicsBodyPx();

		cVector3f GetLinearVelocity()const;

		cVector3f GetAngularVelocity()const;

		void SetMass(float a_fMass);
		float GetMass()const;

		cVector3f GetMassCentre()const;

		void AddForceAtPosition(const cVector3f &a_vForce, const cVector3f &a_vPos);

		void SetEnabled(bool a_bEnabled);

		void SetGravity(bool a_bEnabled);
		bool GetGravity()const;

		void ClearForces();

		void DeleteLowLevel();

	private:
		static void OnTransformCallback(const PxRigidBody *a_pBody, PxReal *a_pMatrix);
		static void OnUpdateCallback(const PxRigidBody *a_pBody);

		PxRigidDynamic *m_pPxBody;
		PxScene *m_pPxScene;
		cPhysicsBodyPxCallback *m_pCallback;
		
		static bool m_bUseCallback;

		bool m_bGravity;

		float m_fMaxLinearSpeed;
		float m_fMaxAngularSpeed;
		float m_fMass;

		float m_fAutoDisableLinearThreshold;
		float m_fAutoDisableAngularThreshold;
		int m_lAutoDisableNumSteps;

		cVector3f m_vTotalForce;
		cVector3f m_vTotalTorque;
	};
};
#endif