#ifndef EFE_LOWLEVELRESOURCES_DX_H
#define EFE_LOWLEVELRESOURCES_DX_H

#include "resources/LowLevelResources.h"
#include "impl/LowLevelGraphicsDX11.h"
#include "system/SystemTypes.h"

namespace efe
{
	class cLowLevelResourcesDX : public iLowLevelResources
	{
	public:
		cLowLevelResourcesDX(cLowLevelGraphicsDX11 *a_pLowLevelGraphics);
		~cLowLevelResourcesDX();

		void FindFilesInDirs(tWStringList &a_lstStrings, tWString a_sDir, tWString a_sMask);

		void GetSupportedImageFormats(tStringList &a_lstFormats);
		iBitmap2D *LoadBitmap2D(tString a_sFilePath, tString a_sType = "");

		void AddMeshLoaders(cMeshLoaderHandler *a_pHandler);
	private:
		tString m_vImageFormats[30];
		cLowLevelGraphicsDX11 *m_pLowLevelGraphics;
	};
};

#endif