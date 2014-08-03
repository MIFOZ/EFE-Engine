#ifndef EFE_MESH_ENTITY_H
#define EFE_MESH_ENTITY_H

#include <Vector>
#include <map>

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "scene/Entity3D.h"
#include "graphics/Renderable.h"

#include "scene/SubMeshEntity.h"

#include "scene/AnimationState.h"

namespace efe
{
	class cMaterialManager;
	class cMeshManager;
	class cAnimationManager;
	class cMesh;
	class cSubMesh;
	class cSubMeshEntity;
	class cAnimation;
	class cNodeState;
	class cBone;
	class cNode3D;
	class iCollideShape;
	class iPhysicsBody;
	class iPhysicsWorld;
	class cWorld3D;

	class cMeshEntityRootNodeUpdate : public iEntityCallback
	{
	public:
		void OnTransformUpdate(iEntity3D *a_pEntity);
	};

	//--------------------------------------------------------------

	class cMeshEntityCallback
	{
	public:
		virtual void AfterAnimationUpdate(cMeshEntity *a_pMeshEntity, float a_fTimeStep)=0;
	};

	//--------------------------------------------------------------

	kSaveData_ChildClass(iRenderable, cMeshEntity)
	{
		kSaveData_ClassInit(cMeshEntity)
	public:
		tString m_sMeshName;

		bool m_bCastShadows;

		int m_lBodyId;

		cContainerVec<cSaveData_cSubMeshEntity> m_vSubEntities;

		virtual iSaveObject *CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler,cGame *a_pGame);
		virtual int GetSaveCreatePrio();
	};

	class cMeshEntity : public iRenderable
	{
		friend class cSubMeshEntity;
		friend class cMeshEntityRootNodeUpdate;
		friend class cMesh;
	public:
		cMeshEntity(const tString a_sName, cMesh *a_pMesh, cMaterialManager *a_pMaterialManager,
			cMeshManager *a_pMeshManager, cAnimationManager *a_pAnimationManager);
		~cMeshEntity();

		void SetWorld(cWorld3D *a_pWorld){m_pWorld = a_pWorld;}
		cWorld3D *GetWorld(){return m_pWorld;}

		void SetCastsShadows(bool a_bX);

		cNode3D *GetRootnode(){return m_pRootNode;}

		cMesh *GetMesh(){return m_pMesh;}

		//Sub mesh entities
		cSubMeshEntity *GetSubMeshEntity(unsigned int a_lIdx);
		cSubMeshEntity *GetSubMeshEntityName(const tString &a_sName);
		int GetSubMeshEntityNum();

		//Animation states
		cAnimationState *AddAnimation(cAnimation *a_pAnimation, const tString &a_sName, float a_fBaseSpeed);
		void ClearAnimations();

		cAnimationState *GetAnimationState(int a_lIndex);
		int GetAnimationStateIndex(const tString &a_sName);
		cAnimationState *GetAnimationStateFromName(const tString &a_sName);
		int GetAnimationStateNum();

		//Animation controller
		void Play(int a_lIndex, bool a_bLoop, bool a_bStopPrev);
		void PlayName(const tString &a_sName, bool a_bLoop, bool a_bStopPrev);
		void Stop();

		//Bone states
		cBoneState *GetBoneState(int a_lIndex);
		int GetBoneStateIndex(const tString &a_sName);
		cBoneState *GetBoneStateFromName(const tString &a_sName);
		int GetBoneStateNum();

		void SetSkeletonPhysicsActive(bool a_bX);
		bool GetSkeletonPhysicsActive();

		void ResetGraphicsUpdated();

		//Node states
		cNode3D *GetNodeState(int a_lIndex);
		int GetNodeStateIndex(const tString &a_sName);
		cNode3D *GetNodeStateFromName(const tString &a_sName);
		int GetNodeStateNum();
		bool HasNodes(){return m_bHasNodes;}

		bool AttachEntityToParent(iEntity3D *a_pEntity, const tString &a_sParent);

		//Entity implementation
		tString GetEntityType(){return "Mesh";}

		void UpdateLogic(float a_fTimeStep);

		//Renderable implementation

		void SetRendered(bool a_bX);
		iMaterial *GetMaterial();
		iVertexBuffer *GetVertexBuffer();
		bool IsShadowCaster();
		cMatrixf *GetModelMatrix(cCamera3D *a_pCamera);
		int GetMatrixUpdateCount();
		eRenderableType GetRenderType();

		void SetBody(iPhysicsBody *a_pBody) {m_pBody = a_pBody;}
		iPhysicsBody *GetBody() {return m_pBody;}

		virtual iSaveData *CreateSaveData(); 
	private:
		void UpdateNodeMatrixRec(const cMatrixf &a_mtxParentWorld, cNode3D *a_pNode);

		void HandleAnimationEvent(cAnimationEvent *a_pEvent);

		void UpdateBVFromSubs();

		cMaterialManager *m_pMaterialManager;
		cMeshManager *m_pMeshManager;
		cAnimationManager *m_pAnimationManager;

		cWorld3D *m_pWorld;

		tSubMeshEntityVec m_vSubMeshes;
		tSubMeshEntityMap m_mapSubMeshes;

		tAnimationStateVec m_vAnimationStates;
		tAnimationStateIndexMap m_mapAnimationStateIndices;

		tNodeStateVec m_vBoneStates;


		std::vector<cMatrixf> m_vBoneMatrices;

		bool m_bSkeletonPhysics;
		bool m_bSkeletonPhysicsFading;
		float m_fSkeletonPhysicsFadeSpeed;
		float m_fSkeletonPhysicsWeight;

		bool m_bSkeletonPhysicsSleeping;
		bool m_bSkeletonPhysicsCanSleep;

		bool m_bSkeletonColliders;

		bool m_bUpdatedBones;

		bool m_bHasNodes;
		tNodeStateVec m_vNodeStates;
		tNodeStateIndexMap m_mapNodeStateIndices;

		int m_lStartSleepCount;
		int m_lUpdateCount;
		float m_fTimeStepAccum;

		cMesh *m_pMesh;

		cNode3D *m_pRootNode;
		cMeshEntityRootNodeUpdate *m_pRootCallback;

		cMeshEntityCallback *m_pCallback;

		tEntity3DList m_lstAttachedEntities;

		cMatrixf mtxTemp;

		iPhysicsBody *m_pBody;

		bool m_bCastShadows;
	};
};

#endif