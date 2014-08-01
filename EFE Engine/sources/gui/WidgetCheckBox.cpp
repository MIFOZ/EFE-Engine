#include "gui/WidgetCheckBox.h"

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

	cWidgetCheckBox::cWidgetCheckBox(cGuiSet *a_pSet, cGuiSkin *a_pSkin) : iWidget(eWidgetType_ComboBox, a_pSet, a_pSkin)
	{
		m_bChecked = false;
		m_bPressed = false;

		LoadGraphics();
	}

	//--------------------------------------------------------------

	cWidgetCheckBox::~cWidgetCheckBox()
	{
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetCheckBox::SetChecked(bool a_bX)
	{
		if (m_bChecked ==  a_bX) return;

		m_bChecked = a_bX;

		cGuiMessageData data = cGuiMessageData(m_bChecked);
		ProcessMessage(eGuiMessage_CheckChange, data);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetCheckBox::OnLoadGraphics()
	{
		m_vGfxBox[1][0] = m_pSkin->GetGfx(eGuiSkinGfx_CheckBoxEnabledUnchecked);
		m_vGfxBox[1][1] = m_pSkin->GetGfx(eGuiSkinGfx_CheckBoxEnabledChecked);
		m_vGfxBox[0][0] = m_pSkin->GetGfx(eGuiSkinGfx_CheckBoxDisabledUnchecked);
		m_vGfxBox[0][1] = m_pSkin->GetGfx(eGuiSkinGfx_CheckBoxDisabledChecked);
	}

	//--------------------------------------------------------------

	void cWidgetCheckBox::OnChangeSize()
	{
		if (m_vSize.y < m_vDefaultFontSize.y) m_vSize.y = m_vDefaultFontSize.y;
		if (m_vSize.x < m_vGfxBox[0][0]->GetActiveSize().x) m_vSize.x = m_vGfxBox[0][0]->GetActiveSize().x;
	}

	//--------------------------------------------------------------

	void cWidgetCheckBox::OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion)
	{
		// Box
		int lEnabled = IsEnabled() ? 1 : 0;
		m_pSet->DrawGfx(m_vGfxBox[lEnabled][m_bChecked], GetGlobalPosition() +
			cVector3f(0,m_vSize.y/2 - m_vGfxBox[0][0]->GetActiveSize().y/2,0));

		// Text
		eGuiSkinFont font = IsEnabled() ? eGuiSkinFont_Default : eGuiSkinFont_Disabled;
		DrawSkinText(m_sText, font, GetGlobalPosition() +
			cVector3f(m_vGfxBox[0][0]->GetActiveSize().x + 3.0f,
			m_vSize.y/2 - m_vDefaultFontSize.y/2,0),
			eFontAlign_Left);
	}

	//--------------------------------------------------------------

	bool cWidgetCheckBox::OnMouseMove(cGuiMessageData &a_Data)
	{
		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetCheckBox::OnMouseDown(cGuiMessageData &a_Data)
	{
		m_bPressed = true;
		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetCheckBox::OnMouseUp(cGuiMessageData &a_Data)
	{
		if (m_bPressed) SetChecked(!m_bChecked);

		m_bPressed = false;
		return true;
	}
	
	//--------------------------------------------------------------

	bool cWidgetCheckBox::OnMouseEnter(cGuiMessageData &a_Data)
	{
		return false;
	}
	
	//--------------------------------------------------------------

	bool cWidgetCheckBox::OnMouseLeave(cGuiMessageData &a_Data)
	{
		m_bPressed = false;
		return false;
	}
}