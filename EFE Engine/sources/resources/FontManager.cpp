#include "resources/FontManager.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "graphics/Graphics.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/ImageManager.h"

#include "graphics/FontData.h"

namespace efe
{

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cFontManager::cFontManager(cGraphics *a_pGraphics, cGui *a_pGui, cResources *a_pResources)
		: iResourceManager(a_pResources->GetFileSearcher(), a_pResources->GetLowLevel(),
							a_pResources->GetLowLevelSystem())
	{
		m_pGraphics = a_pGraphics;
		m_pResources = a_pResources;
		m_pGui = a_pGui;
	}

	cFontManager::~cFontManager()
	{
		DestroyAll();
		Log("Done with fonts");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iFontData *cFontManager::CreateFontData(const tString &a_sName, int a_lSize, unsigned short a_lFirstChar,
								unsigned short a_lLastChar)
	{
		tString sPath;
		iFontData *pFont;
		tString a_sNewName = cString::ToLowerCase(a_sName);

		BeginLoad(a_sName);

		pFont = static_cast<iFontData*>(this->FindLoadedResource(a_sNewName, sPath));

		if (pFont==NULL && sPath!="")
		{
			pFont = m_pGraphics->GetLowLevel()->CreateFontData(a_sNewName);
			pFont->SetUp(m_pGraphics->GetDrawer(), m_pLowLevelResources, m_pGui);

			tString sExt = cString::ToLowerCase(cString::GetFileExt(a_sName));

			if (sExt == "ttf")
			{
				if (pFont->CreateFromFontFile(sPath, a_lSize, a_lFirstChar, a_lLastChar)==false)
				{
					efeDelete(pFont);
					EndLoad();
					return NULL;
				}
			}
			else
			{
				Error("Font '%s' has an uknown extension!\n", a_sName.c_str());
				efeDelete(pFont);
				EndLoad();
				return NULL;
			}
			AddResource(pFont);
		}

		if (pFont)pFont->IncUserCount();
		else Error("Couldn't create font '%s'\n", a_sName.c_str());

		EndLoad();
		return pFont;
	}

	//--------------------------------------------------------------

	iResourceBase *cFontManager::Create(const tString &a_sName)
	{
		return CreateFontData(a_sName, 16, 32, 255);
	}

	//--------------------------------------------------------------

	void cFontManager::Unload(iResourceBase *a_pResource)
	{
	
	}

	//--------------------------------------------------------------

	void cFontManager::Destroy(iResourceBase *a_pResource)
	{
		a_pResource->DecUserCount();

		if (a_pResource->HasUsers()==false)
		{
			RemoveResource(a_pResource);
			efeDelete(a_pResource);
		}
	}
}