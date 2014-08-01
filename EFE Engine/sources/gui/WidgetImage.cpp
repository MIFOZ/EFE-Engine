#include "gui/WidgetImage.h"

#include "system/LowLevelSystem.h"

//#include "math/Math.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cWidgetImage::cWidgetImage(cGuiSet *a_pSet, cGuiSkin *a_pSkin) : iWidget(eWidgetType_ComboBox, a_pSet, a_pSkin)
	{
		m_pGfxImage = NULL;
		m_Color = cColor(1,1);
	}

	//--------------------------------------------------------------

	cWidgetImage::~cWidgetImage()
	{
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetImage::SetImage(cGuiGfxElement *a_pGfx)
	{
		if (m_pGfxImage == a_pGfx) return;

		m_pGfxImage = a_pGfx;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetImage::OnLoadGraphics()
	{
	}

	//--------------------------------------------------------------

	void cWidgetImage::OnChangeSize()
	{
	}

	//--------------------------------------------------------------

	void cWidgetImage::OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion)
	{
		if (m_pGfxImage)
			m_pSet->DrawGfx(m_pGfxImage, GetGlobalPosition(), m_vSize, m_Color);
	}

	//--------------------------------------------------------------

	bool cWidgetImage::OnMouseMove(cGuiMessageData &a_Data)
	{
		return true;
	}

	bool cWidgetImage::OnMouseDown(cGuiMessageData &a_Data)
	{
		return true;
	}

	bool cWidgetImage::OnMouseUp(cGuiMessageData &a_Data)
	{
		return true;
	}

	bool cWidgetImage::OnMouseEnter(cGuiMessageData &a_Data)
	{
		return false;
	}

	bool cWidgetImage::OnMouseLeave(cGuiMessageData &a_Data)
	{
		return false;
	}
}