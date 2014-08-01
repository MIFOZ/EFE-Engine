#include "impl/PhysicsWorldPx.h"

#include "impl/CollideShapePx.h"
#include "impl/PhysicsBodyPx.h"
#include "impl/PhysicsMaterialPx.h"
#include "impl/CharacterBodyPx.h"

#include "scene/World3D.h"
#include "scene/PortalContainer.h"

#include "system/LowLevelSystem.h"
#include "graphics/VertexBuffer.h"
#include "graphics/LowLevelGraphics.h"
#include "math/Math.h"

#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxExtensionsAPI.h>

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	

	cPhysicsWorldPx::cPhysicsWorldPx()
		: iPhysicsWorld()
	{
		static PxDefaultAllocator g_DefaultAllocatorCallback;
		static PxDefaultErrorCallback g_DefaultErrorCallback;

		m_pPxSDK = PxCreatePhysics(PX_PHYSICS_VERSION, g_DefaultAllocatorCallback, g_DefaultErrorCallback,
			PxTolerancesScale(), false);

		PxInitExtensions(*m_pPxSDK);

		PxSceneDesc desc(m_pPxSDK->getTolerancesScale());
		desc.flags |= PxSceneFlag::eENABLE_SWEPT_INTEGRATION;
		desc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
		desc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
		if (!desc.cpuDispatcher)
		{
			FatalError("Couldn't create default Cpu dispatcher!\n"); 
		}/**/

		//desc.simulationEventCallback

		desc.filterShader = PxDefaultSimulationFilterShader;

		m_pPxScene = m_pPxSDK->createScene(desc);

		m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, &m_pPxSDK->getFoundation(), PxCookingParams());

		//Create default material
		int lDefaultMatId = 0;
		cPhysicsMaterialPx *pMaterial = efeNew(cPhysicsMaterialPx, ("Default", this, lDefaultMatId));
		tPhysicsMaterialMap::value_type Val("Default", pMaterial);
		m_mapMaterials.insert(Val);


		//m_pPxScene->setSimulationEventCallback(
		m_pTempDepths = efeNewArray(float, 500);
		m_pTempNormals = efeNewArray(float, 500 * 3);
		m_pTempPoints = efeNewArray(float, 500 * 3);
	}

	//-------------------------------------------------------------

	cPhysicsWorldPx::~cPhysicsWorldPx()
	{
		DestroyAll();
		m_pPxScene->release();
		m_pPxSDK->release();

		efeDeleteArray(m_pTempDepths);
		efeDeleteArray(m_pTempNormals);
		efeDeleteArray(m_pTempPoints);
	}

	//-------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	void cPhysicsWorldPx::Simulate(float a_fTimeStep)
	{
		while(a_fTimeStep>m_fMaxTimeStep)
		{
			m_pPxScene->simulate(m_fMaxTimeStep);
			a_fTimeStep -= m_fMaxTimeStep;
		}
		m_pPxScene->simulate(a_fTimeStep);
		m_pPxScene->fetchResults();

		tPhysicsBodyListIt it = m_lstBodies.begin();
		for (; it != m_lstBodies.end(); ++it)
		{
			cPhysicsBodyPx *pBody = static_cast<cPhysicsBodyPx*>(*it);
			pBody->ClearForces();
		}
	}

	//-------------------------------------------------------------

	void cPhysicsWorldPx::SetMaxStepSize(float a_fTimeStep)
	{
		m_fMaxTimeStep = a_fTimeStep;
	}

	//-------------------------------------------------------------

	void cPhysicsWorldPx::SetWorldSize(const cVector3f &a_vMin, const cVector3f &a_vMax)
	{
		m_vWorldSizeMin = a_vMin;
		m_vWorldSizeMax = a_vMax;

		//m_pPxScene->
	}

	//-------------------------------------------------------------

	iCollideShape *cPhysicsWorldPx::CreateSphereShape(const cVector3f &a_vRadii, cMatrixf *a_pOffsetMtx)
	{
		iCollideShape *pShape = efeNew(cCollideShapePx, (eCollideShapeType_Sphere, a_vRadii, a_pOffsetMtx,
															m_pPxScene, this));
		m_lstShapes.push_back(pShape);

		return pShape;
	}

	//-------------------------------------------------------------

	iCollideShape *cPhysicsWorldPx::CreateCylinderShape(float a_fRadius, float a_fHeight, cMatrixf *a_pOffsetMtx)
	{
		iCollideShape *pShape = efeNew(cCollideShapePx, (eCollideShapeType_Capsule,
															cVector3f(a_fRadius, a_fHeight-a_fRadius*2, a_fRadius),
															a_pOffsetMtx,
															m_pPxScene, this));
		m_lstShapes.push_back(pShape);

		return pShape;
	}

	//-------------------------------------------------------------

	iCollideShape *cPhysicsWorldPx::CreateCapsuleShape(float a_fRadius, float a_fHeight, cMatrixf *a_pOffsetMtx)
	{
		iCollideShape *pShape = efeNew(cCollideShapePx, (eCollideShapeType_Capsule,
															cVector3f(a_fRadius, a_fHeight, a_fRadius),
															a_pOffsetMtx,
															m_pPxScene, this));
		m_lstShapes.push_back(pShape);

		return pShape;
	}

	//-------------------------------------------------------------

	iCollideShape *cPhysicsWorldPx::CreateMeshShape(iVertexBuffer *a_pVtxBuffer)
	{
		cCollideShapePx *pShape = efeNew(cCollideShapePx, (eCollideShapeType_Mesh,
			0, NULL, m_pPxScene, this));

		pShape->CreateFromVertices(a_pVtxBuffer->GetIndices(), a_pVtxBuffer->GetIndexNum(),
			a_pVtxBuffer->GetArray(eVertexFlag_Position),
			kvVertexElements[cMath::Log2ToInt(eVertexFlag_Position)],
			a_pVtxBuffer->GetVertexNum());
		m_lstShapes.push_back(pShape);

		return pShape;
	}

	//-------------------------------------------------------------

	iPhysicsBody *cPhysicsWorldPx::CreateBody(const tString &a_sName, iCollideShape *a_pShape)
	{
		cPhysicsBodyPx *pBody = efeNew(cPhysicsBodyPx, (a_sName, this, a_pShape));

		m_lstBodies.push_back(pBody);

		if (m_pWorld3D)m_pWorld3D->GetPortalContainer()->AddEntity(pBody);

		return pBody;
	}

	//-------------------------------------------------------------

	iCharacterBody *cPhysicsWorldPx::CreateCharacterBody(const tString &a_sName, const cVector3f &a_vSize)
	{
		cCharacterBodyPx *pChar = efeNew(cCharacterBodyPx, (a_sName, this, a_vSize));

		m_lstCharBodies.push_back(pChar);

		return pChar;
	}

	//-------------------------------------------------------------

	iPhysicsMaterial *cPhysicsWorldPx::CreateMaterial(const tString &a_sName)
	{
		cPhysicsMaterialPx *pMaterial = efeNew(cPhysicsMaterialPx, (a_sName, this));

		tPhysicsMaterialMap::value_type Val(a_sName, pMaterial);
		m_mapMaterials.insert(Val);

		pMaterial->UpdateMaterials();

		return pMaterial;
	}

	void cPhysicsWorldPx::CastRay(iPhysicsRayCallback *a_pCallback,
							const cVector3f &a_vOrigin,	const cVector3f &a_vEnd,
							bool a_bCalcDist, bool a_bCalcNorm, bool a_bCalcPoint,
							bool a_bUsePrefilter)
	{
		

		//m_pPxScene->r
	}
}