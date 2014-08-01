#include "scene/Node3D.h"
#include "scene/Entity3D.h"
#include "system/LowLevelSystem.h"
#include "math/Math.h"

namespace efe
{
	cNode3D::cNode3D(const tString &a_sName, bool a_bAutoDeleteChildren) : iNode()
	{
		m_mtxLocalTransform = cMatrixf::Identity;
		m_mtxWorldTransform = cMatrixf::Identity;

		m_vWorldPosition = cVector3f(0,0,0);

		m_bTransformUpdated = true;

		m_pParent = NULL;

		m_sName = a_sName;

		m_bAutoDeleteChildren = a_bAutoDeleteChildren;

		m_mtxRotation = cMatrixf::Identity;
		m_vScale = cVector3f(1,1,1);
		m_vTranslation = cVector3f(0,0,0);
	}

	cNode3D::~cNode3D()
	{
		if (m_bAutoDeleteChildren)
			STLDeleteAll(m_lstNode);
	}

	iNode *cNode3D::CreateChild()
	{
		return CreateChild3D();
	}

	cNode3D *cNode3D::CreateChild3D(const tString &a_sName, bool a_bAutoDeleteChildren)
	{
		cNode3D *pNode = efeNew(cNode3D, (a_sName, a_bAutoDeleteChildren));

		pNode->m_pParent = this;
		m_lstNode.push_back(pNode);

		return pNode;
	}
	
	//--------------------------------------------------------------

	cVector3f cNode3D::GetLocalPosition()
	{
		return m_mtxLocalTransform.GetTranslation();
	}

	//--------------------------------------------------------------

	cMatrixf &cNode3D::GetLocalMatrix()
	{
		return m_mtxLocalTransform;
	}

	//--------------------------------------------------------------

	cMatrixf &cNode3D::GetWorldMatrix()
	{
		UpdateWorldTransform();

		return m_mtxWorldTransform;
	}

	//--------------------------------------------------------------

	void cNode3D::SetPosition(const cVector3f &a_vPos)
	{
		m_mtxLocalTransform.m[0][3] = a_vPos.x;
		m_mtxLocalTransform.m[1][3] = a_vPos.y;
		m_mtxLocalTransform.m[2][3] = a_vPos.z;

		SetWorldTransformUpdated();
	}

	void cNode3D::SetMatrix(const cMatrixf &a_mtxTransform, bool a_bSetChildrenUpdated)
	{
		m_mtxLocalTransform = a_mtxTransform;

		if (a_bSetChildrenUpdated)
			SetWorldTransformUpdated();
		else
			m_bTransformUpdated = true;
	}

	//--------------------------------------------------------------

	const char *cNode3D::GetName()
	{
		return m_sName.c_str();
	}

	//--------------------------------------------------------------

	cNode3D *cNode3D::GetParent()
	{
		return m_pParent;
	}

	//--------------------------------------------------------------

	void cNode3D::AddRotation(const cQuaternion &a_qRotation)
	{
		m_mtxRotation = cMath::MatrixMul(cMath::MatrixQuaternion(a_qRotation), m_mtxRotation);
	}

	//--------------------------------------------------------------

	void cNode3D::AddTranslation(const cVector3f &a_vTrans)
	{
		m_vTranslation += a_vTrans;
	}

	//--------------------------------------------------------------

	void cNode3D::SetSource(const tString &a_sSource)
	{
		m_sSource = a_sSource;
	}

	const char *cNode3D::GetSource()
	{
		return m_sSource.c_str();
	}

	//--------------------------------------------------------------

	void cNode3D::SetParent(cNode3D *a_pNode)
	{
		m_pParent = a_pNode;

		m_pParent->m_lstNode.push_back(this);
	}

	void cNode3D::AddChild(cNode3D *a_pNode)
	{
		m_lstNode.push_back(a_pNode);
	}

	//--------------------------------------------------------------

	void cNode3D::UpdateWorldTransform()
	{
		if (m_bTransformUpdated)
		{
			m_bTransformUpdated = false;

			if (m_pParent)
				m_mtxWorldTransform = cMath::MatrixMul(m_pParent->GetWorldMatrix(), m_mtxLocalTransform);
			else
				m_mtxWorldTransform = m_mtxLocalTransform;
		}
	}

	void cNode3D::SetWorldTransformUpdated()
	{
		m_bTransformUpdated = true;

		tEntityListIt EIt = m_lstEntity.begin();
		for(;EIt!= m_lstEntity.end(); EIt++)
		{
			iEntity3D *pEntity = static_cast<iEntity3D*>(*EIt);

			pEntity->SetTransformUpdated();
		}

		tNodeListIt NIt = m_lstNode.begin();
		for (;NIt!= m_lstNode.end();++NIt)
		{
			cNode3D *pNode = static_cast<cNode3D*>(*NIt);

			pNode->SetWorldTransformUpdated();
		}
	}

	//--------------------------------------------------------------

	kBeginSerialize(cSaveData_cNode3D, cSaveData_iNode)
	kSerializeVar(m_sName, eSerializeType_String)
	kSerializeVar(m_sSource, eSerializeType_String)
	kSerializeVar(m_bAutoDeleteChildren, eSerializeType_Bool)
	kSerializeVar(m_mtxLocalTransform, eSerializeType_Matrixf)
	kSerializeVar(m_lParentId, eSerializeType_Int32)
	kEndSerialize()

	iSaveObject *cSaveData_cNode3D::CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		return efeNew(cNode3D, (m_sName, m_bAutoDeleteChildren));
	}

	int cSaveData_cNode3D::GetSaveCreatePrio()
	{
		return 0;
	}

	iSaveData *cNode3D::CreateSaveData()
	{
		return efeNew(cSaveData_cNode3D,());
	}

	void cNode3D::SaveToSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_SaveToBegin(cNode3D);

		kSaveData_SaveTo(m_sName);
		kSaveData_SaveTo(m_sSource);
		kSaveData_SaveTo(m_bAutoDeleteChildren);
		kSaveData_SaveTo(m_mtxLocalTransform);

		kSaveData_SaveObject(m_pParent, m_lParentId);
	}

	void cNode3D::LoadFromSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_LoadFromBegin(cNode3D);

		kSaveData_LoadFrom(m_sName);
		kSaveData_LoadFrom(m_sSource);
		kSaveData_LoadFrom(m_bAutoDeleteChildren);
		kSaveData_LoadFrom(m_mtxLocalTransform);
	}

	void cNode3D::SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		kSaveData_SetupBegin(cNode3D);

		kSaveData_LoadObject(m_pParent, m_lParentId, cNode3D*);
	}
}