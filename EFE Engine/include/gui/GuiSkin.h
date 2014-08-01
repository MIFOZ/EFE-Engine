#ifndef EFE_GUI_SKIN_H
#define EFE_GUI_SKIN_H

#include <map>

#include "gui/GuiTypes.h"

namespace efe
{
	class cGui;
	class cGuiGfxElement;
	class iFontData;
	class iGuiMaterial;

	class cGuiSkinFont
	{
	public:
		cGuiSkinFont(cGui *a_pGui);
		~cGuiSkinFont();

		iFontData *m_pFont;
		cVector2f m_vSize;
		cColor m_Color;
		iGuiMaterial *m_pMaterial;
	private:
		cGui *m_pGui;
	};

	class cGuiSkin
	{
	public:
		cGuiSkin(const tString &a_sName, cGui *a_pGui);
		~cGuiSkin();

		const tString &GetName(){return m_sName;}

		bool LoadFromFile(const tString &a_sFile);

		cGuiGfxElement *GetGfx(eGuiSkinGfx a_Type);
		cGuiSkinFont *GetFont(eGuiSkinFont a_Type);
		const cVector3f &GetAttribute(eGuiSkinAttribute a_Type);
	private:
		tString m_sName;
		cGui *m_pGui;

		std::vector<cGuiGfxElement*> m_vGfxElements;
		std::vector<cGuiSkinFont*> m_vFonts;
		std::vector<cVector3f> m_vAttributes;
	};
};

#endif