#include "physics/PhysicsWorld.h"

#include "physics/CollideShape.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsMaterial.h"
#include "physics/CharacterBody.h"
#include "physics/PhysicsJoint.h"
#include "physics/PhysicsController.h"
#include "physics/SurfaceData.h"
#include "system/LowLevelSystem.h"
#include "system/System.h"
#include "math/Math.h"
#include "graphics/LowLevelGraphics.h"
#include "scene/World3D.h"
#include "scene/PortalContainer.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iPhysicsWorld::iPhysicsWorld()
	{
		m_bLogDebug = false;
	}

	//--------------------------------------------------------------

	iPhysicsWorld::~iPhysicsWorld()
	{
	
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void iPhysicsWorld::Update(float a_fTimeStep)
	{
		m_vContactPoints.clear();

		//Update controllers
		tPhysicsControllerListIt CtrIt = m_lstControllers.begin();
		for (; CtrIt != m_lstControllers.end(); ++CtrIt)
		{
			iPhysicsController *pCtrl = *CtrIt;

			pCtrl->Update(a_fTimeStep);
		}

		//Update character bodies
		unsigned int lTime = GetApplicationTime();
		tCharacterBodyListIt CharIt = m_lstCharBodies.begin();
		for (; CharIt != m_lstCharBodies.end(); ++CharIt)
		{
			iCharacterBody *pBody = *CharIt;

			pBody->Update(a_fTimeStep);
		}

		//Update the rigid bodies before simulation
		tPhysicsBodyListIt BodyIt = m_lstBodies.begin();
		for (; BodyIt != m_lstBodies.end(); ++BodyIt)
		{
			iPhysicsBody *pBody = *BodyIt;

			pBody->UpdateBeforeSimulate(a_fTimeStep);
		}

		lTime = GetApplicationTime();
		Simulate(a_fTimeStep);

		//Update the joints after simulation
		tPhysicsJointListIt JointIt = m_lstJoints.begin();
		for (; JointIt != m_lstJoints.end();)
		{
			iPhysicsJoint *pJoint = *JointIt;

			pJoint->OnPhysicsUpdate();

			if (pJoint->CheckBreakage())
			{
				JointIt = m_lstJoints.erase(JointIt);
				efeDelete(pJoint);
			}
			else
				++JointIt;
		}

		//Update the rigid bodies after simulation
		BodyIt = m_lstBodies.begin();
		for (; BodyIt != m_lstBodies.end(); ++BodyIt)
		{
			iPhysicsBody *pBody = *BodyIt;

			pBody->UpdateAfterSimulate(a_fTimeStep);
		}
	}

	//--------------------------------------------------------------

	void iPhysicsWorld::DestroyShape(iCollideShape *a_pShape)
	{
		a_pShape->DecUserCount();
		if (a_pShape->HasUsers()==false)
			STLFindAndDelete(m_lstShapes, a_pShape);
	}

	//--------------------------------------------------------------

	void iPhysicsWorld::DestroyJoint(iPhysicsJoint *a_pJoint)
	{
		STLFindAndDelete(m_lstJoints, a_pJoint);
	}

	//--------------------------------------------------------------

	cPhysicsJointIterator iPhysicsWorld::GetJointIterator()
	{
		return cPhysicsJointIterator(&m_lstJoints);
	}

	//--------------------------------------------------------------

	iPhysicsMaterial *iPhysicsWorld::GetMaterialFromName(const tString &a_sName)
	{
		tPhysicsMaterialMapIt it = m_mapMaterials.find(a_sName);
		if (it == m_mapMaterials.end())
			return NULL;

		iPhysicsMaterial *pMaterial = it->second;

		if (pMaterial->IsPreloaded()==false && pMaterial->GetSurfaceData())
		{
			pMaterial->SetPreloaded(true);
			pMaterial->GetSurfaceData()->PreloadData();
		}

		return pMaterial;
	}

	//--------------------------------------------------------------

	cPhysicsMaterialIterator iPhysicsWorld::GetMaterialIterator()
	{
		return cPhysicsMaterialIterator(&m_mapMaterials);
	}

	//--------------------------------------------------------------

	void iPhysicsWorld::DestroyBody(iPhysicsBody *a_pBody)
	{
		tPhysicsBodyListIt it = m_lstBodies.begin();
		for (; it != m_lstBodies.end(); ++it)
		{
			iPhysicsBody *pBody = *it;
			if (pBody == a_pBody)
			{
				if (m_pWorld3D) m_pWorld3D->GetPortalContainer()->RemoveEntity(pBody);
				pBody->Destroy();
				efeDelete(pBody);
				m_lstBodies.erase(it);
				return;
			}
		}
	}

	cPhysicsBodyIterator iPhysicsWorld::GetBodyIterator()
	{
		return cPhysicsBodyIterator(&m_lstBodies);
	}

	//--------------------------------------------------------------

	void iPhysicsWorld::EnableBodiesInBV(cBoundingVolume *a_pBV, bool a_bEnabled)
	{
		tPhysicsBodyListIt BodyIt = m_lstBodies.begin();
		for (; BodyIt != m_lstBodies.end(); ++BodyIt)
		{
			iPhysicsBody *pBody = *BodyIt;

			if (pBody->GetMass() > 0 && cMath::CheckCollisionBV(*a_pBV, *pBody->GetBV()))
			{
				pBody->SetEnabled(a_bEnabled);
			}
		}
	}

	//--------------------------------------------------------------

	void iPhysicsWorld::DestroyController(iPhysicsController *a_pController)
	{
		STLFindAndDelete(m_lstControllers, a_pController);
	}

	//--------------------------------------------------------------

	bool iPhysicsWorld::CheckShapeWorldCollision(cVector3f *a_pNewPos,
			iCollideShape *a_pShape, const cMatrixf &a_mtxTransform,
			iPhysicsBody *a_pSkipBody, bool a_bSkipStatic,
			bool a_bIsCharacter,
			iPhysicsWorldCollisionCallback *a_pCallback,
			bool a_bCollideCharacter,
			bool a_bDebug)
	{
		cCollideData collideData;

		cVector3f vPushVec(0,0,0);
		bool bCollide = false;

		cBoundingVolume boundingVolume = a_pShape->GetBoundingVolume();
		boundingVolume.SetTransform(cMath::MatrixMul(a_mtxTransform, boundingVolume.GetTransform()));

		cPortalContainerEntityIterator entIt = m_pWorld3D->GetPortalContainer()->GetEntityIterator(&boundingVolume);
		while (entIt.HasNext())
		{
			iPhysicsBody *pBody = static_cast<iPhysicsBody*>(entIt.Next());

			if (pBody->IsCharacter() && a_bCollideCharacter == false) continue;
			if (pBody->IsActive() == false) continue;
			if (pBody == a_pSkipBody) continue;
			if (a_bSkipStatic && pBody->GetMass() == 0) continue;
			if (a_bIsCharacter && pBody->GetCollideCharacter() == false) continue;
			if (a_bIsCharacter == false && pBody->GetCollide() == false) continue;

			if (cMath::CheckCollisionBV(boundingVolume, *pBody->GetBV()) == false)
				continue;

			collideData.SetMaxSize(32);
			bool bRet = CheckShapeCollision(a_pShape, a_mtxTransform,
				pBody->GetShape(), pBody->GetLocalMatrix(),
				collideData, 32);

			if (bRet)
			{
				if (a_pCallback) a_pCallback->OnCollision(pBody, &collideData);

				for (int i=0; i<collideData.m_lNumOfPoints; i++)
				{
					
					cCollidePoint &point = collideData.m_vContactPoints[i];

					int mul = 1;
					cVector3f iv = point.m_vPoint - boundingVolume.GetWorldCenter();
					iv.Normalize();
					if (cMath::Vector3Dot(iv, point.m_vNormal) > 0)
						mul = -1;

					cVector3f vPush = point.m_vNormal * (point.m_fDepth * mul);
					if (std::abs(vPushVec.x) < std::abs(vPush.x)) vPushVec.x = vPush.x;
					if (std::abs(vPushVec.y) < std::abs(vPush.y)) vPushVec.y = vPush.y;
					if (std::abs(vPushVec.z) < std::abs(vPush.z)) vPushVec.z = vPush.z;
				}
				bCollide = true;
			}
		}

		if (a_pNewPos)
			*a_pNewPos = a_mtxTransform.GetTranslation() + vPushVec;
		return bCollide;
	}

	//--------------------------------------------------------------

	void iPhysicsWorld::DestroyAll()
	{
		STLDeleteAll(m_lstCharBodies);

		//Bodies
		tPhysicsBodyListIt it = m_lstBodies.begin();
		for (; it != m_lstBodies.end(); ++it)
		{
			iPhysicsBody *pBody = *it;
			pBody->Destroy();
			efeDelete(pBody);
		}
		m_lstBodies.clear();

		STLDeleteAll(m_lstShapes);
		STLDeleteAll(m_lstJoints);
		STLDeleteAll(m_lstControllers);
		STLMapDeleteAll(m_mapMaterials);
	}
};