#include "scene/SubMeshEntity.h"

#include "scene/MeshEntity.h"

#include "resources/MaterialManager.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"

#include "scene/NodeState.h"

#include "physics/PhysicsBody.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSubMeshEntity::cSubMeshEntity(const tString &a_sName, cMeshEntity *a_pMeshEntity, cSubMesh *a_pSubMesh,
		cMaterialManager *a_pMaterialManager) : iRenderable(a_sName)
	{
		m_pMeshEntity = a_pMeshEntity;
		m_pSubMesh = a_pSubMesh;

		m_bIsOneSided = m_pSubMesh->GetIsOneSided();
		m_vOneSidedNormal = m_pSubMesh->GetOneSidedNormal();

		m_pMaterialManager = a_pMaterialManager;

		m_bCastShadows = false;

		m_bGraphicsUpdated = false;

		m_pBody = NULL;

		if (m_pMeshEntity->GetMesh()->GetSkeleton())
		{
			//m_pDynVtxBuffer = m_pSubMesh
		}
		else
			m_pDynVtxBuffer = NULL;

		m_pLocalNode = NULL;

		m_pEntityCallback = efeNew(cSubMeshEntityBodyUpdate, ());
		m_bUpdateBody  = false;

		m_pMaterial = NULL;
	}

	cSubMeshEntity::~cSubMeshEntity()
	{
		efeDelete(m_pEntityCallback);

		if (m_pDynVtxBuffer) efeDelete(m_pDynVtxBuffer);

		if (m_pMaterial) m_pMaterialManager->Destroy(m_pMaterial);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iMaterial *cSubMeshEntity::GetMaterial()
	{
		if (m_pMaterial==NULL && m_pSubMesh->GetMaterial()==NULL)
			Error("Materials for sub entity %s are NULL!\n", GetName().c_str());

		if (m_pMaterial)
			return m_pMaterial;
		else
			return m_pSubMesh->GetMaterial();
	}

	iVertexBuffer *cSubMeshEntity::GetVertexBuffer()
	{
		if (m_pDynVtxBuffer)
			return m_pDynVtxBuffer;
		else
			return m_pSubMesh->GetVertexBuffer();
	}

	//--------------------------------------------------------------

	int cSubMeshEntity::GetMatrixUpdateCount()
	{
		if (m_pMeshEntity->HasNodes())
			return GetTransformUpdateCount();
		else
			return m_pMeshEntity->GetMatrixUpdateCount();
	}

	void cSubMeshEntity::SetLocalNode(cNode3D *a_pNode)
	{
		m_pLocalNode = a_pNode;

		m_pLocalNode->AddEntity(this);
	}

	cMatrixf *cSubMeshEntity::GetModelMatrix(cCamera3D *a_pCamera)
	{
		if (m_pMeshEntity->HasNodes())
			return &GetWorldMatrix();
		else
		{
			if (m_pMeshEntity->IsStatic()) return NULL;

			return m_pMeshEntity->GetModelMatrix(NULL);
		}
	}

	//--------------------------------------------------------------

	void cSubMeshEntity::SetUpdateBody(bool a_bX)
	{
		m_bUpdateBody = a_bX;
	}

	bool cSubMeshEntity::GetUpdateBody()
	{
		return m_bUpdateBody;
	}

	//--------------------------------------------------------------

	void cSubMeshEntity::SetCustomMaterial(iMaterial *a_Material, bool a_bDestroyOldCusotm)
	{
		if (a_bDestroyOldCusotm)
			if (m_pMaterial) m_pMaterialManager->Destroy(m_pMaterial);

		m_pMaterial = a_Material;
	}

	//--------------------------------------------------------------

	kBeginSerialize(cSaveData_cSubMeshEntity, cSaveData_iRenderable)
		kSerializeVar(m_sMaterial, eSerializeType_String)
		kSerializeVar(m_bCastShadows, eSerializeType_Bool)
		kSerializeVar(m_lBodyId, eSerializeType_Int32)
		kSerializeVar(m_bUpdateBody, eSerializeType_Bool)
	kEndSerialize()

	//--------------------------------------------------------------

	iSaveData *cSubMeshEntity::CreateSaveData()
	{
		return efeNew(cSaveData_cSubMeshEntity, ());
	}

	//--------------------------------------------------------------

	void cSubMeshEntity::SaveToSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_SaveToBegin(cSubMeshEntity);

		kSaveData_SaveTo(m_bCastShadows);
		kSaveData_SaveTo(m_bUpdateBody);

		pData->m_sMaterial = m_pMaterial==NULL ? "" : m_pMaterial->GetName();

		kSaveData_SaveObject(m_pBody, m_lBodyId);
	}

	//--------------------------------------------------------------

	void cSubMeshEntity::LoadFromSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_LoadFromBegin(cSubMeshEntity);

		kSaveData_LoadFrom(m_bCastShadows);
		kSaveData_LoadFrom(m_bUpdateBody);

		if (pData->m_sMaterial != "")
		{
			iMaterial *pMat = m_pMaterialManager->CreateMaterial(pData->m_sMaterial);
			if (pMat) SetCustomMaterial(pMat);
		}
	}

	//--------------------------------------------------------------

	void cSubMeshEntity::SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		kSaveData_SetupBegin(cSubMeshEntity);

		kSaveData_LoadObject(m_pBody, m_lBodyId, iPhysicsBody*);

		if (m_pBody && m_bUpdateBody == false)
			m_pBody->CreateNode()->AddEntity(this);
	}
}