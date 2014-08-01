#ifndef EFE_SAVE_GAME_H
#define EFE_SAVE_GAME_H

#include <map>

#include "system/SystemTypes.h"
#include "system/SerializeClass.h"

class TiXmlElement;

#define kSaveData_LoadFromBegin(a_Class) \
							__super::LoadFromSaveData(a_pSaveData); \
							cSaveData_##a_Class *pData = static_cast<cSaveData_##a_Class*>(a_pSaveData);

#define kSaveData_SaveToBegin(a_Class) \
							__super::SaveToSaveData(a_pSaveData); \
							cSaveData_##a_Class *pData = static_cast<cSaveData_##a_Class*>(a_pSaveData);

#define kSaveData_SetupBegin(a_Class) \
							__super::SaveDataSetup(a_pSaveObjectHandler,a_pGame);\
							cSaveData_##a_Class *pData = static_cast<cSaveData_##a_Class*>(m_pSaveData); \
							const char *sClassNameString = #a_Class;

#define kSaveData_BaseClass(a_Class) class cSaveData_##a_Class : public iSaveData
#define kSaveData_ChildClass(a_Parent, a_Child) class cSaveData_##a_Child : public cSaveData_##a_Parent

#define kSaveData_ClassInit(a_Class) kSerializableClassInit(cSaveData_##a_Class)

#define kSaveData_SaveTo(a_Var) pData->a_Var = a_Var
#define kSaveData_LoadFrom(a_Var) a_Var = pData->a_Var

#define kSaveData_SaveObject(a_Object, a_Id) \
						if (a_Object) \
							pData->a_Id = a_Object->GetSaveObjectId(); \
						else \
							pData->a_Id = -1;

#define kSaveData_LoadObject(a_Object, a_Id, a_Class) \
								if (pData->a_Id == -1) \
								a_Object = NULL; \
								else \
								a_Object = static_cast<a_Class>(a_pSaveObjectHandler->Get(pData->a_Id));

#define kSaveData_SaveIdList(a_SrcList, a_SrcIt, a_DestList) \
	pData->a_DestList.Clear(); \
	for (a_SrcIt it = a_SrcList.begin(); it != a_SrcList.end(); ++it) \
	{ \
		pData->a_DestList.Add((*it)->GetSaveObjectId()); \
	}

#define kSaveData_LoadIdList(a_SrcList, a_DestList, a_Class) \
{ cContainerListIterator<int> it = pData->a_DestList->GetIterator(); \
	a_SrcList.clear(); \
	while (it.HasNext()) \
	{ \
		int lId = it.Next(); \
		iSaveObject *pObject = a_pSaveObjectHandler->Get(lId); \
		if (pObject == NULL) \
		{ \
			continue; \
		} \
		a_SrcList.push_back(static_cast<a_Class>(pObject)); \
	} \
}


namespace efe
{
	class cSaveObjectHandler;
	class iSaveObject;
	class cGame;

	class iSaveData : public iSerializable
	{
		kSerializableClassInit(iSaveData);
	public:
		int m_lSaveDataId;

		virtual iSaveObject *CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)=0;

		virtual int GetSaveCreatePrio()=0;
	};

	class iSaveObject
	{
	friend class cSaveObjectHandler;
	public:
		iSaveObject();
		~iSaveObject();

		int GetSaveObjectId(){return m_lSaveObjectId;}

		virtual void SaveToSaveData(iSaveData *a_pSaveData);

		virtual void LoadFromSaveData(iSaveData *a_pSaveData);

		virtual iSaveData *CreateSaveData()=0;

		virtual void SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);

		void SetIsSaved(bool a_bX){m_bIsSaved = a_bX;}
		bool IsSaved(){return m_bIsSaved;}
	protected:
		iSaveData *m_pSaveData;
	private:
		int m_lSaveObjectId;
		bool m_bIsSaved;
		static int _m_lGlobalIdCount;
	};

	typedef std::multimap<int, iSaveObject*> tSaveObjectMap;
	typedef tSaveObjectMap::iterator tSaveObjectMapIt;

	typedef cSTLMapIterator<iSaveObject*, tSaveObjectMap, tSaveObjectMapIt> cSaveObjectIterator;

	class cSaveObjectHandler
	{
	public:
		cSaveObjectHandler();
		~cSaveObjectHandler();

		void Add(iSaveObject *a_pObject);

		iSaveObject *Get(int a_lId);

		cSaveObjectIterator GetIterator();

		void SetUpAll(cGame *a_pGame);

		void Clear();
		size_t Size();
	private:
		tSaveObjectMap m_mapSaveObjects;
	};

	typedef std::multimap<int, iSaveData*> tSaveDataMap;
	typedef tSaveDataMap::iterator tSaveDataMapIt;

	typedef cSTLMapIterator<iSaveData*, tSaveDataMap, tSaveDataMapIt> cSaveDataIterator;

	class cSaveDataHandler : public iContainer
	{
	public:
		cSaveDataHandler();
		~cSaveDataHandler();

		void Add(iSaveData *a_pData);

		cSaveDataIterator GetIterator();

		void Clear();
		size_t Size();
	private:
		void AddVoidPtr(void **a_pPtr);
		void AddVoidClass(void *a_pClass);

		iContainerIterator *CreateIteratorPtr();

		tSaveDataMap m_mapSaveData;
	};
};
#endif