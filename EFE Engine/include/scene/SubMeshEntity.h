#ifndef EFE_SUB_MESH_ENTITY_H
#define EFE_SUB_MESH_ENTITY_H

#include <Vector>
#include <map>

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "scene/Entity3D.h"
#include "graphics/Renderable.h"
#include "math/MeshTypes.h"

namespace efe
{
	class cMaterialManager;
	class cMeshManager;
	class cMesh;
	class cSubMesh;
	class cSubMeshEntity;
	class cAnimationState;
	class cNodeState;
	class cBone;
	class cNode3D;
	class iPhysicsBody;
	class iMaterial;
	class cBoneState;

	typedef std::vector<cAnimationState*> tAnimationStateVec;
	typedef  tAnimationStateVec::iterator  tAnimationStateVecIt;

	typedef std::map<tString, int> tAnimationStateIndexMap;
	typedef tAnimationStateIndexMap::iterator tAnimationStateIndexMapIt;

	typedef std::vector<cBoneState*> tNodeStateVec;
	typedef tNodeStateVec::iterator tNodeStateVecIt;

	typedef std::map<tString, int> tNodeStateIndexMap;
	typedef tNodeStateIndexMap::iterator tNodeStateIndexMapIt;

	class cSubMeshEntityBodyUpdate
	{
	public:
		void OnTransformUpdate(iEntity3D *a_pEntity);
	};

	kSaveData_ChildClass(iRenderable, cSubMeshEntity)
	{
		kSaveData_ClassInit(cSubMeshEntity)
	public:
		tString m_sMaterial;

		bool m_bCastShadows;

		int m_lBodyId;

		bool m_bUpdateBody;

		iSaveObject *CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame){return NULL;}
		int GetSaveCreatePrio(){return 0;}
	};

	class cSubMeshEntity : public iRenderable
	{
		friend class cMeshEntity;
	public:
		cSubMeshEntity(const tString &a_sName, cMeshEntity *a_pMeshEntity, cSubMesh *a_pSubMesh, cMaterialManager *a_pMaterialManager);
		~cSubMeshEntity();

		iMaterial *GetMaterial();

		iVertexBuffer *GetVertexBuffer();

		void SetCastsShadows(bool a_bX) {m_bCastShadows = a_bX;}
		bool IsShadowCaster(){return m_bCastShadows;}

		cMatrixf *GetModelMatrix(cCamera3D *a_pCamera);

		int GetMatrixUpdateCount();

		eRenderableType GetRenderType(){return eRenderableType_Normal;}

		void SetLocalNode(cNode3D *a_pNode);

		//Entity implementation
		tString GetEntityType(){return "SubMesh";}

		iPhysicsBody *GetBody() {return m_pBody;}
		void SetBody(iPhysicsBody *a_pBody) {m_pBody = a_pBody;}

		void SetUpdateBody(bool a_bX);
		bool GetUpdateBody();

		void SetCustomMaterial(iMaterial *a_Material, bool a_bDestroyOldCusotm=true);



		virtual iSaveData *CreateSaveData();
		virtual void SaveToSaveData(iSaveData *a_pSaveData);
		virtual void LoadFromSaveData(iSaveData *a_pSaveData);
		virtual void SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);

	private:
		bool m_bCastShadows;
		cSubMesh *m_pSubMesh;
		cMeshEntity *m_pMeshEntity;

		iMaterial *m_pMaterial;

		cNode3D *m_pLocalNode;

		cMaterialManager *m_pMaterialManager;

		iVertexBuffer *m_pDynVtxBuffer;
		tTriangleDataVec m_vDynTriangles;

		cSubMeshEntityBodyUpdate *m_pEntityCallback;
		bool m_bUpdateBody;

		bool m_bGraphicsUpdated;

		iPhysicsBody *m_pBody;
	};

	typedef std::vector<cSubMeshEntity*> tSubMeshEntityVec;
	typedef tSubMeshEntityVec::iterator tSubMeshEntityVecIt;

	typedef std::multimap<tString, cSubMeshEntity*> tSubMeshEntityMap;
	typedef tSubMeshEntityMap::iterator tSubMeshEntityMapIt;
};

#endif