#include "scene/Entity.h"

#include "scene/Node.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	iEntity::~iEntity()
	{
		if (m_pParentNode)
			m_pParentNode->RemoveEntity(this);
	}

	//////////////////////////////////////////////////////////////
	// SAVE OBJECT TYPES
	//////////////////////////////////////////////////////////////

	kBeginSerializeVirtual(cSaveData_iEntity, iSaveData)
	kSerializeVar(m_lParentNodeId, eSerializeType_Int32)
	kSerializeVar(m_sName, eSerializeType_String)
	kSerializeVar(m_bIsVisible, eSerializeType_Bool)
	kSerializeVar(m_bIsActive, eSerializeType_Bool)
	kEndSerialize()

	iSaveData *iEntity::CreateSaveData()
	{
		return NULL;
	}

	//--------------------------------------------------------------

	void iEntity::SaveToSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_SaveToBegin(iEntity);

		kSaveData_SaveTo(m_bIsActive);
		kSaveData_SaveTo(m_bIsVisible);
		kSaveData_SaveTo(m_sName);

		kSaveData_SaveObject(m_pParentNode, m_lParentNodeId);
	}

	//--------------------------------------------------------------

	void iEntity::LoadFromSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_LoadFromBegin(iEntity);

		kSaveData_LoadFrom(m_bIsActive);
		kSaveData_LoadFrom(m_bIsVisible);
		kSaveData_LoadFrom(m_sName);
	}

	void iEntity::SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		kSaveData_SetupBegin(iEntity);
	}
}