#ifndef EFE_GUI_POP_UP
#define EFE_GUI_POP_UP

#include "gui/GuiTypes.h"

namespace efe
{
	class cGuiSet;
	class cGuiSkin;

	class iGuiPopUp
	{
	public:
		iGuiPopUp(cGuiSet *a_pSet);
		virtual ~iGuiPopUp();

	protected:
		void SelfDestruct();

		cGuiSet *m_pSet;
		cGuiSkin *m_pSkin;
	};
};
#endif