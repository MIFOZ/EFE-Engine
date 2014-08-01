#include "impl/Platform.h"
#include <io.h>

#include "system/LowLevelSystem.h"

#define _UNICODE
#include <stdio.h>

namespace efe
{
	long Platform::FileLength(const tWString &a_sFileName)
	{
		FILE *pFile = _wfopen(a_sFileName.c_str(), _W("rb"));
		long ret = Platform::FileLength(pFile);
		fclose(pFile);
		return ret;
	}

	long Platform::FileLength(FILE *pFile)
	{
		return (long)_filelength(_fileno(pFile));
	}

	void Platform::FindFileInDirs(tWStringList &a_lstStrings, tWString a_sDir, tWString a_sMask)
	{
		a_sDir = cString::ReplaceCharToW(a_sDir, _W("/"), _W("\\"));

		wchar_t sSpec[256];
		wchar_t end = a_sDir[a_sDir.size()-1];

		if (end == _W('\\') || end == _W('/'))
			swprintf(sSpec,256,_W("%s$s"),a_sDir.c_str(),a_sMask.c_str());
		else
			swprintf(sSpec,256,_W("%s\\%s"),a_sDir.c_str(),a_sMask.c_str());

		intptr_t lHandle;
		struct _wfinddata_t FileInfo;

		lHandle = _wfindfirst(sSpec, &FileInfo);
		if (lHandle == -1L) return;

		if ((FileInfo.attrib & _A_SUBDIR)==0)
			a_lstStrings.push_back(FileInfo.name);

		while (_wfindnext(lHandle, &FileInfo) == 0)
		{
			if ((FileInfo.attrib & _A_SUBDIR) == 0)
				a_lstStrings.push_back(FileInfo.name);
		}
	}
}