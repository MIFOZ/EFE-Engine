#include "resources/ResourceManager.h"
#include "system/String.h"

#include "resources/LowLevelResources.h"
#include "resources/FileSearcher.h"
#include "resources/ResourceBase.h"

#include "system/LowLevelSystem.h"

#include <algorithm>

namespace efe
{
	int iResourceManager::m_lTabCount=0;

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iResourceManager::iResourceManager(cFileSearcher *a_pFileSearcher, iLowLevelResources *a_pLowLevelResources,
										iLowLevelSystem *a_pLowLevelSystem)
	{
		m_pFileSearcher = a_pFileSearcher;
		m_pLowLevelResources = a_pLowLevelResources;
		m_pLowLevelSystem = a_pLowLevelSystem;
		m_lHandleCount = 0;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iResourceBase *iResourceManager::GetByName(const tString &a_sName)
	{
		tString sName = cString::ToLowerCase(a_sName);
		Log("Looking for '%s' \n", sName.c_str());

		tResourceNameMapIt it = m_mapNameResources.find(sName);
		if (it == m_mapNameResources.end()) return NULL;
		return it->second;
	}



	//--------------------------------------------------------------

	void iResourceManager::DestroyAll()
	{
		tResourceHandleMapIt it = m_mapHandleResources.begin();
		while (it != m_mapHandleResources.end())
		{
			iResourceBase *pResource = it->second;

			while(pResource->HasUsers()) pResource->DecUserCount();

			Destroy(pResource);

			it = m_mapHandleResources.begin();
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void iResourceManager::BeginLoad(const tString &a_sFile)
	{
		m_lTimeStart = GetApplicationTime();

		m_lTabCount++;
	}

	void iResourceManager::EndLoad()
	{
		m_lTabCount--;
	}

	iResourceBase *iResourceManager::FindLoadedResource(const tString &a_sName, tString &a_sFilePath)
	{
		iResourceBase *pResource = GetByName(a_sName);
		if (pResource == NULL)
			a_sFilePath = m_pFileSearcher->GetFilePath(a_sName);
		else
			a_sFilePath = "";

		return pResource;
	}

	//--------------------------------------------------------------

	tString iResourceManager::GetTabs()
	{
		tString sTabs = "";
		for (int i=0; i<m_lTabCount;++i) sTabs += " ";
		return sTabs;
	}

	void iResourceManager::AddResource(iResourceBase *a_pResource, bool a_bLog)
	{
		a_pResource->SetHanlde(GetHandle());

		tString sName = cString::ToLowerCase(a_pResource->GetName());

		m_mapHandleResources.insert(tResourceHandleMap::value_type(
								a_pResource->GetHandle(), a_pResource));
		m_mapNameResources.insert(tResourceNameMap::value_type(
								sName, a_pResource));

		if (a_bLog && iResourceBase::GetLogCreateAndDelete())
		{
			unsigned long lTime = GetApplicationTime() - m_lTimeStart;
			Log("%sLoaded resource %s in %d ms\n", GetTabs().c_str(), a_pResource->GetName().c_str(), lTime);
			a_pResource->SetLogDestruction(true);
		}
	}

	//--------------------------------------------------------------

	void iResourceManager::RemoveResource(iResourceBase *a_pResource)
	{
		m_mapHandleResources.erase(a_pResource->GetHandle());
		m_mapNameResources.erase(cString::ToLowerCase(a_pResource->GetName()));
	}

	unsigned long iResourceManager::GetHandle()
	{
		return m_lHandleCount++;
	}
}