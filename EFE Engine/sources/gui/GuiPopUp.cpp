#include "gui/GuiPopUp.h"

#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"

namespace efe
{
	iGuiPopUp::iGuiPopUp(cGuiSet *a_pSet)
	{
		m_pSet = a_pSet;
		m_pSkin = m_pSet->GetSkin();
	}

	iGuiPopUp::~iGuiPopUp()
	{
	}

	void iGuiPopUp::SelfDestruct()
	{
		m_pSet->DestroyPopUp(this);
	}
}