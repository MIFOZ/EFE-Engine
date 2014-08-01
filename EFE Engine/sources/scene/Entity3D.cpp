#include "scene/Entity3D.h"
#include "scene/Node3D.h"
#include "math/Math.h"
#include "graphics/RenderList.h"

#include "system/LowlevelSystem.h"

#include "scene/PortalContainer.h"

namespace efe
{
	iEntity3D::iEntity3D(tString a_sName) : iEntity(a_sName)
	{
		m_mtxLocalTransform = cMatrixf::Identity;
		m_mtxWorldTransform = cMatrixf::Identity;

		m_bTransformUpdated = true;

		m_lCount = 0;

		m_lGlobalRenderCount = cRenderList::GetGlobalRenderCount();

		m_sSourceFile = "";

		m_bApplyTransformToBV = true;
		m_bUpdateBoundingVolume = true;

		m_pParent = NULL;

		m_lIteratorCount = -1;

		m_pCurrentSector = NULL;
	}

	iEntity3D::~iEntity3D()
	{
		//if (m_pParent) m_pParent->re
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cVector3f iEntity3D::GetLocalPosition()
	{
		return m_mtxLocalTransform.GetTranslation();
	}

	//--------------------------------------------------------------

	cMatrixf &iEntity3D::GetLocalMatrix()
	{
		return m_mtxLocalTransform;
	}

	//--------------------------------------------------------------

	cVector3f iEntity3D::GetWorldPosition()
	{
		UpdateWorldTransform();

		return m_mtxWorldTransform.GetTranslation();
	}

	cMatrixf &iEntity3D::GetWorldMatrix()
	{
		UpdateWorldTransform();

		return m_mtxWorldTransform;
	}

	//--------------------------------------------------------------

	void iEntity3D::SetPosition(const cVector3f &a_vPos)
	{
		m_mtxLocalTransform.m[0][3] = a_vPos.x;
		m_mtxLocalTransform.m[1][3] = a_vPos.y;
		m_mtxLocalTransform.m[2][3] = a_vPos.z;

		SetTransformUpdated();
	}

	//--------------------------------------------------------------

	void iEntity3D::SetMatrix(const cMatrixf &a_mtxTransform)
	{
		m_mtxLocalTransform = a_mtxTransform;

		SetTransformUpdated();
	}

	//--------------------------------------------------------------

	void iEntity3D::SetWorldPosition(const cVector3f &a_vWorldPos)
	{
		if (m_pParent)
			SetPosition(a_vWorldPos - m_pParent->GetWorldPosition());
		else
			SetPosition(a_vWorldPos);
	}

	//--------------------------------------------------------------

	void iEntity3D::SetTransformUpdated(bool a_bUpdatedCallbacks)
	{
		m_bTransformUpdated = true;
		m_lCount++;

		if (m_bApplyTransformToBV)
			m_BoundingVolume.SetTransform(GetWorldMatrix());

		m_bUpdateBoundingVolume = true;

		for (tEntity3DListIt EntIt = m_lstChildren.begin(); EntIt != m_lstChildren.end(); ++EntIt)
		{
			iEntity3D *pChild = *EntIt;
			pChild->SetTransformUpdated(true);
		}

		if (m_lstCallbacks.empty() || a_bUpdatedCallbacks == false) return;

		tEntityCallbackListIt it = m_lstCallbacks.begin();
		for (; it != m_lstCallbacks.end(); ++it)
		{
			iEntityCallback *pCallBack = *it;
			pCallBack->OnTransformUpdate(this);
		}
	}

	//--------------------------------------------------------------

	int iEntity3D::GetTransformUpdateCount()
	{
		return m_lCount;
	}

	void iEntity3D::AddCallback(iEntityCallback *a_pCallback)
	{
		m_lstCallbacks.push_back(a_pCallback);
	}

	//--------------------------------------------------------------

	void iEntity3D::AddChild(iEntity3D *a_pEntity)
	{
		if (a_pEntity==NULL) return;
		if (a_pEntity->m_pParent != NULL) return;

		m_lstChildren.push_back(a_pEntity);
		a_pEntity->m_pParent = this;

		a_pEntity->SetTransformUpdated(true);
	}

	//--------------------------------------------------------------

	bool iEntity3D::IsInSector(cSector *a_pSector)
	{
		if (a_pSector == GetCurrentSector())
			return true;

		tRenderContainerDataList *pDataList = GetRenderContainerDataList();
		tRenderContainerDataListIt it = pDataList->begin();
		for (;it != pDataList->end();++it)
		{
			iRenderContainerData *pRenderContainerData = *it;
			cSector *pSector = static_cast<cSector*>(pRenderContainerData);

			if (pSector == a_pSector)
				return true;
		}

		return false;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void iEntity3D::UpdateWorldTransform()
	{
		if (m_bTransformUpdated)
		{
			m_bTransformUpdated = false;

			if (m_pParentNode)
			{
				cNode3D *pNode3D = static_cast<cNode3D*>(m_pParentNode);

				m_mtxWorldTransform = cMath::MatrixMul(pNode3D->GetWorldMatrix(), m_mtxLocalTransform);
			}
			else if (m_pParent)
				m_mtxWorldTransform = cMath::MatrixMul(m_pParent->GetWorldMatrix(), m_mtxLocalTransform);
			else
				m_mtxWorldTransform = m_mtxLocalTransform;
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	kBeginSerializeVirtual(cSaveData_iEntity3D, cSaveData_iEntity)
		kSerializeVar(m_mtxLocalTransform, eSerializeType_Matrixf)
		kSerializeVar(m_BoundingVolume, eSerializeType_Class)
		kSerializeVar(m_sSourceFile, eSerializeType_String)

		kSerializeVar(m_lParentId, eSerializeType_Int32)
		kSerializeVarContainer(m_lstChildren, eSerializeType_Int32)
	kEndSerialize()

	//--------------------------------------------------------------

	iSaveData *iEntity3D::CreateSaveData()
	{
		return NULL;
	}

	//--------------------------------------------------------------

	void iEntity3D::SaveToSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_SaveToBegin(iEntity3D);

		kSaveData_SaveTo(m_mtxLocalTransform);
		kSaveData_SaveTo(m_BoundingVolume);
		kSaveData_SaveTo(m_sSourceFile);

		kSaveData_SaveObject(m_pParent, m_lParentId);
		kSaveData_SaveIdList(m_lstChildren, tEntity3DListIt, m_lstChildren);
	}

	//--------------------------------------------------------------

	void iEntity3D::LoadFromSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_LoadFromBegin(iEntity3D);

		SetMatrix(pData->m_mtxLocalTransform);

		kSaveData_LoadFrom(m_BoundingVolume);

		kSaveData_LoadFrom(m_sSourceFile);
	}

	//--------------------------------------------------------------

	void iEntity3D::SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		kSaveData_SetupBegin(iEntity3D);
	}
}