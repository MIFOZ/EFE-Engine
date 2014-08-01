#ifndef EFE_RESOURCEMANAGER_H
#define EFE_RESOURCEMANAGER_H

#include <map>
#include "system/SystemTypes.h"

namespace efe
{
	class iLowLevelResources;
	class iLowLevelSystem;
	class cFileSearcher;
	class iResourceBase;

	typedef std::map<unsigned long, iResourceBase*> tResourceHandleMap;
	typedef tResourceHandleMap::iterator tResourceHandleMapIt;

	typedef std::map<tString, iResourceBase*> tResourceNameMap;
	typedef tResourceNameMap::iterator tResourceNameMapIt;

	typedef std::list<iResourceBase*> tResourceBaseList;
	typedef tResourceBaseList::iterator tResourceBaseListIt;

	typedef cSTLMapIterator<iResourceBase*, tResourceNameMap, tResourceNameMapIt> cResourceBaseIterator;

	class iResourceManager
	{
	public:
		iResourceManager(cFileSearcher *a_pFileSearcher, iLowLevelResources *a_pLowLevelResources,
						iLowLevelSystem *a_pLowLevelSystem);
		virtual ~iResourceManager(){}

		virtual iResourceBase *Create(const tString &a_sName)=0;

		iResourceBase *GetByName(const tString &a_sName);
		iResourceBase *GetByHandle(unsigned long a_lHandle);

		cResourceBaseIterator GetResourceBaseIterator();

		void DestroyUnused(int a_lMaxToKeep);

		virtual void Destroy(iResourceBase *a_pResource)=0;
		virtual void DestroyAll();

		virtual void Unload(iResourceBase *a_pResource)=0;

		virtual void Update(float a_fTimeStep){}
	protected:
		unsigned long m_lHandleCount;
		tResourceNameMap m_mapNameResources;
		tResourceHandleMap m_mapHandleResources;

		cFileSearcher *m_pFileSearcher;
		iLowLevelResources *m_pLowLevelResources;
		iLowLevelSystem *m_pLowLevelSystem;

		void BeginLoad(const tString &a_sFile);
		void EndLoad();

		unsigned long m_lTimeStart;

		iResourceBase *FindLoadedResource(const tString &a_sName, tString &a_sFilePath);
		void AddResource(iResourceBase *a_pResource, bool a_bLog = true);
		void RemoveResource(iResourceBase *a_pResource);

		unsigned long GetHandle();

		tString GetTabs();
		static int m_lTabCount;
	};


};

#endif