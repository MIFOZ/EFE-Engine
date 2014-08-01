#ifndef EFE_PHYSICS_WORLD_NEWTON_H
#define EFE_PHYSICS_WORLD_NEWTON_H

#include "physics/PhysicsWorld.h"

#include <Newton.h>

namespace efe
{
	class cPhysicsWorldNewton : public iPhysicsWorld
	{
	public:
		cPhysicsWorldNewton();
		virtual ~cPhysicsWorldNewton();

		void Simulate(float a_fTimeStep);

		void SetMaxStepSize(float a_fTimeStep);
		float GetMaxStepSize();

		void SetWorldSize(const cVector3f &a_vMin, const cVector3f &a_vMax);
		cVector3f GetWorldSizeMin();
		cVector3f GetWorldSizeMax();

		void SetGravity(const cVector3f &a_vGravity);
		cVector3f GetGravity();

		void SetAccuracyLevel(ePhysicsAccuracy a_Accuracy);
		ePhysicsAccuracy GetAccuracyLevel();

		iCollideShape *CreateNullShape();
		iCollideShape *CreateBoxShape(const cVector3f &a_vSize, cMatrixf *a_pOffsetMtx);
		iCollideShape *CreateSphereShape(const cVector3f &a_vRadii, cMatrixf *a_pOffsetMtx);
		iCollideShape *CreateCylinderShape(float a_fRadius, float a_fHeight, cMatrixf *a_pOffsetMtx);
		iCollideShape *CreateCapsuleShape(float a_fRadius, float a_fHeight, cMatrixf *a_pOffsetMtx);
		iCollideShape *CreateMeshShape(iVertexBuffer *a_pVtxBuffer);
		iCollideShape *CreateCompoundShape(tCollideShapeVec &a_vShapes);

		iPhysicsJointBall *CreateJointBall(const tString &a_sName, 
			const cVector3f &a_vPivotPoint,
			iPhysicsBody *a_pParentBody, iPhysicsBody *a_pChildBody);

		iPhysicsBody *CreateBody(const tString &a_sName, iCollideShape *a_pShape);

		iCharacterBody *CreateCharacterBody(const tString &a_sName, const cVector3f &a_vSize);

		iPhysicsMaterial *CreateMaterial(const tString &a_sName);

		iPhysicsController *CreateController(const tString &a_sName);

		void CastRay(iPhysicsRayCallback *a_pCallback,
			const cVector3f &a_vOrigin,	const cVector3f &a_vEnd,
			bool a_bCalcDist, bool a_bCalcNorm, bool a_bCalcPoint,
			bool a_bUsePrefilter=false);

		void RenderDebugGeometry(iLowLevelGraphics *a_pLowLevel, const cColor &a_Color);

		bool CheckShapeCollision(iCollideShape *a_pShapeA, const cMatrixf &a_mtxA,
			iCollideShape *a_pShapeB, const cMatrixf &a_mtxB,
			cCollideData &a_CollideData, int a_lMaxPoints=4);

		const NewtonWorld *GetNewtonWorld() {return m_pNewtonWorld;}

	private:
		const NewtonWorld *m_pNewtonWorld;

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