#include "impl/PhysicsWorldNewton.h"

#include "impl/CollideShapeNewton.h"
#include "impl/PhysicsBodyNewton.h"
#include "impl/PhysicsMaterialNewton.h"
#include "impl/CharacterBodyNewton.h"

#include "impl/PhysicsJointBallNewton.h"

#include "impl/PhysicsControllerNewton.h"

#include "scene/World3D.h"
#include "scene/PortalContainer.h"

#include "system/LowLevelSystem.h"
#include "graphics/VertexBuffer.h"
#include "graphics/LowLevelGraphics.h"
#include "math/Math.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPhysicsWorldNewton::cPhysicsWorldNewton()
		: iPhysicsWorld()
	{
		m_pNewtonWorld = NewtonCreate();

		if (m_pNewtonWorld == NULL)
			Warning ("Couldn't create newton world!\n");

		m_vWorldSizeMin = cVector3f(0,0,0);
		m_vWorldSizeMax = cVector3f(0,0,0);

		m_vGravity = cVector3f(0,-9.81f,0);
		m_fMaxTimeStep = 1.0f/60.0f;

		// Create default material
		int lDefaultMatId = 0;
		cPhysicsMaterialNewton *pMaterial = efeNew(cPhysicsMaterialNewton, ("Default", this, lDefaultMatId));
		tPhysicsMaterialMap::value_type Val("Default", pMaterial);
		m_mapMaterials.insert(Val);
		pMaterial->UpdateMaterials();

		m_pTempDepths = efeNewArray(float, 500);
		m_pTempNormals = efeNewArray(float, 500 * 3);
		m_pTempPoints = efeNewArray(float, 500 * 3);
	}

	//--------------------------------------------------------------

	cPhysicsWorldNewton::~cPhysicsWorldNewton()
	{
		DestroyAll();
		NewtonDestroy(m_pNewtonWorld);

		efeDeleteArray(m_pTempDepths);
		efeDeleteArray(m_pTempNormals);
		efeDeleteArray(m_pTempPoints);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cPhysicsWorldNewton::Simulate(float a_fTimeStep)
	{
		while (a_fTimeStep > m_fMaxTimeStep)
		{
			NewtonUpdate(m_pNewtonWorld, m_fMaxTimeStep);
			a_fTimeStep -= m_fMaxTimeStep;
		}
		NewtonUpdate(m_pNewtonWorld, a_fTimeStep);

		tPhysicsBodyListIt it = m_lstBodies.begin();
		for (; it != m_lstBodies.end(); ++it)
		{
			cPhysicsBodyNewton *pBody = static_cast<cPhysicsBodyNewton*>(*it);
			pBody->ClearForces();
		}
	}

	//--------------------------------------------------------------

	void cPhysicsWorldNewton::SetMaxStepSize(float a_fTimeStep)
	{
		m_fMaxTimeStep = a_fTimeStep;
	}

	float cPhysicsWorldNewton::GetMaxStepSize()
	{
		return m_fMaxTimeStep;
	}

	//--------------------------------------------------------------

	void cPhysicsWorldNewton::SetWorldSize(const cVector3f &a_vMin, const cVector3f &a_vMax)
	{
		m_vWorldSizeMin = a_vMin;
		m_vWorldSizeMax = a_vMax;
	}

	cVector3f cPhysicsWorldNewton::GetWorldSizeMin()
	{
		return m_vWorldSizeMin;
	}

	cVector3f cPhysicsWorldNewton::GetWorldSizeMax()
	{
		return m_vWorldSizeMax;
	}

	//--------------------------------------------------------------

	void cPhysicsWorldNewton::SetGravity(const cVector3f &a_vGravity)
	{
		m_vGravity = a_vGravity;
	}

	//--------------------------------------------------------------

	cVector3f cPhysicsWorldNewton::GetGravity()
	{
		return m_vGravity;
	}

	//--------------------------------------------------------------

	void cPhysicsWorldNewton::SetAccuracyLevel(ePhysicsAccuracy a_Accuracy)
	{
		m_Accuracy = a_Accuracy;

		switch(m_Accuracy)
		{
		case ePhysicsAccuracy_Low:
			NewtonSetSolverModel(m_pNewtonWorld, 4);
			NewtonSetFrictionModel(m_pNewtonWorld, 1);
			Log("SETTING LOW!\n");
			break;
		case ePhysicsAccuracy_Medium:
			NewtonSetSolverModel(m_pNewtonWorld, 8);
			NewtonSetFrictionModel(m_pNewtonWorld, 1);
			break;
		case ePhysicsAccuracy_High:
			NewtonSetSolverModel(m_pNewtonWorld, 0);
			NewtonSetFrictionModel(m_pNewtonWorld, 0);
			break;
		}
	}

	//--------------------------------------------------------------

	ePhysicsAccuracy cPhysicsWorldNewton::GetAccuracyLevel()
	{
		return m_Accuracy;
	}

	//--------------------------------------------------------------

	iCollideShape *cPhysicsWorldNewton::CreateNullShape()
	{
		iCollideShape *pShape = efeNew(cCollideShapeNewton, (eCollideShapeType_Null, 0, NULL,
			m_pNewtonWorld, this));

		m_lstShapes.push_back(pShape);

		return pShape;
	}

	//--------------------------------------------------------------

	iCollideShape *cPhysicsWorldNewton::CreateBoxShape(const cVector3f &a_vSize, cMatrixf *a_pOffsetMtx)
	{
		iCollideShape *pShape = efeNew(cCollideShapeNewton, (eCollideShapeType_Box, a_vSize, a_pOffsetMtx,
			m_pNewtonWorld, this));

		m_lstShapes.push_back(pShape);

		return pShape;
	}

	//--------------------------------------------------------------

	iCollideShape *cPhysicsWorldNewton::CreateSphereShape(const cVector3f &a_vRadii, cMatrixf *a_pOffsetMtx)
	{
		iCollideShape *pShape = efeNew(cCollideShapeNewton, (eCollideShapeType_Sphere, a_vRadii, a_pOffsetMtx,
			m_pNewtonWorld, this));

		m_lstShapes.push_back(pShape);

		return pShape;
	}

	//--------------------------------------------------------------

	iCollideShape *cPhysicsWorldNewton::CreateCylinderShape(float a_fRadius, float a_fHeight, cMatrixf *a_pOffsetMtx)
	{
		iCollideShape *pShape = efeNew(cCollideShapeNewton, (eCollideShapeType_Cylinder,
			cVector3f(a_fRadius, a_fHeight, a_fRadius), a_pOffsetMtx,
			m_pNewtonWorld, this));

		m_lstShapes.push_back(pShape);

		return pShape;
	}

	//--------------------------------------------------------------

	iCollideShape *cPhysicsWorldNewton::CreateCapsuleShape(float a_fRadius, float a_fHeight, cMatrixf *a_pOffsetMtx)
	{
		iCollideShape *pShape = efeNew(cCollideShapeNewton, (eCollideShapeType_Capsule,
			cVector3f(a_fRadius, a_fHeight, a_fRadius), a_pOffsetMtx,
			m_pNewtonWorld, this));

		m_lstShapes.push_back(pShape);

		return pShape;
	}

	//--------------------------------------------------------------

	iCollideShape *cPhysicsWorldNewton::CreateMeshShape(iVertexBuffer *a_pVtxBuffer)
	{
		cCollideShapeNewton *pShape = efeNew(cCollideShapeNewton, (eCollideShapeType_Mesh, 0, NULL,
			m_pNewtonWorld, this));

		pShape->CreateFromVertices(a_pVtxBuffer->GetIndices(), a_pVtxBuffer->GetIndexNum(),
			a_pVtxBuffer->GetArray(eVertexFlag_Position),
			kvVertexElements[cMath::Log2ToInt(eVertexFlag_Position)],
			a_pVtxBuffer->GetVertexNum());
		m_lstShapes.push_back(pShape);

		return pShape;
	}

	//--------------------------------------------------------------

	iCollideShape *cPhysicsWorldNewton::CreateCompoundShape(tCollideShapeVec &a_vShapes)
	{
		cCollideShapeNewton *pShape = efeNew(cCollideShapeNewton, (eCollideShapeType_Compound, 0, NULL,
			m_pNewtonWorld, this));

		pShape->CreateFromShapeVec(a_vShapes);
		m_lstShapes.push_back(pShape);

		return pShape;
	}

	//--------------------------------------------------------------

	iPhysicsJointBall *cPhysicsWorldNewton::CreateJointBall(const tString &a_sName, 
		const cVector3f &a_vPivotPoint,
		iPhysicsBody *a_pParentBody, iPhysicsBody *a_pChildBody)
	{
		iPhysicsJointBall *pJoint = efeNew(cPhysicsJointBallNewton, (a_sName, a_pParentBody, a_pChildBody, this,
			a_vPivotPoint));

		m_lstJoints.push_back(pJoint);
		return pJoint;
	}

	//--------------------------------------------------------------

	iPhysicsBody *cPhysicsWorldNewton::CreateBody(const tString &a_sName, iCollideShape *a_pShape)
	{
		cPhysicsBodyNewton *pBody = efeNew(cPhysicsBodyNewton, (a_sName, this, a_pShape));

		m_lstBodies.push_back(pBody);

		if (m_pWorld3D) m_pWorld3D->GetPortalContainer()->AddEntity(pBody);

		return pBody;
	}

	//--------------------------------------------------------------

	iCharacterBody *cPhysicsWorldNewton::CreateCharacterBody(const tString &a_sName, const cVector3f &a_vSize)
	{
		cCharacterBodyNewton *pChar = efeNew(cCharacterBodyNewton, (a_sName, this, a_vSize));

		m_lstCharBodies.push_back(pChar);

		return pChar;
	}

	//--------------------------------------------------------------

	iPhysicsMaterial *cPhysicsWorldNewton::CreateMaterial(const tString &a_sName)
	{
		cPhysicsMaterialNewton *pMaterial = efeNew(cPhysicsMaterialNewton, (a_sName, this));

		tPhysicsMaterialMap::value_type Val(a_sName, pMaterial);
		m_mapMaterials.insert(Val);

		pMaterial->UpdateMaterials();

		return pMaterial;
	}

	//--------------------------------------------------------------

	iPhysicsController *cPhysicsWorldNewton::CreateController(const tString &a_sName)
	{
		iPhysicsController *pController = efeNew(iPhysicsController, (a_sName, this));

		m_lstControllers.push_back(pController);

		return pController;
	}

	//--------------------------------------------------------------

	static bool g_bRayCalcDist;
	static bool g_bRayCalcNormal;
	static bool g_bRayCalcPoint;
	static iPhysicsRayCallback *g_pRayCallback;
	static cVector3f g_vRayOrigin;
	static cVector3f g_vRayEnd;
	static cVector3f g_vRayDelta;
	static float g_fRayLength;

	static cPhysicsRayParams g_RayParams;

	static unsigned RayCastPrefilterFunc(const NewtonBody *a_pNewtonBody, const NewtonCollision *a_pCollision,
		void *a_pUserData)
	{
		cPhysicsBodyNewton *pRigidBody = (cPhysicsBodyNewton*) NewtonBodyGetUserData(a_pNewtonBody);
		if (pRigidBody->IsActive() == false) return 0;

		bool bRet = g_pRayCallback->BeforeIntersect(pRigidBody);

		if (bRet) return 1;
		else return 0;
	}

	static float RayCastFilterFunc(const NewtonBody *const a_pNewtonBody, const NewtonCollision *const a_pShapeHit, const float *const a_pHitPoint,
		const float *const a_pNormalVec, long long a_lCollisionID, void *const a_pUserData, float a_fIntersectParam)
	{
		cPhysicsBodyNewton *pRigidBody = (cPhysicsBodyNewton*) NewtonBodyGetUserData(a_pNewtonBody);
		if (pRigidBody->IsActive() == false) return 1;

		g_RayParams.m_fT = a_fIntersectParam;

		if (g_bRayCalcDist)
			g_RayParams.m_fDist = g_fRayLength * a_fIntersectParam;

		if (g_bRayCalcNormal)
			g_RayParams.m_vNormal.FromVec(a_pNormalVec);

		if (g_bRayCalcPoint)
			g_RayParams.m_vPoint = g_vRayOrigin + g_vRayDelta * a_fIntersectParam;

		bool bRet = g_pRayCallback->OnIntersect(pRigidBody, &g_RayParams);

		if (bRet) return 1;
		else return 0;
	}

	void cPhysicsWorldNewton::CastRay(iPhysicsRayCallback *a_pCallback,
		const cVector3f &a_vOrigin,	const cVector3f &a_vEnd,
		bool a_bCalcDist, bool a_bCalcNorm, bool a_bCalcPoint,
		bool a_bUsePrefilter)
	{
		g_bRayCalcDist = a_bCalcDist;
		g_bRayCalcNormal = a_bCalcNorm;
		g_bRayCalcPoint = a_bCalcPoint;

		g_vRayOrigin = a_vOrigin;
		g_vRayEnd = a_vEnd;

		g_vRayDelta = a_vEnd - a_vOrigin;
		g_fRayLength = g_vRayDelta.Length();

		g_pRayCallback = a_pCallback;

		if (a_bUsePrefilter)
			NewtonWorldRayCast(m_pNewtonWorld, a_vOrigin.v, a_vEnd.v, RayCastFilterFunc, NULL, RayCastPrefilterFunc, 0);
		else
			NewtonWorldRayCast(m_pNewtonWorld, a_vOrigin.v, a_vEnd.v, RayCastFilterFunc, NULL, NULL, 0);
	}

	//--------------------------------------------------------------

	void cPhysicsWorldNewton::RenderDebugGeometry(iLowLevelGraphics *a_pLowLevel, const cColor &a_Color)
	{
		tPhysicsBodyListIt it = m_lstBodies.begin();
		for (; it != m_lstBodies.end(); ++it)
		{
			iPhysicsBody *pBody = *it;
			pBody->RenderDebugGeometry(a_pLowLevel, a_Color);
		}
	}

	bool cPhysicsWorldNewton::CheckShapeCollision(iCollideShape *a_pShapeA, const cMatrixf &a_mtxA,
		iCollideShape *a_pShapeB, const cMatrixf &a_mtxB,
		cCollideData &a_CollideData, int a_lMaxPoints)
	{
		cCollideShapeNewton *pNewtonShapeA = static_cast<cCollideShapeNewton*>(a_pShapeA);
		cCollideShapeNewton *pNewtonShapeB = static_cast<cCollideShapeNewton*>(a_pShapeB);

		cMatrixf mtxTransposeA = a_mtxA.GetTranspose();
		cMatrixf mtxTransposeB = a_mtxB.GetTranspose();

		bool bLog = false;

		//Check compound collision
		if (pNewtonShapeA->GetType() == eCollideShapeType_Compound ||
			pNewtonShapeB->GetType() == eCollideShapeType_Compound)
		{
			int lACount = pNewtonShapeA->GetSubShapeNum();
			int lBCount = pNewtonShapeB->GetSubShapeNum();

			bool bCollision = false;
			a_CollideData.m_lNumOfPoints = 0;
			int lCollideDataStart = 0;
			for (int a=0; a<lACount; a++)
			{
				for (int b=0; b<lBCount; b++)
				{
					cCollideShapeNewton *pSubShapeA = static_cast<cCollideShapeNewton*>(pNewtonShapeA->GetSubShape(a));
					cCollideShapeNewton *pSubShapeB = static_cast<cCollideShapeNewton*>(pNewtonShapeB->GetSubShape(b));

					dLong AttrA[50];
					dLong AttrB[50];

					int lNum = NewtonCollisionCollide(m_pNewtonWorld, a_lMaxPoints,
						pSubShapeA->GetNewtonCollision(), &(mtxTransposeA.m[0][0]),
						pSubShapeB->GetNewtonCollision(), &(mtxTransposeB.m[0][0]),
						m_pTempPoints, m_pTempNormals, m_pTempDepths, AttrA, AttrB, 0);

					if (lNum < 1) continue;
					if (lNum > a_lMaxPoints) lNum = a_lMaxPoints;

					bCollision = true;

					a_lMaxPoints -= lNum;

					for (int i=0; i<lNum; i++)
					{
						cCollidePoint &CollPoint = a_CollideData.m_vContactPoints[lCollideDataStart + i];
						CollPoint.m_fDepth = m_pTempDepths[i];

						int lVertex = i*3;

						CollPoint.m_vNormal.x = m_pTempNormals[lVertex+0];
						CollPoint.m_vNormal.y = m_pTempNormals[lVertex+1];
						CollPoint.m_vNormal.z = m_pTempNormals[lVertex+2];

						CollPoint.m_vPoint.x = m_pTempPoints[lVertex+0];
						CollPoint.m_vPoint.y = m_pTempPoints[lVertex+1];
						CollPoint.m_vPoint.z = m_pTempPoints[lVertex+2];
					}

					lCollideDataStart += lNum;
					a_CollideData.m_lNumOfPoints += lNum;

					if (a_lMaxPoints <= 0) break;
				}
				if (a_lMaxPoints <= 0) break;
			}

			return bCollision;
		}
		//Check NON compound collision
		else
		{
			dLong AttrA[50];
			dLong AttrB[50];

			int lNum = NewtonCollisionCollide(m_pNewtonWorld, a_lMaxPoints,
				pNewtonShapeA->GetNewtonCollision(), &(mtxTransposeA.m[0][0]),
				pNewtonShapeB->GetNewtonCollision(), &(mtxTransposeB.m[0][0]),
				m_pTempPoints, m_pTempNormals, m_pTempDepths, AttrA, AttrB, 0);

			if (lNum < 1) return false;
			if (lNum > a_lMaxPoints) lNum = a_lMaxPoints;

			a_lMaxPoints -= lNum;

			for (int i=0; i<lNum; i++)
			{
				cCollidePoint &CollPoint = a_CollideData.m_vContactPoints[i];
				CollPoint.m_fDepth = m_pTempDepths[i];

				int lVertex = i*3;

				CollPoint.m_vNormal.x = m_pTempNormals[lVertex+0];
				CollPoint.m_vNormal.y = m_pTempNormals[lVertex+1];
				CollPoint.m_vNormal.z = m_pTempNormals[lVertex+2];

				CollPoint.m_vPoint.x = m_pTempPoints[lVertex+0];
				CollPoint.m_vPoint.y = m_pTempPoints[lVertex+1];
				CollPoint.m_vPoint.z = m_pTempPoints[lVertex+2];
			}

			a_CollideData.m_lNumOfPoints = lNum;
		}

		return true;
	}
}