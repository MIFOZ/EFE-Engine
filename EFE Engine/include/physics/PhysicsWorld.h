#ifndef EFE_PHYSICS_WORLD_H
#define EFE_PHYSICS_WORLD_H

#include <map>
#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

#include "physics/CollideData.h"

#include "game/SaveGame.h"

namespace efe
{
	class iCollideShape;
	class iVertexBuffer;
	class iPhysicsBody;
	class iLowLevelGraphics;
	class iPhysicsMaterial;
	class iCharacterBody;
	class iPhysicsJoint;
	class iPhysicsJointBall;
	class iPhysicsJointHinge;
	class iPhysicsJointScrew;
	class iPhysicsJointSlider;
	class iPhysicsController;
	class cWorld3D;
	class cBoundingVolume;

	typedef std::list<iCollideShape*> tCollideShapeList;
	typedef tCollideShapeList::iterator tCollideShapeListIt;

	typedef std::vector<iCollideShape*> tCollideShapeVec;
	typedef tCollideShapeVec::iterator tCollideShapeVecIt;

	typedef std::list<iPhysicsBody*> tPhysicsBodyList;
	typedef tPhysicsBodyList::iterator tPhysicsBodyListIt;

	typedef std::list<iPhysicsJoint*> tPhysicsJointList;
	typedef tPhysicsJointList::iterator tPhysicsJointListIt;

	typedef std::list<iPhysicsController*> tPhysicsControllerList;
	typedef tPhysicsControllerList::iterator tPhysicsControllerListIt;

	typedef std::list<iCharacterBody*> tCharacterBodyList;
	typedef tCharacterBodyList::iterator tCharacterBodyListIt;

	typedef std::map<tString, iPhysicsMaterial*> tPhysicsMaterialMap;
	typedef tPhysicsMaterialMap::iterator tPhysicsMaterialMapIt;

	typedef cSTLMapIterator<iPhysicsMaterial*, tPhysicsMaterialMap, tPhysicsMaterialMapIt> cPhysicsMaterialIterator;

	typedef cSTLIterator<iPhysicsBody*, tPhysicsBodyList, tPhysicsBodyListIt> cPhysicsBodyIterator;
	typedef cSTLIterator<iPhysicsJoint*, tPhysicsJointList, tPhysicsJointListIt> cPhysicsJointIterator;

	enum ePhysicsAccuracy
	{
		ePhysicsAccuracy_Low,
		ePhysicsAccuracy_Medium,
		ePhysicsAccuracy_High,
		ePhysicsAccuracy_LastEnum
	};

	//--------------------------------------------------------------

	class cPhysicsRayParams
	{
	public:
		float m_fT;
		float m_fDist;
		cVector3f m_vNormal;
		cVector3f m_vPoint;
	};

	class iPhysicsRayCallback
	{
	public:
		virtual bool BeforeIntersect(iPhysicsBody *a_pBody){return true;}
		virtual bool OnIntersect(iPhysicsBody *a_pBody, cPhysicsRayParams *a_pParams)=0;
	};

	class iPhysicsWorldCollisionCallback
	{
	public:
		virtual void OnCollision(iPhysicsBody *a_pBody, cCollideData *a_pCollideData)=0;
	};

	class iPhysicsWorld
	{
	public:
		iPhysicsWorld();
		virtual ~iPhysicsWorld();

		void Update(float a_fTimeStep);
		virtual void Simulate(float a_fTimeStep)=0;

		virtual void SetMaxStepSize(float a_fTimeStep)=0;
		virtual float GetMaxStepSize()=0;

		virtual void SetWorldSize(const cVector3f &a_vMin, const cVector3f &a_vMax)=0;
		virtual cVector3f GetWorldSizeMin()=0;
		virtual cVector3f GetWorldSizeMax()=0;

		virtual void SetGravity(const cVector3f &a_vGravity)=0;
		virtual cVector3f GetGravity()=0;

		virtual void SetAccuracyLevel(ePhysicsAccuracy a_Accuracy)=0;
		virtual ePhysicsAccuracy GetAccuracyLevel()=0;

		virtual iCollideShape *CreateNullShape()=0;
		virtual iCollideShape *CreateBoxShape(const cVector3f &a_vSize, cMatrixf *a_pOffsetMtx)=0;
		virtual iCollideShape *CreateSphereShape(const cVector3f &a_vRadii, cMatrixf *a_pOffsetMtx)=0;
		virtual iCollideShape *CreateCylinderShape(float a_fRadius, float a_fHeight, cMatrixf *a_pOffsetMtx)=0;
		virtual iCollideShape *CreateCapsuleShape(float a_fRadius, float a_fHeight, cMatrixf *a_pOffsetMtx)=0;
		virtual iCollideShape *CreateMeshShape(iVertexBuffer *a_pVtxBuffer)=0;
		virtual iCollideShape *CreateCompoundShape(tCollideShapeVec &a_vShapes)=0;
		void DestroyShape(iCollideShape *a_pShape);

		virtual iPhysicsJointBall *CreateJointBall(const tString &a_sName, 
			const cVector3f &a_vPivotPoint,
			iPhysicsBody *a_pParentBody, iPhysicsBody *a_pChildBody) = 0;

		void DestroyJoint(iPhysicsJoint *a_pJoint);
		cPhysicsJointIterator GetJointIterator();

		iPhysicsMaterial *GetMaterialFromName(const tString &a_sName);
		cPhysicsMaterialIterator GetMaterialIterator();

		virtual iPhysicsMaterial *CreateMaterial(const tString &a_sName)=0;
		
		virtual iPhysicsBody *CreateBody(const tString &a_sName, iCollideShape *a_pShape)=0;
		void DestroyBody(iPhysicsBody *a_pBody);
		cPhysicsBodyIterator GetBodyIterator();

		virtual iCharacterBody *CreateCharacterBody(const tString &a_sName, const cVector3f &a_vSize)=0;

		void EnableBodiesInBV(cBoundingVolume *a_pBV, bool a_bEnabled);

		virtual iPhysicsController *CreateController(const tString &a_sName) = 0;
		void DestroyController(iPhysicsController *a_pController);

		tCollidePointVec *GetContactPoints() {return &m_vContactPoints;}
		bool GetSaveContactPoints() {return m_bSaveContactPoints;}

		virtual void CastRay(iPhysicsRayCallback *a_pCallback,
							const cVector3f &a_vOrigin,	const cVector3f &a_vEnd,
							bool a_bCalcDist, bool a_bCalcNorm, bool a_bCalcPoint,
							bool a_bUsePrefilter=false)=0;
		
		virtual void RenderDebugGeometry(iLowLevelGraphics *a_pLowLevel, const cColor &a_Color)=0;

		virtual bool CheckShapeCollision(iCollideShape *a_pShapeA, const cMatrixf &a_mtxA,
			iCollideShape *a_pShapeB, const cMatrixf &a_mtxB,
			cCollideData &a_CollideData, int a_lMaxPoints=4)=0;

		bool CheckShapeWorldCollision(cVector3f *a_pNewPos,
			iCollideShape *a_pShape, const cMatrixf &a_mtxTransform,
			iPhysicsBody *a_pSkipBody=NULL, bool a_bSkipStatic=false,
			bool a_bIsCharacter=false,
			iPhysicsWorldCollisionCallback *a_pCallback=NULL,
			bool a_bCollideCharacter=true,
			bool a_bDebug=false);

		void DestroyAll();

		cWorld3D *GetWorld3D(){return m_pWorld3D;}
		void SetWorld3D(cWorld3D *a_pWorld3D){m_pWorld3D = a_pWorld3D;}

	protected:
		tCollideShapeList m_lstShapes;
		tPhysicsBodyList m_lstBodies;
		tCharacterBodyList m_lstCharBodies;
		tPhysicsMaterialMap m_mapMaterials;
		tPhysicsJointList m_lstJoints;
		tPhysicsControllerList m_lstControllers;
		cWorld3D *m_pWorld3D;

		bool m_bLogDebug;

		tCollidePointVec m_vContactPoints;
		bool m_bSaveContactPoints;
	};
};
#endif