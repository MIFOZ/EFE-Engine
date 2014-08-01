#ifndef EFE_ENTITY_H
#define EFE_ENTITY_H

#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "game/SaveGame.h"

namespace efe
{
	class iNode;

	kSaveData_BaseClass(iEntity)
	{
		kSaveData_ClassInit(iEntity)
	public:
		int m_lParentNodeId;
		tString m_sName;
		bool m_bIsVisible;
		bool m_bIsActive;
	};

	class iEntity : public iSaveObject
	{
	public:
		iEntity(tString a_sName) : m_sName(a_sName), m_bIsVisible(true),
			m_bIsActive(true), m_pParentNode(NULL){}

		virtual ~iEntity();

		virtual tString GetEntityType()=0;

		virtual void UpdateLogic(float a_fTimeStep){}

		tString &GetName(){return m_sName;}
		void SetName(const tString &a_sName){m_sName = a_sName;}

		iNode *GetParent(){return m_pParentNode;}
		void SetParent(iNode *a_pNode){m_pParentNode = a_pNode;}
		bool HasParent(){return m_pParentNode!=NULL;}

		bool IsActive(){return m_bIsActive;}
		void SetActive(bool a_bActive){m_bIsActive = a_bActive;}
		virtual bool IsVisible(){return m_bIsVisible;}
		virtual void SetVisible(bool a_bActive){m_bIsVisible = a_bActive;}

		virtual iSaveData *CreateSaveData();
		virtual void SaveToSaveData(iSaveData *a_pSaveData);
		virtual void LoadFromSaveData(iSaveData *a_pSaveData);
		virtual void SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);
	protected:
		iNode *m_pParentNode;

		tString m_sName;
		bool m_bIsVisible;
		bool m_bIsActive;
	};

	typedef std::list<iEntity*> tEntityList;
	typedef tEntityList::iterator tEntityListIt;

	typedef cSTLIterator<iEntity*, tEntityList, tEntityListIt> cEntityIterator;
};
#endif