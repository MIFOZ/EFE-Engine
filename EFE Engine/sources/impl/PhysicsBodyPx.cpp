#include "impl/PhysicsBodyPx.h"

#include "impl/CollideShapePx.h"
#include "impl/PhysicsWorldPx.h"
#include "impl/PhysicsMaterialPx.h"
#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "math/Math.h"
#include "scene/Node3D.h"

#include <extensions/PxSimpleFactory.h>

namespace efe
{
	bool cPhysicsBodyPx::m_bUseCallback = true;

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPhysicsBodyPx::cPhysicsBodyPx(const tString &a_sName, iPhysicsWorld *a_pWorld, iCollideShape *a_pShape)
		: iPhysicsBody(a_sName, a_pWorld, a_pShape)
	{
		cPhysicsWorldPx *pWorldPx = static_cast<cPhysicsWorldPx*>(a_pWorld);
		cCollideShapePx *pShapePx = static_cast<cCollideShapePx*>(a_pShape);
		
		m_pPxScene = pWorldPx->GetPxScene();
		
		PxMat44 mat((float *)a_pShape->GetOffset().v);
		PxTransform transform/*(PxVec3(0.0f,0.0f,0.0f))*/(mat);
		cPhysicsMaterialPx *pPxMat = static_cast<cPhysicsMaterialPx*>(pWorldPx->GetMaterialFromName("Default"));
		PxGeometry *pGeom = efeNew(PxBoxGeometry, (10,10,10));
		m_pPxBody = PxCreateDynamic(*pWorldPx->GetPxSDK(), transform, /**pGeom,*/*pShapePx->GetPxGeometry(),
			*pPxMat->GetPxMaterial(), 10);

		if (m_pPxBody==NULL)
		{
			FatalError("COuldn't create physics body '%s'\n", a_sName.c_str());
		}

		m_pPxScene->addActor(*m_pPxBody);

		m_pCallback = efeNew(cPhysicsBodyPxCallback, ());

		AddCallback(m_pCallback);

		//m_pPxBody->set

		m_pPxBody->userData = this;
	}

	//-------------------------------------------------------------

	cPhysicsBodyPx::~cPhysicsBodyPx()
	{
		//m_pPxBody->release();
	}

	//-------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// CALLBACK METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	void cPhysicsBodyPxCallback::OnTransformUpdate(iEntity3D *a_pEntity)
	{
		if (cPhysicsBodyPx::m_bUseCallback==false) return;

		cPhysicsBodyPx *pRigidBody = static_cast<cPhysicsBodyPx*>(a_pEntity);
		PxTransform transform(PxMat44((float *)a_pEntity->GetLocalMatrix().v));
		pRigidBody->m_pPxBody->setGlobalPose(transform, true);
	}

	//-------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	cVector3f cPhysicsBodyPx::GetLinearVelocity()const
	{
		PxVec3 vVel = m_pPxBody->getLinearVelocity();
		return cVector3f(vVel.x, vVel.y, vVel.z);
	}

	//-------------------------------------------------------------

	cVector3f cPhysicsBodyPx::GetAngularVelocity()const
	{
		PxVec3 vVel = m_pPxBody->getAngularVelocity();
		return cVector3f(vVel.x, vVel.y, vVel.z);
	}

	//-------------------------------------------------------------

	void cPhysicsBodyPx::SetMass(float a_fMass)
	{
		m_pPxBody->setMass(a_fMass);

		m_fMass = a_fMass;
	}

	//-------------------------------------------------------------

	float cPhysicsBodyPx::GetMass()const
	{
		return m_fMass;
	}

	//-------------------------------------------------------------
	
	cVector3f cPhysicsBodyPx::GetMassCentre()const
	{
		PxTransform vPos = m_pPxBody->getCMassLocalPose();

		return cVector3f(vPos.p.x, vPos.p.y, vPos.p.z);
	}

	//-------------------------------------------------------------

	void cPhysicsBodyPx::AddForceAtPosition(const cVector3f &a_vForce, const cVector3f &a_vPos)
	{
		m_vTotalForce += a_vForce;

		cVector3f vLocalPos = a_vPos - GetLocalPosition();
		cVector3f vMassCentre = GetMassCentre();
		if (vMassCentre != cVector3f(0,0,0))
		{
			vMassCentre = cMath::MatrixMul(GetLocalMatrix().GetRotation(), vMassCentre);
			vLocalPos -= vMassCentre;
		}

		cVector3f vTorque = cMath::Vector3Cross(vLocalPos, a_vForce);

		m_vTotalTorque += vTorque;

		SetEnabled(true);
	}

	//-------------------------------------------------------------

	void cPhysicsBodyPx::SetEnabled(bool a_bEnabled)
	{
		if (a_bEnabled)
			m_pPxBody->putToSleep();
		else
			m_pPxBody->wakeUp();
	}

	//-------------------------------------------------------------

	void cPhysicsBodyPx::SetGravity(bool a_bEnabled)
	{
		m_bGravity = a_bEnabled;
	}
	bool cPhysicsBodyPx::GetGravity()const
	{
		return m_bGravity;
	}

	//-------------------------------------------------------------

	void cPhysicsBodyPx::ClearForces()
	{
		m_vTotalForce = cVector3f(0,0,0);
		m_vTotalTorque = cVector3f(0,0,0);
	}

	//-------------------------------------------------------------

	void cPhysicsBodyPx::DeleteLowLevel()
	{
		m_pPxBody->release();

		efeDelete(m_pCallback);
	}

	//-------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	void cPhysicsBodyPx::OnTransformCallback(const PxRigidBody *a_pBody, PxReal *a_pMatrix)
	{
		cPhysicsBodyPx *pRigidBody = (cPhysicsBodyPx*)a_pBody->userData;

		//pRigidBody->m_mtxLocalTransform

		m_bUseCallback = false;
		pRigidBody->SetTransformUpdated(true);
		m_bUseCallback = true;

		if (pRigidBody->m_pNode)pRigidBody->m_pNode->SetMatrix(pRigidBody->m_mtxLocalTransform);
	}
}