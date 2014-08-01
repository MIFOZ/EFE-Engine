#ifndef EFE_NODE_H
#define EFE_NODE_H

#include <list>
#include "scene/Entity.h"
#include "math/MathTypes.h"
#include "system/Container.h"
#include "game/SaveGame.h"

namespace efe
{
	class iNode;

	typedef std::list<iNode*> tNodeList;
	typedef tNodeList::iterator tNodeListIt;

	typedef cSTLIterator<iNode*, tNodeList, tNodeListIt> cNodeIterator;

	kSaveData_BaseClass(iNode)
	{
		kSaveData_ClassInit(iNode)
	public:
		cContainerList<int> m_lstEntities;
		cContainerList<int> m_lstNodes;
	};

	class iNode : public iSaveObject
	{
	public:
		iNode() : m_bActive(true){}
		virtual ~iNode();

		int SetVisible(bool a_bX, bool a_bCascade);
		bool AddEntity(iEntity *a_pEntity);
		bool RemoveEntity(iEntity *a_pEntity);
		void ClearEntities();

		void SetActive(bool a_bX){m_bActive = a_bX;}
		bool IsActive(){return m_bActive;}

		virtual iNode *CreateChild()=0;

		cNodeIterator GetChildIterator();
		cEntityIterator GetEntityIterator();

		virtual iSaveData *CreateSaveData(){return NULL;}
		virtual void SaveToSaveData(iSaveData *a_pSaveData);
		virtual void LoadFromSaveData(iSaveData *a_pSaveData);
		virtual void SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);
	protected:
		tEntityList m_lstEntity;
		tNodeList m_lstNode;

		bool m_bActive;
	};
};

#endif