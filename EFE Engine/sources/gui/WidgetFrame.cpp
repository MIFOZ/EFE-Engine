#include "gui/WidgetFrame.h"

#include "system/LowLevelSystem.h"

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

	cWidgetFrame::cWidgetFrame(cGuiSet *a_pSet, cGuiSkin *a_pSkin) : iWidget(eWidgetType_ComboBox, a_pSet, a_pSkin)
	{
		m_bClipsGraphics = true;

		m_bDrawFrame = false;

		m_bDrawBackground = false;
		m_fBackgroundZ = -0.5;
		m_BackgroundColor = cColor(1,1);
	}

	//--------------------------------------------------------------

	cWidgetFrame::~cWidgetFrame()
	{
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetFrame::OnLoadGraphics()
	{
		m_pGfxBackground = m_pSkin->GetGfx(eGuiSkinGfx_WindowBackground);

		m_vGfxBorders[0] = m_pSkin->GetGfx(eGuiSkinGfx_WindowBorderRight);
		m_vGfxBorders[1] = m_pSkin->GetGfx(eGuiSkinGfx_WindowBorderLeft);
		m_vGfxBorders[2] = m_pSkin->GetGfx(eGuiSkinGfx_WindowBorderUp);
		m_vGfxBorders[3] = m_pSkin->GetGfx(eGuiSkinGfx_WindowBorderDown);

		m_vGfxCorners[0] = m_pSkin->GetGfx(eGuiSkinGfx_WindowCornerLU);
		m_vGfxCorners[1] = m_pSkin->GetGfx(eGuiSkinGfx_WindowCornerRU);
		m_vGfxCorners[2] = m_pSkin->GetGfx(eGuiSkinGfx_WindowCornerRD);
		m_vGfxCorners[3] = m_pSkin->GetGfx(eGuiSkinGfx_WindowCornerLD);
	}

	//--------------------------------------------------------------

	void cWidgetFrame::OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion)
	{
		// Borders
		if (m_bDrawFrame)
		{
			DrawBordersAndCorners(NULL,m_vGfxBorders,m_vGfxCorners,GetGlobalPosition() -
				cVector3f(m_vGfxCorners[0]->GetActiveSize().x,
				m_vGfxCorners[0]->GetActiveSize().y,0),
				m_vSize + m_vGfxCorners[0]->GetActiveSize() +
				m_vGfxCorners[2]->GetActiveSize());
		}
	}

	//--------------------------------------------------------------

	void cWidgetFrame::OnDrawAfterClip(float a_fTimeStep, cGuiClipRegion *a_pClipRegion)
	{
		// Background
		if (m_bDrawBackground)
			m_pSet->DrawGfx(m_pGfxBackground,GetGlobalPosition() + cVector3f(0,0,m_fBackgroundZ),
			m_vSize, m_BackgroundColor);
	}

	//--------------------------------------------------------------

	bool cWidgetFrame::OnMouseMove(cGuiMessageData &a_Data)
	{
		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetFrame::OnMouseDown(cGuiMessageData &a_Data)
	{
		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetFrame::OnMouseUp(cGuiMessageData &a_Data)
	{
		return true;
	}
	
	//--------------------------------------------------------------

	bool cWidgetFrame::OnMouseEnter(cGuiMessageData &a_Data)
	{
		return false;
	}
	
	//--------------------------------------------------------------

	bool cWidgetFrame::OnMouseLeave(cGuiMessageData &a_Data)
	{
		return false;
	}
}