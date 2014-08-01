#ifndef EFE_FONT_MANAGER_H
#define EFE_FONT_MANAGER_H

#include "resources/ResourceManager.h"

namespace efe
{
	class cGraphics;
	class cResources;
	class cGui;
	class iFontData;

	class cFontManager : public iResourceManager
	{
	public:
		cFontManager(cGraphics *a_pGraphics, cGui *a_pGui, cResources *a_pResources);
		~cFontManager();

		iResourceBase *Create(const tString &a_sName);

		iFontData *CreateFontData(const tString &a_sName, int a_lSize=16, unsigned short a_lFirstChar=32,
								unsigned short a_lLastChar=255);

		void Destroy(iResourceBase *a_pResource);
		void Unload(iResourceBase *a_pResource);

	private:
		cGraphics *m_pGraphics;
		cResources *m_pResources;
		cGui *m_pGui;
	};
};
#endif