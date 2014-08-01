#include "impl/PhysicsMaterialNewton.h"

#include "impl/PhysicsWorldNewton.h"
#include "impl/PhysicsBodyNewton.h"
#include "physics/SurfaceData.h"

#include "system/LowLevelSystem.h"
#include <algorithm>

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPhysicsMaterialNewton::cPhysicsMaterialNewton(const tString &a_sName, iPhysicsWorld *a_pWorld, int a_lMatId)
		: iPhysicsMaterial(a_sName, a_pWorld)
	{
		cPhysicsWorldNewton *pNWorld = static_cast<cPhysicsWorldNewton*>(a_pWorld);

		m_pNewtonWorld = pNWorld->GetNewtonWorld();

		if (a_lMatId == -1)
			m_lMaterialId = NewtonMaterialCreateGroupID(m_pNewtonWorld);
		else
			m_lMaterialId = a_lMatId;

		m_FrictionMode = ePhysicsMaterialCombMode_Average;
		m_ElasticityMode = ePhysicsMaterialCombMode_Average;

		m_fElasticity = 0.5f;
		m_fStaticFriction = 0.3f;
		m_fKinectFriction = 0.3f;
	}

	//--------------------------------------------------------------

	cPhysicsMaterialNewton::~cPhysicsMaterialNewton()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cPhysicsMaterialNewton::SetElasticity(float a_fElasctictiy)
	{
		m_fElasticity = a_fElasctictiy;

		UpdateMaterials();
	}

	float cPhysicsMaterialNewton::GetElasticity() const
	{
		return m_fElasticity;
	}

	//--------------------------------------------------------------

	void cPhysicsMaterialNewton::SetStaticFriction(float a_fFriction)
	{
		m_fStaticFriction = a_fFriction;

		UpdateMaterials();
	}

	float cPhysicsMaterialNewton::GetStaticFriction() const
	{
		return m_fStaticFriction;
	}

	//--------------------------------------------------------------

	void cPhysicsMaterialNewton::SetKineticFriction(float a_fFriction)
	{
		m_fKinectFriction = a_fFriction;

		UpdateMaterials();
	}

	float cPhysicsMaterialNewton::GetKineticFriction() const
	{
		return m_fKinectFriction;
	}

	//--------------------------------------------------------------

	void cPhysicsMaterialNewton::SetFrictionCombMode(ePhysicsMaterialCombMode a_Mode)
	{
		m_FrictionMode = a_Mode;

		UpdateMaterials();
	}

	//--------------------------------------------------------------

	void cPhysicsMaterialNewton::SetElasticityCombMode(ePhysicsMaterialCombMode a_Mode)
	{
		m_ElasticityMode = a_Mode;

		UpdateMaterials();
	}

	//--------------------------------------------------------------

	void cPhysicsMaterialNewton::UpdateMaterials()
	{
		cPhysicsMaterialIterator MatIt = m_pWorld->GetMaterialIterator();

		while (MatIt.HasNext())
		{
			cPhysicsMaterialNewton *pMat = static_cast<cPhysicsMaterialNewton*>(MatIt.Next());

			ePhysicsMaterialCombMode frictionMode = (ePhysicsMaterialCombMode) std::max(m_FrictionMode, 
				pMat->m_FrictionMode);
			ePhysicsMaterialCombMode elasticityMode = (ePhysicsMaterialCombMode) std::max(m_ElasticityMode, 
				pMat->m_ElasticityMode);

			if (pMat==this)
			{
				frictionMode = ePhysicsMaterialCombMode_Average;
				elasticityMode = ePhysicsMaterialCombMode_Average;
			}

			NewtonMaterialSetDefaultElasticity(m_pNewtonWorld, m_lMaterialId, pMat->m_lMaterialId, 
				Combine(elasticityMode, m_fElasticity, pMat->m_fElasticity));

			NewtonMaterialSetDefaultFriction(m_pNewtonWorld, m_lMaterialId, pMat->m_lMaterialId, 
				Combine(frictionMode, m_fStaticFriction, pMat->m_fStaticFriction),
				Combine(frictionMode, m_fKinectFriction, pMat->m_fKinectFriction));

			NewtonMaterialSetCollisionCallback(m_pNewtonWorld, m_lMaterialId, pMat->m_lMaterialId,
				(void*)NULL, BeginContactCallback, ProcessContactCallback);
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	float cPhysicsMaterialNewton::Combine(ePhysicsMaterialCombMode a_Mode, float a_fX, float a_fY)
	{
		switch (a_Mode)
		{
		case efe::ePhysicsMaterialCombMode_Average: return (a_fX + a_fY) / 2;
		case efe::ePhysicsMaterialCombMode_Min: return std::min(a_fX, a_fY);
		case efe::ePhysicsMaterialCombMode_Max: return std::max(a_fX, a_fY);
		case efe::ePhysicsMaterialCombMode_Multiply: return a_fX * a_fY;
		}

		return (a_fX + a_fY) / 2;
	}

	//////////////////////////////////////////////////////////////
	// STATIC NEWTON CALLBACKS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	int cPhysicsMaterialNewton::BeginContactCallback(const NewtonMaterial *a_pMaterial,
			const NewtonBody *a_pBody1, const NewtonBody *a_pBody2, int a_lThreadIndex)
	{
		iPhysicsBody *pContactBody1 = (cPhysicsBodyNewton*) NewtonBodyGetUserData(a_pBody1);
		iPhysicsBody *pContactBody2 = (cPhysicsBodyNewton*) NewtonBodyGetUserData(a_pBody2);

		if (pContactBody1->GetCollide() == false) return 0;
		if (pContactBody2->GetCollide() == false) return 0;

		if (pContactBody1->IsActive() == false) return 0;
		if (pContactBody2->IsActive() == false) return 0;

		if (pContactBody1->IsRagDoll() && pContactBody2->GetCollideRagDoll()==false) return 0;
		if (pContactBody2->IsRagDoll() && pContactBody1->GetCollideRagDoll()==false) return 0;

		if (pContactBody1->IsCharacter() && pContactBody2->GetCollideCharacter()==false) return 0;
		if (pContactBody2->IsCharacter() && pContactBody1->GetCollideCharacter()==false) return 0;

		if (pContactBody1->OnBeginCollision(pContactBody2) == false) return 0;
		if (pContactBody2->OnBeginCollision(pContactBody1) == false) return 0;

		return 1;
	}

	//--------------------------------------------------------------

	void cPhysicsMaterialNewton::ProcessContactCallback(const NewtonJoint *a_pJoint, dFloat a_fTimeStep, int a_lThreadIndex)
	{
		cPhysicsContactData ContactData;
		int lContactNum = 0;
		NewtonBody *pBody0 = NewtonJointGetBody0(a_pJoint);

		iPhysicsBody *pContactBody1 = (iPhysicsBody*) NewtonBodyGetUserData(pBody0);
		iPhysicsBody *pContactBody2 = (iPhysicsBody*) NewtonBodyGetUserData(NewtonJointGetBody1(a_pJoint));

		for (void *pContact = NewtonContactJointGetFirstContact(a_pJoint);
			pContact;
			pContact = NewtonContactJointGetNextContact(a_pJoint, pContact))
		{
			NewtonMaterial *pMaterial = NewtonContactGetMaterial(pContact);

			//Normal speed
			float fNormSpeed = NewtonMaterialGetContactNormalSpeed(pMaterial);
			if (ContactData.m_vMaxContactNormalSpeed < fNormSpeed)
				ContactData.m_vMaxContactNormalSpeed = fNormSpeed;

			//Tangent speed
			float fTanSpeed0 = NewtonMaterialGetContactTangentSpeed(pMaterial, 0);
			float fTanSpeed1 = NewtonMaterialGetContactTangentSpeed(pMaterial, 1);
			if (std::abs(ContactData.m_vMaxContactTangentSpeed) < std::abs(fTanSpeed0))
				ContactData.m_vMaxContactTangentSpeed = fTanSpeed0;
			if (std::abs(ContactData.m_vMaxContactTangentSpeed) < std::abs(fTanSpeed1))
				ContactData.m_vMaxContactTangentSpeed = fTanSpeed1;

			//Force
			cVector3f vForce;
			NewtonMaterialGetContactForce(pMaterial, pBody0, vForce.v);
			ContactData.m_vForce += vForce;

			//Position and normal
			cVector3f vPos, vNormal;
			NewtonMaterialGetContactPositionAndNormal(pMaterial, pBody0, vPos.v, vNormal.v);
			ContactData.m_vContactNormal += vNormal;
			ContactData.m_vContactPosition += vPos;

			if (pContactBody1->GetWorld()->GetSaveContactPoints())
			{
				cCollidePoint collidePoint;
				collidePoint.m_fDepth = 1;
				NewtonMaterialGetContactPositionAndNormal(pMaterial, pBody0, collidePoint.m_vPoint.v,
					collidePoint.m_vNormal.v);

				pContactBody1->GetWorld()->GetContactPoints()->push_back(collidePoint);
			}

			lContactNum++;
		}

		if (lContactNum <= 0) return;

		iPhysicsMaterial *pMaterial1 = pContactBody1->GetMaterial();
		iPhysicsMaterial *pMaterial2 = pContactBody2->GetMaterial();

		ContactData.m_vContactNormal = ContactData.m_vContactNormal / (float)lContactNum;
		ContactData.m_vContactPosition = ContactData.m_vContactPosition / (float)lContactNum;

		pMaterial1->GetSurfaceData()->CreateImpactEffect(ContactData.m_vMaxContactNormalSpeed,
			ContactData.m_vContactPosition, lContactNum, pMaterial2->GetSurfaceData());

		int lPrio1 = pMaterial1->GetSurfaceData()->GetPriority();
		int lPrio2 = pMaterial2->GetSurfaceData()->GetPriority();

		if (lPrio1 >= lPrio2)
		{
			if (std::abs(ContactData.m_vMaxContactNormalSpeed) > 0)
				pMaterial1->GetSurfaceData()->OnImpact(ContactData.m_vMaxContactNormalSpeed,
				ContactData.m_vContactPosition, lContactNum, pContactBody1);

			if (std::abs(ContactData.m_vMaxContactTangentSpeed) > 0)
				pMaterial1->GetSurfaceData()->OnSlide(ContactData.m_vMaxContactTangentSpeed,
				ContactData.m_vContactPosition, lContactNum, pContactBody1, pContactBody2);
		}

		if (lPrio2 >= lPrio1 && pMaterial2 != pMaterial1)
		{
			if (std::abs(ContactData.m_vMaxContactNormalSpeed) > 0)
				pMaterial2->GetSurfaceData()->OnImpact(ContactData.m_vMaxContactNormalSpeed,
				ContactData.m_vContactPosition, lContactNum, pContactBody2);

			if (std::abs(ContactData.m_vMaxContactTangentSpeed) > 0)
				pMaterial1->GetSurfaceData()->OnSlide(ContactData.m_vMaxContactTangentSpeed,
				ContactData.m_vContactPosition, lContactNum, pContactBody2, pContactBody1);
		}

		pContactBody1->OnCollide(pContactBody2, &ContactData);
		pContactBody2->OnCollide(pContactBody1, &ContactData);
	}
}