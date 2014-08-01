#include "resources/FileSearcher.h"
#include "system/String.h"
#include "resources/LowLevelResources.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	cFileSearcher::cFileSearcher(iLowLevelResources *a_pLowLevelResources)
	{
		m_pLowLevelResources = a_pLowLevelResources;
	}

	cFileSearcher::~cFileSearcher()
	{
	}

	void cFileSearcher::AddDirectory(tString a_sPath, tString a_sMask)
	{
		tWStringList lstFileNames;

		a_sPath = cString::ToLowerCase(cString::ReplaceCharTo(a_sPath, "\\", "/"));

		tStringSetIt it = m_setLoadedDirs.find(a_sPath);

		if (it == m_setLoadedDirs.end())
		{
			m_setLoadedDirs.insert(a_sPath);

			m_pLowLevelResources->FindFilesInDirs(lstFileNames,cString::To16Char(a_sPath),
													cString::To16Char(a_sMask));

			for (tWStringListIt it = lstFileNames.begin();it!=lstFileNames.end();it++)
			{
				tString sFile = cString::To8Char(*it);
				m_mapFiles.insert(tFilePathMap::value_type(
														cString::ToLowerCase(sFile),
														cString::SetFilePath(sFile,a_sPath)));
			
			}
		}
	}

	void cFileSearcher::ClearDirectories()
	{
		m_mapFiles.clear();
		m_setLoadedDirs.clear();
	}

	tString cFileSearcher::GetFilePath(tString a_sName)
	{
		tFilePathMapIt it = m_mapFiles.find(cString::ToLowerCase(a_sName));
		if (it == m_mapFiles.end())return "";

		return it->second;
	}
}