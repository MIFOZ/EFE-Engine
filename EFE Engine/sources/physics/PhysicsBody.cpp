#include "physics/PhysicsBody.h"

#include "physics/CollideShape.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsJoint.h"
#include "system/LowLevelSystem.h"
#include "physics/PhysicsMaterial.h"
#include "physics/SurfaceData.h"

#include "scene/World3D.h"
#include "scene/Scene.h"
#include "game/Game.h"

#include "scene/SoundEntity.h"


#include "scene/Node3D.h"

#include "math/Math.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	iPhysicsBody::iPhysicsBody(const tString &a_sName, iPhysicsWorld *a_pWorld, iCollideShape *a_pShape)
		: iEntity3D(a_sName)
	{
		m_pWorld = a_pWorld;
		m_pShape = a_pShape;
		m_pNode = NULL;

		a_pShape->IncUserCount();

		m_BoundingVolume.SetLocalMinMax(a_pShape->GetBoundingVolume().GetMin(),
										a_pShape->GetBoundingVolume().GetMax());

		m_pMaterial = m_pWorld->GetMaterialFromName("Default");

		m_pCharacterBody = NULL;

		m_bBlocksSound = false;
		m_bBlocksLight = true;

		m_pScrapeBody = NULL;
		m_pScrapeSoundEntity = NULL;
		m_pRollSoundEntity = NULL;
		m_bHasImpact = false;
		m_bHasSlide = false;

		m_lSlideCount = 0;
		m_lImpactCount = 0;

		m_lBuoyancy = -1;

		m_bCanAttachCharacter = false;

		m_pUserData = NULL;

		m_pHapticsShape = NULL;

		m_bPushedByCharacterGravity = false;

		m_bIsCharacter = false;
		m_bCollideCharacter = true;

		m_bIsRagDoll = false;
		m_bCollideRagDoll = true;

		m_bCollide = true;

		m_bVolatile = false;

		m_bDisableAfterSimulation = false;

		m_bHasCollision = false;

		m_mtxPrevScrapeMatrix = cMatrixf::Identity;
	}

	//-------------------------------------------------------------

	iPhysicsBody::~iPhysicsBody()
	{
		
	}

	//-------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	void iPhysicsBody::Destroy()
	{
		if (m_pNode) efeDelete(m_pNode);
		m_pWorld->DestroyShape(m_pShape);

		for (int i=0;i<(int)m_vJoints.size();i++)
		{
			iPhysicsJoint *pJoint = m_vJoints[i];

			pJoint->RemoveBody(this);

			if (pJoint->GetParentBody()==NULL && pJoint->GetChildBody()==NULL)
				m_pWorld->DestroyJoint(pJoint);
		}

		if (m_pScrapeSoundEntity && m_pWorld->GetWorld3D()->SoundEntityExists(m_pScrapeSoundEntity))
			m_pWorld->GetWorld3D()->DestroySoundEntity(m_pScrapeSoundEntity);
		if (m_pRollSoundEntity && m_pWorld->GetWorld3D()->SoundEntityExists(m_pRollSoundEntity))
			m_pWorld->GetWorld3D()->DestroySoundEntity(m_pRollSoundEntity);

		DeleteLowLevel();
	}

	//-------------------------------------------------------------

	iPhysicsMaterial *iPhysicsBody::GetMaterial()
	{
		return m_pMaterial;
	}

	//-------------------------------------------------------------

	cNode3D *iPhysicsBody::GetNode()
	{
		return m_pNode;
	}

	cNode3D *iPhysicsBody::CreateNode()
	{
		if (m_pNode) return m_pNode;

		m_pNode = efeNew(cNode3D, ());
		return m_pNode;
	}

	//-------------------------------------------------------------

	iCollideShape *iPhysicsBody::GetShape()
	{
		return m_pShape;
	}

	//-------------------------------------------------------------

	void iPhysicsBody::AddJoint(iPhysicsJoint *a_pJoint)
	{
		m_vJoints.push_back(a_pJoint);
	}

	iPhysicsJoint *iPhysicsBody::GetJoint(int a_lIndex)
	{
		return m_vJoints[a_lIndex];
	}

	int iPhysicsBody::GetJointNum()
	{
		return (int)m_vJoints.size();
	}

	void iPhysicsBody::RemoveJoint(iPhysicsJoint *a_pJoint)
	{
		std::vector<iPhysicsJoint*>::iterator it = m_vJoints.begin();
		for (; it != m_vJoints.end(); ++it)
		{
			if (*it == a_pJoint)
			{
				m_vJoints.erase(it);
				break;
			}
		}
	}

	//-------------------------------------------------------------

	cVector3f iPhysicsBody::GetVelocityAtPosition(cVector3f a_vPos)
	{
		return GetLinearVelocity() + cMath::Vector3Cross(GetAngularVelocity(), a_vPos - GetLocalPosition());
	}

	//-------------------------------------------------------------

	void iPhysicsBody::UpdateBeforeSimulate(float a_fTimeStep)
	{
		//Reset that the body had contact
		SetHasSlide(false);
		SetHasImpact(false);
		m_bHasCollision = false;
	}

	void iPhysicsBody::UpdateAfterSimulate(float a_fTimeStep)
	{
		if (m_bDisableAfterSimulation)
		{
			m_bDisableAfterSimulation = false;
			SetEnabled(false);
		}

		if (HasSlide()==false)
		{
			if (GetScrapeSoundEntity())
			{
				if (m_lSlideCount<=0)
				{
					if (m_pWorld->GetWorld3D())
					{
						if (m_pWorld->GetWorld3D()->SoundEntityExists(GetScrapeSoundEntity()))
							GetScrapeSoundEntity()->FadeOut(5.2f);
					}

					SetScrapeSoundEntity(NULL);
					SetScrapeBody(NULL);
				}
				else if (m_lSlideCount>0)
					m_lSlideCount--;
			}
		}
		else
			m_lSlideCount = 8;

		if (m_pMaterial) m_pMaterial->GetSurfaceData()->UpdateRollEffect(this);
	}

	//-------------------------------------------------------------

	bool iPhysicsBody::OnBeginCollision(iPhysicsBody *a_pBody)
	{
		if (m_lstBodyCallbacks.empty()) return true;

		bool bReturn = true;

		tPhysicsBodyCallbackListIt it = m_lstBodyCallbacks.begin();
		for (; it != m_lstBodyCallbacks.end(); ++it)
		{
			iPhysicsBodyCallback *pCallback = *it;

			if (pCallback->OnBeginCollision(this, a_pBody) == false) bReturn = false;
		}

		return bReturn;
	}

	//-------------------------------------------------------------

	void iPhysicsBody::OnCollide(iPhysicsBody *a_pBody, cPhysicsContactData *a_pContactData)
	{
		m_bHasCollision = true;

		if (m_lstBodyCallbacks.empty()) return;

		tPhysicsBodyCallbackListIt it = m_lstBodyCallbacks.begin();
		for (; it != m_lstBodyCallbacks.end(); ++it)
		{
			iPhysicsBodyCallback *pCallback = *it;

			pCallback->OnCollide(this, a_pBody, a_pContactData);
		}
	}

	//-------------------------------------------------------------

	void iPhysicsBody::AddAttachedCharacter(iCharacterBody *a_pChar)
	{
		RemoveAttachedCharacter(a_pChar);

		m_lstAttachedCharacters.push_back(a_pChar);
	}

	//-------------------------------------------------------------

	void iPhysicsBody::RemoveAttachedCharacter(iCharacterBody *a_pChar)
	{
		std::list<iCharacterBody*>::iterator it = m_lstAttachedCharacters.begin();
		for (; it != m_lstAttachedCharacters.end(); ++it)
		{
			if (a_pChar == *it)
			{
				m_lstAttachedCharacters.erase(it);
				break;
			}
		}

	}
}