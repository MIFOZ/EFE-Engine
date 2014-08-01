#ifndef EFE_PHYSICS_BODY_NEWTON_H
#define EFE_PHYSICS_BODY_NEWTON_H

#include <Newton.h>

#include "physics/PhysicsBody.h"

namespace efe
{
	class cPhysicsBodyNewtonCallback : public iEntityCallback
	{
		void OnTransformUpdate(iEntity3D *a_pEntity);
	};

	class cPhysicsBodyNewton : public iPhysicsBody
	{
		friend class cPhysicsBodyNewtonCallback;
	public:
		cPhysicsBodyNewton(const tString &a_sName, iPhysicsWorld *a_pWorld, iCollideShape *a_pShape);
		virtual ~cPhysicsBodyNewton();

		void SetMaterial(iPhysicsMaterial *a_pMaterial);

		void SetLinearVelocity(const cVector3f &a_vVel);
		cVector3f GetLinearVelocity() const;
		void SetAngularVelocity(const cVector3f &a_vVel);
		cVector3f GetAngularVelocity() const;
		void SetLinearDamping(float a_fDamping);
		float GetLinearDamping() const;
		void SetAngularDamping(float a_fDamping);
		float GetAngularDamping() const;
		void SetMaxLinearSpeed(float a_fSpeed);
		float GetMaxLinearSpeed() const;
		void SetMaxAngularSpeed(float a_fSpeed);
		float GetMaxAngularSpeed() const;
		cMatrixf GetInertiaMatrix();

		void SetMass(float a_fMass);
		float GetMass() const;
		void SetMassCentre(const cVector3f &a_vCentre);
		cVector3f GetMassCentre() const;

		void AddForce(const cVector3f &a_vForce);
		void AddForceAtPosition(const cVector3f &a_vForce, const cVector3f &a_vPos);
		void AddTorque(const cVector3f &a_vTorque);
		void AddImpulse(const cVector3f &a_vImpulse);
		void AddImpulseAtPosition(const cVector3f &a_vImpulse, const cVector3f &a_vPos);

		void SetEnabled(bool a_bEnabled);
		bool GetEnabled() const;
		void SetAutoDisable(bool a_bEnabled);
		bool GetAutoDisable() const;
		void SetContinuousCollision(bool a_bOn);
		bool GetContinuousCollision();

		void SetGravity(bool a_bEnabled);
		bool GetGravity() const;

		void RenderDebugGeometry(iLowLevelGraphics *a_pLowLevel, const cColor &a_Color);

		NewtonBody *GetNewtonBody() {return m_pNewtonBody;}

		void ClearForces();

		void DeleteLowLevel();

		static void SetUseCallback(bool a_bX) {m_bUseCallback = a_bX;}

	private:
		static void OnTransformCallback(const NewtonBody *a_pBody, const dFloat *a_pMatrix, int a_lThreadIndex);
		static void OnUpdateCallback(const NewtonBody *a_pBody, dFloat a_fTimeStep, int a_lThreadIndex);

		NewtonBody *m_pNewtonBody;
		const NewtonWorld *m_pNewtonWorld;

		cPhysicsBodyNewtonCallback *m_pCallback;

		static bool m_bUseCallback;

		//Properties
		bool m_bGravity;

		float m_fMaxLinearSpeed;
		float m_fMaxAngularSpeed;
		float m_FMass;

		float m_fAutoDisableLinearThreshold;
		float m_fAutoDisableAngularThreshold;
		int m_lAutoDisableNumSteps;

		cVector3f m_vTotalForce;
		cVector3f m_vTotalTorque;
	};
};

#endif