#ifndef EFE_FILESEARCHER_H
#define EFE_FILESEARCHER_H

#include <map>
//#include "resources/ResourcesTypes.h"
#include "system/SystemTypes.h"

namespace efe
{
	class iLowLevelResources;

	typedef std::multimap<tString, tString> tFilePathMap;
	typedef tFilePathMap::iterator tFilePathMapIt;

	class cFileSearcher
	{
	public:
		cFileSearcher(iLowLevelResources *a_pLowLevelResources);
		~cFileSearcher();

		void AddDirectory(tString a_sPath, tString a_sMask);

		tStringSet &GetLoadedDirsSet(){return m_setLoadedDirs;}
		tFilePathMap &GetFilesMap(){return m_mapFiles;}

		void ClearDirectories();

		tString GetFilePath(tString a_sName);
	private:
		tFilePathMap m_mapFiles;
		tStringSet m_setLoadedDirs;

		iLowLevelResources *m_pLowLevelResources;
	};
};

#endif