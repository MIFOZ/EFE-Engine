#include "game/SaveGame.h"

#include "system/LowLevelSystem.h"

namespace efe
{
	kBeginSerializeBaseVirtual(iSaveData)
	kSerializeVar(m_lSaveDataId, eSerializeType_Int32)
	kEndSerialize()

	int iSaveObject::_m_lGlobalIdCount = 0;

	iSaveObject::iSaveObject()
	{
		m_lSaveObjectId = _m_lGlobalIdCount++;
		if (_m_lGlobalIdCount < 0) _m_lGlobalIdCount=0;

		m_bIsSaved = true;
	}

	iSaveObject::~iSaveObject()
	{
	}

	void iSaveObject::SaveToSaveData(iSaveData *a_pSaveData)
	{
		a_pSaveData->m_lSaveDataId = m_lSaveObjectId;
	}

	void iSaveObject::LoadFromSaveData(iSaveData *a_pSaveData)
	{
		m_lSaveObjectId = a_pSaveData->m_lSaveDataId;
		m_pSaveData = a_pSaveData;
	}

	void iSaveObject::SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// SAVE GAME CONTAINER
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSaveObjectHandler::cSaveObjectHandler()
	{
		
	}

	cSaveObjectHandler::~cSaveObjectHandler()
	{
		
	}

	iSaveObject *cSaveObjectHandler::Get(int a_lId)
	{
		tSaveObjectMapIt it = m_mapSaveObjects.find(a_lId);
		if (it == m_mapSaveObjects.end())
		{
			Warning("Couldn't find save object with id %d\n", a_lId);
			return NULL;
		}

		return it->second;
	}
}