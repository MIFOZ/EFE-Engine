#ifndef EFE_PHYSICS_WORLD_PX_H
#define EFE_PHYSICS_WORLD_PX_H

#include "physics/PhysicsWorld.h"

#include <PxPhysicsAPI.h>

namespace efe
{
	using namespace physx;
	class cPhysicsWorldPx : public iPhysicsWorld
	{
	public:
		cPhysicsWorldPx();
		~cPhysicsWorldPx();

		void Simulate(float a_fTimeStep);

		void SetMaxStepSize(float a_fTimeStep);

		void SetWorldSize(const cVector3f &a_vMin, const cVector3f &a_vMax);

		iCollideShape *CreateSphereShape(const cVector3f &a_vRadii, cMatrixf *a_pOffsetMtx);
		iCollideShape *CreateCylinderShape(float a_fRadius, float a_fHeight, cMatrixf *a_pOffsetMtx);
		iCollideShape *CreateCapsuleShape(float a_fRadius, float a_fHeight, cMatrixf *a_pOffsetMtx);
		iCollideShape *CreateMeshShape(iVertexBuffer *a_pVtxBuffer);

		iPhysicsBody *CreateBody(const tString &a_sName, iCollideShape *a_pShape);

		iCharacterBody *CreateCharacterBody(const tString &a_sName, const cVector3f &a_vSize);

		iPhysicsMaterial *CreateMaterial(const tString &a_sName);

		void CastRay(iPhysicsRayCallback *a_pCallback,
							const cVector3f &a_vOrigin,	const cVector3f &a_vEnd,
							bool a_bCalcDist, bool a_bCalcNorm, bool a_bCalcPoint,
							bool a_bUsePrefilter=false);
		
		PxScene *GetPxScene(){return m_pPxScene;}
		PxPhysics *GetPxSDK(){return m_pPxSDK;}
	private:
		PxScene *m_pPxScene;
		PxPhysics *m_pPxSDK;
		PxCooking *m_pCooking;

		float *m_pTempPoints;
		float *m_pTempNormals;
		float *m_pTempDepths;

		cVector3f m_vWorldSizeMin;
		cVector3f m_vWorldSizeMax;
		cVector3f m_vGravity;
		float m_fMaxTimeStep;

		ePhysicsAccuracy m_Accuracy;
	};
};
#endif