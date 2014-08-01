#ifndef EFE_MESH_H
#define EFE_MESH_H

#include <vector>
#include <map>

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "resources/ResourceBase.h"

#include "physics/CollideShape.h"
#include "physics/PhysicsJoint.h"

#include "scene/Light3D.h"

namespace efe
{
	class cMaterialManager;
	class cAnimationManager;
	class cSubMesh;
	class cMeshEntity;
	class cSkeleton;
	class cAnimation;
	class cNode3D;
	class iCollideShape;
	class iPhysicsWorld;
	class iPhysicsBody;
	class iPhysicsJoint;
	class cBillboard;
	class cBeam;
	class cParticleSystem3D;
	class cSoundEntity;
	class cWorld3D;

	typedef std::vector<cAnimation*> tAnimationVec;
	typedef tAnimationVec::iterator tAnimationVecIt;

	typedef std::map<tString, int> tAnimationIndexMap;
	typedef tAnimationIndexMap::iterator tAnimationIndexMapIt;

	typedef std::vector<cSubMesh*> tSubMeshVec;
	typedef tSubMeshVec::iterator tSubMeshVecIt;

	typedef std::map<tString, cSubMesh*> tSubMeshMap;
	typedef tSubMeshMap::iterator tSubMeshMapIt;

	typedef std::vector<cNode3D*> tNode3DVec;
	typedef tNode3DVec::iterator tNode3DVecIt;

	class cMeshCollider
	{
	public:
		tString m_sGroup;

		eCollideShapeType m_Type;
		cVector3f m_vSize;
		cMatrixf m_mtxOffset;
	};

	typedef std::vector<cMeshCollider*> tMeshColliderVec;
	typedef tMeshColliderVec::iterator tMeshColliderVecIt;

	class cMeshJoint
	{
	public:
		ePhysicsJointType m_Type;

		tString m_sName;

		bool m_bCollide;

		cVector3f m_vPivot;
		cVector3f m_vPinDir;

		float m_fMax;
		float m_fMin;

		tString m_sChildBody;
		tString m_sParentBody;
	};

	typedef std::vector<cMeshJoint*> tMeshJointVec;
	typedef tMeshJointVec::iterator tMeshJointVecIt;

	class cMeshReference
	{
	public:
		tString m_sParent;
		tString m_sName;
		tString m_sFile;

		cMatrixf m_mtxTransform;
	};

	typedef std::vector<cMeshReference*> tMeshReferenceVec;
	typedef tMeshReferenceVec::iterator tMeshReferenceVecIt;

	class cMesh : public iResourceBase
	{
		friend class cSubMesh;
		friend class cMeshEntity;
	public:
		cMesh(const tString a_sName, cMaterialManager *a_pMaterialManager,
				cAnimationManager *a_pAnimationManager);
		~cMesh();

		bool CreateFromFile(const tString a_sFile);

		cSubMesh *CreateSubMesh(const tString &a_sName);

		cSubMesh *GetSubMesh(unsigned int a_lIdx);
		cSubMesh *GetSubMeshName(const tString &a_sName);
		int GetSubMeshNum();

		void SetSkeleton(cSkeleton *a_pSkeleton);
		cSkeleton *GetSkeleton();

		void AddAnimation(cAnimation *a_pAnimation);

		cAnimation *GetAnimation(int a_lIndex);
		cAnimation *GetAnimationFromName(const tString &a_sName);
		int GetAnimationIndex(const tString &a_sName);

		void ClearAnimations(bool a_bDeleteAll);

		int GetAnimationNum();

		void SetupBones();

		//Joints
		cMeshJoint *GetPhysicsJoint(int a_lIdx);
		int GetPhysicsJointNum();
		iPhysicsJoint *CreateJointInWorld(const tString &a_sNamePrefix, cMeshJoint *a_pMeshJoint,
			iPhysicsBody *a_pParentBody, iPhysicsBody *a_pChildBody,
			const cMatrixf &a_mtxOffset, iPhysicsWorld *a_pWorld);

		void CreateNodeBodies(iPhysicsBody **a_pRootBodyPtr, std::vector<iPhysicsBody*> *a_pBodyVec,
			cMeshEntity *a_pEntity, iPhysicsWorld *a_pPhysicsWorld,
			const cMatrixf &a_mtxTransform);

		void CreateJointsAndBodies(std::vector<iPhysicsBody*> *a_pBodyVec, cMeshEntity *a_pEntity,
			std::vector<iPhysicsJoint*> *a_pJointVec,
			const cMatrixf &a_mtxOffset, iPhysicsWorld *a_pWorld);

		bool HasSeveralBodies();

		//Coliders
		cMeshCollider *GetCollider(int a_lIdx);
		int GetColliderNum();
		iCollideShape *CreateCollideShape(iPhysicsWorld *a_pWorld);
		iCollideShape *CreateCollideShapeFromCollider(cMeshCollider *a_pCollider, iPhysicsWorld *a_pWorld);

		//References
		cMeshReference *CreateReference();
		cMeshReference *GetReference(int a_lIdx);
		int GetReferenceNum();
		iEntity3D *CreateReferenceInWorld(const tString &a_sNamePrefix, cMeshReference *a_pMeshRef,
			cMeshEntity *a_pMeshEntity, cWorld3D *a_pWorld,
			const cMatrixf &a_mtxOffset);

		//Node
		cNode3D *GetRootNode();
		void AddNode(cNode3D *a_pNode);
		int GetNodeNum();
		cNode3D *GetNode(int a_lIdx);

		bool Reload(){return false;}
		void Unload(){}
		void Destroy(){}
	private:
		cMaterialManager *m_pMaterialManager;
		cAnimationManager *m_pAnimationManager;

		tSubMeshVec m_vSubMeshes;
		tSubMeshMap m_mapSubMeshes;

		cSkeleton *m_pSkeleton;

		tAnimationVec m_vAnimations;
		tAnimationIndexMap m_mapAnimIndeces;

		cNode3D *m_pRootNode;
		tNode3DVec m_vNodes;

		tMeshJointVec m_vPhysicJoints;
		tMeshColliderVec m_vColliders;
		tMeshReferenceVec m_vReferences;
	};
};

#endif