#include "scene/Node.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	iNode::~iNode()
	{
		for (tEntityListIt it = m_lstEntity.begin();it != m_lstEntity.end();it++)
		{
			iEntity *pEntity = *it;
			pEntity->SetParent(NULL);
		}
		m_lstEntity.clear();
	}

	bool iNode::AddEntity(iEntity *a_pEntity)
	{
		if (a_pEntity->HasParent()) return false;

		m_lstEntity.push_back(a_pEntity);
		a_pEntity->SetParent(this);

		return true;
	}

	bool iNode::RemoveEntity(iEntity *a_pEntity)
	{
		for (tEntityListIt it = m_lstEntity.begin(); it!=m_lstEntity.end();it++)
		{
			if (*it == a_pEntity)
			{
				a_pEntity->SetParent(NULL);
				m_lstEntity.erase(it);
				return true;
			}
		}
		return false;
	}

	//--------------------------------------------------------------

	cNodeIterator iNode::GetChildIterator()
	{
		return cNodeIterator(&m_lstNode);
	}

	cEntityIterator iNode::GetEntityIterator()
	{
		return cEntityIterator(&m_lstEntity);
	}

	//--------------------------------------------------------------

	kBeginSerializeVirtual(cSaveData_iNode, iSaveData)
	kSerializeVarContainer(m_lstEntities, eSerializeType_Int32)
	kSerializeVarContainer(m_lstNodes, eSerializeType_Int32)
	kEndSerialize()

	void iNode::SaveToSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_SaveToBegin(iNode);

		//
	}

	void iNode::LoadFromSaveData(iSaveData *a_pSaveData)
	{
	}

	void iNode::SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
	}
}