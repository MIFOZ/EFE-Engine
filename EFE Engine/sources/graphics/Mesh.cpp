#include "graphics/Mesh.h"

#include "graphics/SubMesh.h"
#include "resources/MaterialManager.h"

#include "graphics/VertexBuffer.h"
#include "graphics/Material.h"
#include "graphics/Skeleton.h"
#include "graphics/Animation.h"

#include "math/Math.h"

#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsJointBall.h"

#include "scene/Node3D.h"
#include "scene/MeshEntity.h"


#include "scene/World3D.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMesh::cMesh(const tString a_sName, cMaterialManager *a_pMaterialManager,
				cAnimationManager *a_pAnimationManager)
				: iResourceBase(a_sName, 0)
	{
		m_pMaterialManager = a_pMaterialManager;
		m_pAnimationManager = a_pAnimationManager;
		m_pSkeleton = NULL;

		m_pRootNode = efeNew(cNode3D, ());
	}

	cMesh::~cMesh()
	{
		for (int i=0;i<(int)m_vSubMeshes.size();i++)
			efeDelete(m_vSubMeshes[i]);
		if (m_pSkeleton) efeDelete(m_pSkeleton);

		if (m_pRootNode) efeDelete(m_pRootNode);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSubMesh *cMesh::CreateSubMesh(const tString &a_sName)
	{
		cSubMesh *pSubMesh = efeNew(cSubMesh,(a_sName, m_pMaterialManager));

		pSubMesh->m_pParent = this;

		m_vSubMeshes.push_back(pSubMesh);
		m_mapSubMeshes.insert(tSubMeshMap::value_type(a_sName, pSubMesh));

		return pSubMesh;
	}

	//--------------------------------------------------------------

	cSubMesh *cMesh::GetSubMesh(unsigned int a_lIdx)
	{
		if (a_lIdx >= m_vSubMeshes.size()) return NULL;

		return m_vSubMeshes[a_lIdx];
	}

	cSubMesh *cMesh::GetSubMeshName(const tString &a_sName)
	{
		tSubMeshMapIt it = m_mapSubMeshes.find(a_sName);
		if (it == m_mapSubMeshes.end()) return NULL;

		return it->second;
	}

	int cMesh::GetSubMeshNum()
	{
		return (int)m_vSubMeshes.size();
	}

	//--------------------------------------------------------------

	void cMesh::SetSkeleton(cSkeleton *a_pSkeleton)
	{
		m_pSkeleton = a_pSkeleton;
	}

	cSkeleton *cMesh::GetSkeleton()
	{
		return m_pSkeleton;
	}

	//--------------------------------------------------------------

	void cMesh::AddAnimation(cAnimation *a_pAnimation)
	{
		m_vAnimations.push_back(a_pAnimation);

		tAnimationIndexMap::value_type value(a_pAnimation->GetName(), (int)m_vAnimations.size() - 1);
		m_mapAnimIndeces.insert(value);
	}

	cAnimation *cMesh::GetAnimation(int a_lIndex)
	{
		return m_vAnimations[a_lIndex];
	}

	int cMesh::GetAnimationNum()
	{
		return (int)m_vAnimations.size();
	}

	//--------------------------------------------------------------

	cMeshJoint *cMesh::GetPhysicsJoint(int a_lIdx)
	{
		return m_vPhysicJoints[a_lIdx];
	}

	int cMesh::GetPhysicsJointNum()
	{
		return (int)m_vPhysicJoints.size();
	}

	//--------------------------------------------------------------

	iPhysicsJoint *cMesh::CreateJointInWorld(const tString &a_sNamePrefix, cMeshJoint *a_pMeshJoint,
			iPhysicsBody *a_pParentBody, iPhysicsBody *a_pChildBody,
			const cMatrixf &a_mtxOffset, iPhysicsWorld *a_pWorld)
	{
		cVector3f vPivot = cMath::MatrixMul(a_mtxOffset, a_pMeshJoint->m_vPivot);
		cVector3f vPinDir = cMath::MatrixMul(a_mtxOffset.GetRotation(), a_pMeshJoint->m_vPinDir);

		// Hinge
		if (a_pMeshJoint->m_Type == ePhysicsJointType_Hinge)
		{
			_ASSERT(0);
		}
		else if (a_pMeshJoint->m_Type == ePhysicsJointType_Ball)
		{
			iPhysicsJointBall *pJoint = a_pWorld->CreateJointBall(a_sNamePrefix + a_pMeshJoint->m_sName,
				vPivot, a_pParentBody, a_pChildBody);

			pJoint->SetCollideBodies(a_pMeshJoint->m_bCollide);

			pJoint->SetConeLimits(vPinDir, cMath::ToRad(a_pMeshJoint->m_fMin), cMath::ToRad(a_pMeshJoint->m_fMax));

			return pJoint;
		}

		return NULL;
	}

	//--------------------------------------------------------------

	void cMesh::CreateNodeBodies(iPhysicsBody **a_pRootBodyPtr, std::vector<iPhysicsBody*> *a_pBodyVec,
			cMeshEntity *a_pEntity, iPhysicsWorld *a_pPhysicsWorld,
			const cMatrixf &a_mtxTransform)
	{
		cMatrixf mtxOldOffset;

		for (int sub=0; sub<GetSubMeshNum(); sub++)
		{
			cSubMesh *pSubMesh = GetSubMesh(sub);
			cSubMeshEntity *pSubEntity = a_pEntity->GetSubMeshEntity(sub);

			tCollideShapeVec vShapes;
			for (int shape=0; shape<GetColliderNum(); shape++)
			{
				cMeshCollider *pColl = GetCollider(shape);
				if (pColl->m_sGroup == pSubMesh->GetGroup() && pColl->m_sGroup != "")
				{
					mtxOldOffset = pColl->m_mtxOffset;

					cMatrixf mtxSub = pSubEntity->GetWorldMatrix();
					cMatrixf mtxScale = cMath::MatrixScale(pSubMesh->GetModelScale());
					mtxSub = cMath::MatrixMul(mtxSub, cMath::MatrixInverse(mtxScale));

					pColl->m_mtxOffset = cMath::MatrixMul(cMath::MatrixInverse(mtxSub),
						pColl->m_mtxOffset);

					//Create shape
					iCollideShape *pShape = CreateCollideShapeFromCollider(pColl, a_pPhysicsWorld);
					vShapes.push_back(pShape);

					pColl->m_mtxOffset = mtxOldOffset;
				}
			}

			iCollideShape *pShape;
			if (vShapes.size() > 1)
				pShape = a_pPhysicsWorld->CreateCompoundShape(vShapes);
			else if (vShapes.size() == 1)
				pShape = vShapes[0];
			else
			{
				Warning("No shapes for sub mesh '%s' with group: '%s'\n", pSubMesh->GetName().c_str(),
					pSubMesh->GetGroup().c_str());
				continue;
			}

			iPhysicsBody *pBody = a_pPhysicsWorld->CreateBody(a_pEntity->GetName()+"_"+pSubMesh->GetName(), pShape);
			pBody->SetMass(0);
			pBody->SetGravity(false);

			pSubEntity->SetBody(pBody);
			pSubEntity->SetUpdateBody(true);

			a_pBodyVec->push_back(pBody);
		}

		tCollideShapeVec vShapes;
		for (int shape=0; shape<GetColliderNum(); shape++)
		{
			cMeshCollider *pColl = GetCollider(shape);
			if (pColl->m_sGroup != "")
			{
				iCollideShape *pShape = CreateCollideShapeFromCollider(pColl, a_pPhysicsWorld);
				vShapes.push_back(pShape);


			}
		}

		bool bHasRoot = false;
		iCollideShape *pShape;
		if (vShapes.size() > 1)
		{
			pShape = a_pPhysicsWorld->CreateCompoundShape(vShapes);
			bHasRoot = true;
		}
		else if (vShapes.size() == 1)
		{
			pShape = vShapes[0];
			bHasRoot = true;
		}
		else
			return;

		if (bHasRoot)
		{
			iPhysicsBody *pBody = a_pPhysicsWorld->CreateBody(a_pEntity->GetName(), pShape);

			pBody->CreateNode()->AddEntity(a_pEntity);
			pBody->SetMass(0);

			a_pEntity->SetBody(pBody);

			pBody->SetMatrix(a_mtxTransform);

			a_pBodyVec->push_back(pBody);
		}
	}

	//--------------------------------------------------------------

	bool cMesh::HasSeveralBodies()
	{
		if (GetColliderNum() <= 0) return false;

		tString sPrevGroup = GetCollider(0)->m_sGroup;

		for (int shape = 0; shape < GetColliderNum(); shape++)
		{
			cMeshCollider *pColl = GetCollider(shape);
			if (pColl->m_sGroup != sPrevGroup) return true;
		}

		return false;
	}

	//--------------------------------------------------------------

	cMeshCollider *cMesh::GetCollider(int a_lIdx)
	{
		return m_vColliders[a_lIdx];
	}

	int cMesh::GetColliderNum()
	{
		return (int)m_vColliders.size();
	}

	iCollideShape *cMesh::CreateCollideShape(iPhysicsWorld *a_pWorld)
	{
		if (m_vColliders.empty()) return NULL;

		if (m_vColliders.size() == 1)
			return CreateCollideShapeFromCollider(m_vColliders[0], a_pWorld);
		else
		{
			tCollideShapeVec vShapes;
			vShapes.reserve(m_vColliders.size());

			for (size_t i=0; i<m_vColliders.size(); ++i)
			{
				vShapes.push_back(CreateCollideShapeFromCollider(m_vColliders[i], a_pWorld));
			}

			return a_pWorld->CreateCompoundShape(vShapes);
		}
	}

	iCollideShape *cMesh::CreateCollideShapeFromCollider(cMeshCollider *a_pCollider, iPhysicsWorld *a_pWorld)
	{
		switch(a_pCollider->m_Type)
		{
		case eCollideShapeType_Box:
			return a_pWorld->CreateBoxShape(a_pCollider->m_vSize, &a_pCollider->m_mtxOffset);
		case eCollideShapeType_Sphere:
			return a_pWorld->CreateSphereShape(a_pCollider->m_vSize, &a_pCollider->m_mtxOffset);
		case eCollideShapeType_Cylinder:
			return a_pWorld->CreateCylinderShape(a_pCollider->m_vSize.x, a_pCollider->m_vSize.y, &a_pCollider->m_mtxOffset);
		case eCollideShapeType_Capsule:
			return a_pWorld->CreateCapsuleShape(a_pCollider->m_vSize.x, a_pCollider->m_vSize.y, &a_pCollider->m_mtxOffset);
		}

		return NULL;
	}

	//--------------------------------------------------------------

	cMeshReference *cMesh::CreateReference()
	{
		cMeshReference *pRef = efeNew(cMeshReference, ());
		m_vReferences.push_back(pRef);
		return pRef;
	}

	cMeshReference *cMesh::GetReference(int a_lIdx)
	{
		return m_vReferences[a_lIdx];
	}

	int cMesh::GetReferenceNum()
	{
		return (int)m_vReferences.size();
	}

	iEntity3D *cMesh::CreateReferenceInWorld(const tString &a_sNamePrefix, cMeshReference *a_pMeshRef,
		cMeshEntity *a_pMeshEntity, cWorld3D *a_pWorld,
		const cMatrixf &a_mtxOffset)
	{
		if (a_pMeshRef->m_sParent != "")
		{
			tString sName = a_sNamePrefix + "_" + a_pMeshRef->m_sName;
			iEntity3D *pEntity = a_pWorld->CreateEntity(sName,
				a_pMeshRef->m_mtxTransform, a_pMeshRef->m_sFile, true);

			if (pEntity) a_pMeshEntity->AttachEntityToParent(pEntity, a_pMeshRef->m_sParent);
			return pEntity;
		}
		else
		{
			tString sName = a_sNamePrefix + "_" + a_pMeshRef->m_sParent;
			iEntity3D *pEntity = a_pWorld->CreateEntity(sName,
				cMath::MatrixMul(a_mtxOffset, a_pMeshRef->m_mtxTransform),
				a_pMeshRef->m_sFile, true);

			return pEntity;
		}
	}

	//--------------------------------------------------------------

	cNode3D *cMesh::GetRootNode()
	{
		return m_pRootNode;
	}

	void cMesh::AddNode(cNode3D *a_pNode)
	{
		m_vNodes.push_back(a_pNode);
	}

	int cMesh::GetNodeNum()
	{
		return (int)m_vNodes.size();
	}

	cNode3D *cMesh::GetNode(int a_lIdx)
	{
		return m_vNodes[a_lIdx];
	}
}