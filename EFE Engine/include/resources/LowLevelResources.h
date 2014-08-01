#ifndef EFE_LOWLEVELRESOURCES_H
#define EFE_LOWLEVELRESOURCES_H

#include "system/SystemTypes.h"

namespace efe
{
	class iBitmap2D;
	class cMeshLoaderHandler;
	class cVideoManager;

	class iLowLevelResources
	{
	public:
		virtual ~iLowLevelResources(){}

		virtual void FindFilesInDirs(tWStringList &a_lstStrings, tWString a_sDir, tWString a_sMask)=0;

		virtual void GetSupportedImageFormats(tStringList &a_lstFormats)=0;
		virtual iBitmap2D *LoadBitmap2D(tString a_sFilePath, tString a_sType = "")=0;

		virtual void AddMeshLoaders(cMeshLoaderHandler *a_pHandler)=0;
	};
};

#endif