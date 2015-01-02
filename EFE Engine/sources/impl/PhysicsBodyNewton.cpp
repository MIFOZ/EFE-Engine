#include "impl/PhysicsBodyNewton.h"

#include "impl/CollideShapeNewton.h"
#include "impl/PhysicsWorldNewton.h"
#include "impl/PhysicsMaterialNewton.h"
#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "math/Math.h"
#include "scene/Node3D.h"
#include "scene/World3D.h"

namespace efe
{
	bool cPhysicsBodyNewton::m_bUseCallback = true;

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	cPhysicsBodyNewton::cPhysicsBodyNewton(const tString &a_sName, iPhysicsWorld *a_pWorld, iCollideShape *a_pShape)
		: iPhysicsBody(a_sName, a_pWorld, a_pShape)
	{
		cPhysicsWorldNewton *pWorldNewton = static_cast<cPhysicsWorldNewton*>(a_pWorld);
		cCollideShapeNewton *pShapeNewton = static_cast<cCollideShapeNewton*>(a_pShape);

		m_pNewtonWorld = pWorldNewton->GetNewtonWorld();
		m_pNewtonBody = NewtonCreateDynamicBody(pWorldNewton->GetNewtonWorld(),
			pShapeNewton->GetNewtonCollision(), cMatrixf::Identity.v);

		m_pCallback = efeNew(cPhysicsBodyNewtonCallback, ());
		
		AddCallback(m_pCallback);

		NewtonBodySetForceAndTorqueCallback(m_pNewtonBody, OnUpdateCallback);
		NewtonBodySetTransformCallback(m_pNewtonBody, OnTransformCallback);
		NewtonBodySetUserData(m_pNewtonBody, this);

		m_fAutoDisableLinearThreshold = 0.01f;
		m_fAutoDisableAngularThreshold = 0.01f;
		m_lAutoDisableNumSteps = 10;
	}

	//-------------------------------------------------------------

	cPhysicsBodyNewton::~cPhysicsBodyNewton()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// CALLBACK METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cPhysicsBodyNewtonCallback::OnTransformUpdate(iEntity3D *a_pEntity)
	{
		if (cPhysicsBodyNewton::m_bUseCallback == false) return;

		cPhysicsBodyNewton *pRigidBody = static_cast<cPhysicsBodyNewton*> (a_pEntity);
		NewtonBodySetMatrix(pRigidBody->m_pNewtonBody,
			&a_pEntity->GetLocalMatrix().GetTranspose().m[0][0]);

		if (pRigidBody->m_pNode) pRigidBody->m_pNode->SetMatrix(a_pEntity->GetLocalMatrix());
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::SetMaterial(iPhysicsMaterial *a_pMaterial)
	{
		m_pMaterial = a_pMaterial;

		if (a_pMaterial == NULL) return;

		cPhysicsMaterialNewton *pNewtonMat = static_cast<cPhysicsMaterialNewton*>(m_pMaterial);

		NewtonBodySetMaterialGroupID(m_pNewtonBody, pNewtonMat->GetId());
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::SetLinearVelocity(const cVector3f &a_vVel)
	{
		NewtonBodySetVelocity(m_pNewtonBody, a_vVel.v);
	}
	cVector3f cPhysicsBodyNewton::GetLinearVelocity() const
	{
		cVector3f vVel;
		NewtonBodyGetVelocity(m_pNewtonBody, vVel.v);
		return vVel;
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::SetAngularVelocity(const cVector3f &a_vVel)
	{
		NewtonBodySetOmega(m_pNewtonBody, a_vVel.v);
	}
	cVector3f cPhysicsBodyNewton::GetAngularVelocity() const
	{
		cVector3f vVel;
		NewtonBodyGetOmega(m_pNewtonBody, vVel.v);
		return vVel;
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::SetLinearDamping(float a_fDamping)
	{
		NewtonBodySetLinearDamping(m_pNewtonBody, a_fDamping);
	}
	float cPhysicsBodyNewton::GetLinearDamping() const
	{
		return NewtonBodyGetLinearDamping(m_pNewtonBody);
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::SetAngularDamping(float a_fDamping)
	{
		float fDamp[3] = {a_fDamping, a_fDamping, a_fDamping};
		NewtonBodySetAngularDamping(m_pNewtonBody, fDamp);
	}
	float cPhysicsBodyNewton::GetAngularDamping() const
	{
		float fDamp[3];
		NewtonBodyGetAngularDamping(m_pNewtonBody, fDamp);
		return fDamp[0];
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::SetMaxLinearSpeed(float a_fSpeed)
	{
		m_fMaxLinearSpeed = a_fSpeed;
	}
	float cPhysicsBodyNewton::GetMaxLinearSpeed() const
	{
		return m_fMaxLinearSpeed;
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::SetMaxAngularSpeed(float a_fSpeed)
	{
		m_fMaxAngularSpeed = a_fSpeed;
	}
	float cPhysicsBodyNewton::GetMaxAngularSpeed() const
	{
		return m_fMaxAngularSpeed;
	}

	//--------------------------------------------------------------

	cMatrixf cPhysicsBodyNewton::GetInertiaMatrix()
	{
		float fIxx, fIyy, fIzz, fMass;

		NewtonBodyGetMassMatrix(m_pNewtonBody, &fMass, &fIxx, &fIyy, &fIzz);

		cMatrixf mtxRot = GetLocalMatrix().GetRotation();
		cMatrixf mtxTransRot = mtxRot.GetTranspose();
		cMatrixf mtxI( fIxx, 0, 0, 0,
			0, fIyy, 0, 0,
			0, 0, fIzz, 0,
			0, 0, 0, 1);

		return cMath::MatrixMul(cMath::MatrixMul(mtxRot, mtxI), mtxTransRot);
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::SetMass(float a_fMass)
	{
		cCollideShapeNewton *pShapeNewton = static_cast<cCollideShapeNewton*>(m_pShape);

		cVector3f vInertia;
		cVector3f vOffset;

		NewtonConvexCollisionCalculateInertialMatrix(pShapeNewton->GetNewtonCollision(),
			vInertia.v, vOffset.v);

		vInertia = vInertia * a_fMass;

		NewtonBodyGetCentreOfMass(m_pNewtonBody, vOffset.v);

		NewtonBodySetMassMatrix(m_pNewtonBody, a_fMass, vInertia.x, vInertia.y, vInertia.z);
		m_FMass = a_fMass;
	}

	float cPhysicsBodyNewton::GetMass() const
	{
		return m_FMass;
	}

	void cPhysicsBodyNewton::SetMassCentre(const cVector3f &a_vCentre)
	{
		NewtonBodySetCentreOfMass(m_pNewtonBody, a_vCentre.v);
	}

	cVector3f cPhysicsBodyNewton::GetMassCentre() const
	{
		cVector3f vCentre;
		NewtonBodyGetCentreOfMass(m_pNewtonBody, vCentre.v);
		return vCentre;
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::AddForce(const cVector3f &a_vForce)
	{
		m_vTotalForce += a_vForce;
		SetEnabled(true);
	}

	void cPhysicsBodyNewton::AddForceAtPosition(const cVector3f &a_vForce, const cVector3f &a_vPos)
	{
		m_vTotalForce += a_vForce;

		cVector3f vLocalPos = a_vPos - GetLocalPosition();
		cVector3f vMassCentre = GetMassCentre();
		if (vMassCentre != cVector3f(0,0,0))
		{
			vMassCentre = cMath::MatrixMul(GetLocalMatrix().GetRotation(), vMassCentre);
			vLocalPos -= vMassCentre;
		}

		cVector3f vTorque = cMath::Vector3Cross(vLocalPos, a_vPos);

		m_vTotalTorque += vTorque;
		SetEnabled(true);
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::AddTorque(const cVector3f &a_vTorque)
	{
		m_vTotalTorque += a_vTorque;
		SetEnabled(true);
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::AddImpulse(const cVector3f &a_vImpulse)
	{
		cVector3f vMassCentre = GetMassCentre();
		if (vMassCentre != cVector3f(0,0,0))
		{
			cVector3f vCentreOffset = cMath::MatrixMul(GetWorldMatrix().GetRotation(), vMassCentre);

			cVector3f vWorldPosition = GetWorldPosition() + vCentreOffset;
			NewtonBodyAddImpulse(m_pNewtonBody, a_vImpulse.v, vWorldPosition.v);
		}
		else
			NewtonBodyAddImpulse(m_pNewtonBody, a_vImpulse.v, GetWorldPosition().v);
	}
	void cPhysicsBodyNewton::AddImpulseAtPosition(const cVector3f &a_vImpulse, const cVector3f &a_vPos)
	{
		NewtonBodyAddImpulse(m_pNewtonBody, a_vImpulse.v, a_vPos.v);
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::SetEnabled(bool a_bEnabled)
	{
		NewtonBodySetFreezeState(m_pNewtonBody, !a_bEnabled);
	}
	bool cPhysicsBodyNewton::GetEnabled() const
	{
		return NewtonBodyGetFreezeState(m_pNewtonBody);
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::SetAutoDisable(bool a_bEnabled)
	{
		NewtonBodySetAutoSleep(m_pNewtonBody, a_bEnabled);
	}
	bool cPhysicsBodyNewton::GetAutoDisable() const
	{
		return NewtonBodyGetAutoSleep(m_pNewtonBody) == 0 ? false : true;
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::SetContinuousCollision(bool a_bOn)
	{
		NewtonBodySetContinuousCollisionMode(m_pNewtonBody, a_bOn ? 1 : 0);
	}

	bool cPhysicsBodyNewton::GetContinuousCollision()
	{
		return NewtonBodyGetContinuousCollisionMode(m_pNewtonBody) == 1 ? true : false;
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::SetGravity(bool a_bEnabled)
	{
		m_bGravity = a_bEnabled;
	}

	bool cPhysicsBodyNewton::GetGravity() const
	{
		return m_bGravity;
	}

	//--------------------------------------------------------------

	static iLowLevelGraphics *g_pLowLevelGraphics;
	static cColor g_DebugColor;

	static void RenderDebugPolygon(void *a_pVoid,
		int a_lVertexCount, const dFloat *a_pFaceVertex, int a_lId)
	{
		int i;

		i = a_lVertexCount - 1;

		cVector3f vP0(a_pFaceVertex[i * 3 + 0], a_pFaceVertex[i * 3 + 1], a_pFaceVertex[i * 3 + 2]);
		for (int i = 0; i < a_lVertexCount; ++i)
		{
			cVector3f vP1(a_pFaceVertex[i * 3 + 0], a_pFaceVertex[i * 3 + 1], a_pFaceVertex[i * 3 + 2]);

			g_pLowLevelGraphics->DrawLine(vP0, vP1, g_DebugColor);

			vP0 = vP1;
		}
	}

	void cPhysicsBodyNewton::RenderDebugGeometry(iLowLevelGraphics *a_pLowLevel, const cColor &a_Color)
	{
		//cPhysicsWorldNewton *pPhysicsWorld = static_cast<cPhysicsWorldNewton*>(NewtonWorldGetUserData(m_pNewtonWorld));
		//pPhysicsWorld->GetWorld3D()->gets

		NewtonCollision *pCollision = NewtonBodyGetCollision(m_pNewtonBody);
		float matrix[4][4];
		NewtonBodyGetMatrix(m_pNewtonBody, &matrix[0][0]);
		g_pLowLevelGraphics = a_pLowLevel;
		g_DebugColor = a_Color;
		NewtonCollision *pNewtonCollision = NewtonBodyGetCollision(m_pNewtonBody);
		NewtonCollisionForEachPolygonDo(pNewtonCollision,
			m_mtxLocalTransform.GetTranspose().m[0], RenderDebugPolygon, this);
	}

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::ClearForces()
	{
		m_vTotalForce = cVector3f(0,0,0);
		m_vTotalTorque = cVector3f(0,0,0);
	}

	void cPhysicsBodyNewton::DeleteLowLevel()
	{
		NewtonDestroyBody(m_pNewtonBody);

		efeDelete(m_pCallback);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// STATIC NEWTON CALLBACKS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cPhysicsBodyNewton::OnTransformCallback(const NewtonBody *a_pBody, const dFloat *a_pMatrix, int a_lThreadIndex)
	{
		cPhysicsBodyNewton *pRigidBody = static_cast<cPhysicsBodyNewton*>(NewtonBodyGetUserData(a_pBody));

		pRigidBody->m_mtxLocalTransform.FromTranspose(a_pMatrix);

		m_bUseCallback = false;
		pRigidBody->SetTransformUpdated(true);
		m_bUseCallback = true;

		if (pRigidBody->m_pNode) pRigidBody->m_pNode->SetMatrix(pRigidBody->m_mtxLocalTransform);
	}

	//--------------------------------------------------------------

	static cPlanef g_SurfacePlane;
	static int BuoyancyPlaneCallback(const int a_lCollisionID, void *a_pContext, 
		const float *a_fGlobalSpaceMatrix, float *a_fGlobalSpacePlane)
	{
		a_fGlobalSpacePlane[0] = g_SurfacePlane.a;
		a_fGlobalSpacePlane[1] = g_SurfacePlane.b;
		a_fGlobalSpacePlane[2] = g_SurfacePlane.c;
		a_fGlobalSpacePlane[3] = g_SurfacePlane.d;
		return 1;
	}

	void cPhysicsBodyNewton::OnUpdateCallback(const NewtonBody *a_pBody, dFloat a_fTimeStep, int a_lThreadIndex)
	{
		float fMass;
		float fX, fY, fZ;

		cPhysicsBodyNewton *pRigidBody = static_cast<cPhysicsBodyNewton*>(NewtonBodyGetUserData(a_pBody));

		if (pRigidBody->IsActive() == false) return;

		cVector3f vGravity = pRigidBody->m_pWorld->GetGravity();

		if (pRigidBody->m_bGravity)
		{
			NewtonBodyGetMassMatrix(a_pBody, &fMass, &fX, &fY, &fZ);

			float fForce[3] = {fMass * vGravity.x, fMass * vGravity.y, fMass * vGravity.z};

			NewtonBodyAddForce(a_pBody, &fForce[0]);
		}

		if (pRigidBody->m_Buoyancy.m_bActive)
		{
			g_SurfacePlane = pRigidBody->m_Buoyancy.m_Surface;
			/*NewtonBodyAddBuoyancyForce(a_pBody, 
				pRigidBody->m_Buoyancy.m_fDensity,
				pRigidBody->m_Buoyancy.m_fLinearViscosity,
				pRigidBody->m_Buoyancy.m_fAngularViscosity,
				vGravity.v, BuoyancyPlaneCallback,
				pRigidBody);*/
		}

		NewtonBodyAddForce(a_pBody, pRigidBody->m_vTotalForce.v);
		NewtonBodyAddTorque(a_pBody, pRigidBody->m_vTotalTorque.v);

		if (pRigidBody->m_fMaxLinearSpeed > 0)
		{
			cVector3f vVel = pRigidBody->GetLinearVelocity();
			float fSpeed = vVel.Length();
			if (fSpeed > pRigidBody->m_fMaxAngularSpeed)
			{
				vVel = cMath::Vector3Normalize(vVel) * pRigidBody->m_fMaxAngularSpeed;
				pRigidBody->SetAngularVelocity(vVel);
			}
		}
	}
}