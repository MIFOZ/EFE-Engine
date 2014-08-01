#include "gui/WidgetComboBox.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

#include "gui/WidgetTextBox.h"
#include "gui/WidgetButton.h"
#include "gui/WidgetSlider.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cWidgetComboBox::cWidgetComboBox(cGuiSet *a_pSet, cGuiSkin *a_pSkin) : iWidget(eWidgetType_ComboBox, a_pSet, a_pSkin)
	{
		m_fButtonWidth = a_pSkin->GetAttribute(eGuiSkinAttribute_ComboBoxButtonWidth).x;
		m_fSliderWidth = a_pSkin->GetAttribute(eGuiSkinAttribute_ComboBoxSliderWidth).x;

		m_pText = NULL;
		m_pButton = NULL;
		m_pSlider = NULL;

		m_bMenuOpen = false;

		m_fMenuHeight = 0;

		m_lSelectedItem = -1;
		m_lMouseOverSelection = -1;

		m_lFirstItem = 0;
		m_lMaxItems = 12;
		m_lItemsShown = 0;
	}

	//--------------------------------------------------------------

	cWidgetComboBox::~cWidgetComboBox()
	{
		if (m_pSet->IsDestroyingSet() == false)
		{
			m_pSet->DestroyWidget(m_pText);
			m_pSet->DestroyWidget(m_pButton);
			m_pSet->DestroyWidget(m_pSlider);
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetComboBox::SetSelectedItem(int a_lX, bool a_bMoveList)
	{
		if (m_lSelectedItem == a_lX && (m_lSelectedItem < 0 ||
			m_pText->GetText() == m_vItems[m_lSelectedItem])) return;

		m_lSelectedItem = a_lX;

		if (a_bMoveList && m_lSelectedItem >= m_lFirstItem + m_lMaxItems)
		{
			while (m_lSelectedItem >= m_lFirstItem + m_lMaxItems)
			{
				m_lFirstItem++;
			}
			m_pSlider->SetValue(m_lFirstItem);
		}
		if (a_bMoveList && m_lSelectedItem < m_lFirstItem && m_lSelectedItem > 0)
		{
			while (m_lSelectedItem < m_lFirstItem)
			{
				m_lFirstItem--;
			}
			m_pSlider->SetValue(m_lSelectedItem);
		}

		if (m_lSelectedItem >= 0)
			SetText(m_vItems[m_lSelectedItem]);

		cGuiMessageData data = cGuiMessageData(m_lSelectedItem);
		ProcessMessage(eGuiMessage_SelectionChange, data);
	}

	//--------------------------------------------------------------

	void cWidgetComboBox::SetCanEdit(bool a_bX)
	{
		m_pText->SetCanEdit(a_bX);
	}

	//--------------------------------------------------------------

	bool cWidgetComboBox::GetCanEdit()
	{
		return m_pText->GetCanEdit();
	}

	//--------------------------------------------------------------

	void cWidgetComboBox::SetMaxShownItems(int a_lX)
	{
		m_lMaxItems = a_lX;

		UpdateProperties();
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetComboBox::UpdateProperties()
	{
		if ((int)m_vItems.size() <= m_lMaxItems)
			m_lItemsShown = (int)m_vItems.size();
		else
			m_lItemsShown = m_lMaxItems;

		m_fMenuHeight = 2 + (m_vDefaultFontSize.y + 2) * (float)m_lItemsShown + 2;

		OnChangeSize();
	}

	//--------------------------------------------------------------

	void cWidgetComboBox::OpenMenu()
	{
		if (m_vItems.empty()) return;

		if (m_bMenuOpen) return;
		m_pSet->SetAttentionWidget(this);
		m_bMenuOpen = true;

		m_lMouseOverSelection = m_lSelectedItem;

		m_vSize = cVector2f(m_vSize.x, m_vSize.y + m_fMenuHeight);

		if ((int)m_vItems.size() > m_lMaxItems)
		{
			m_pSlider->SetEnabled(true);
			m_pSlider->SetVisible(true);

			m_pSlider->SetPosition(cVector3f(m_vSize.x - 20 - m_vGfxBorders[1]->GetActiveSize().x,
				(m_vSize.y - m_fMenuHeight) + m_vGfxBorders[1]->GetActiveSize().y,
				1.2f));
			m_pSlider->SetSize(cVector2f(20,m_fMenuHeight - m_vGfxBorders[1]->GetActiveSize().y - 
				m_vGfxBorders[2]->GetActiveSize().y));

			m_pSlider->SetBarValueSize(m_lMaxItems);
			m_pSlider->SetMaxValue((int)m_vItems.size() - m_lMaxItems);

			m_pSet->SetFocusedWidget(m_pSlider);
		}
		else
			m_pSet->SetFocusedWidget(this);

		m_bClipsGraphics = true;
	}

	//--------------------------------------------------------------

	void cWidgetComboBox::CloseMenu()
	{
		if (m_bMenuOpen == false) return;

		m_pSet->SetAttentionWidget(NULL);
		m_bMenuOpen = false;

		m_vSize = m_pText->GetSize();

		m_pSlider->SetEnabled(false);
		m_pSlider->SetVisible(false);

		m_bClipsGraphics = false;
	}

	//--------------------------------------------------------------

	bool cWidgetComboBox::ButtonPress(iWidget *a_pWidget, cGuiMessageData &a_Data)
	{
		if (m_bMenuOpen)	CloseMenu();
		else				OpenMenu();

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetComboBox, ButtonPress)

	//--------------------------------------------------------------

	bool cWidgetComboBox::DrawText(iWidget *a_pWidget, cGuiMessageData &a_Data)
	{
		if (m_bMenuOpen == false) return false;

		cVector3f vPos = GetGlobalPosition() +
			cVector3f(m_vGfxBorders[0]->GetActiveSize().x+3,m_pText->GetSize().y+2,1.2f);

		for (int i = m_lFirstItem; i < (int)m_vItems.size(); ++i)
		{
			if (i - m_lFirstItem >= m_lMaxItems) break;

			if (i == m_lMouseOverSelection)
			{
				m_pSet->DrawGfx(m_pGfxSelection, vPos - cVector3f(3,0,0),
					cVector2f(m_vSize.x,m_vDefaultFontSize.y));
			}

			DrawDefaultText(m_vItems[i], vPos, eFontAlign_Left);
			vPos.y += m_vDefaultFontSize.y + 2;
		}

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetComboBox, DrawText)

	//--------------------------------------------------------------

	bool cWidgetComboBox::SliderMove(iWidget *a_pWidget, cGuiMessageData &a_Data)
	{
		m_lFirstItem = a_Data.m_lVal;

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetComboBox, SliderMove)

	//--------------------------------------------------------------

	bool cWidgetComboBox::SliderLostFocus(iWidget *a_pWidget, cGuiMessageData &a_Data)
	{
		if (m_bMenuOpen && GetMouseIsOver() == false)
			CloseMenu();

		return false;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetComboBox, SliderLostFocus)

	//--------------------------------------------------------------

	void cWidgetComboBox::OnLoadGraphics()
	{
		m_pGfxBackground = m_pSkin->GetGfx(eGuiSkinGfx_ComboBoxBackground);

		m_pGfxSelection = m_pSkin->GetGfx(eGuiSkinGfx_TextBoxSelectedTextBack);

		m_vGfxBorders[0] = m_pSkin->GetGfx(eGuiSkinGfx_ComboBoxBorderRight);
		m_vGfxBorders[1] = m_pSkin->GetGfx(eGuiSkinGfx_ComboBoxBorderLeft);
		m_vGfxBorders[2] = m_pSkin->GetGfx(eGuiSkinGfx_ComboBoxBorderUp);
		m_vGfxBorders[3] = m_pSkin->GetGfx(eGuiSkinGfx_ComboBoxBorderDown);

		m_vGfxCorners[0] = m_pSkin->GetGfx(eGuiSkinGfx_ComboBoxCornerLU);
		m_vGfxCorners[1] = m_pSkin->GetGfx(eGuiSkinGfx_ComboBoxCornerRU);
		m_vGfxCorners[2] = m_pSkin->GetGfx(eGuiSkinGfx_ComboBoxCornerRD);
		m_vGfxCorners[3] = m_pSkin->GetGfx(eGuiSkinGfx_ComboBoxCornerLD);
	}

	//--------------------------------------------------------------

	void cWidgetComboBox::OnChangeSize()
	{
		if (m_pText && m_pButton && m_pSlider)
		{
			m_pText->SetSize(m_vSize);
			m_vSize = m_pText->GetSize();

			cVector2f vBackSize = m_pText->GetBackgroundSize();

			m_pButton->SetSize(cVector2f(m_fButtonWidth, vBackSize.y));
			m_pButton->SetPosition(cVector3f(m_vSize.x - (m_vSize.x - vBackSize.x)/2 - m_fButtonWidth,
				(m_vSize.y - vBackSize.y)/2, 0.3f));

			m_pSlider->SetBarValueSize(m_lMaxItems);
			m_pSlider->SetMaxValue((int)m_vItems.size() - m_lMaxItems);
		}
	}

	//--------------------------------------------------------------

	void cWidgetComboBox::OnChangeText()
	{
		if (m_pText) m_pText->SetText(m_sText);
	}

	//--------------------------------------------------------------

	void cWidgetComboBox::OnInit()
	{
		m_pText = m_pSet->CreateWidgetTextBox(0,m_vSize,_W(""),this);
		m_pText->SetText(m_sText);
		m_pText->SetCanEdit(false);

		m_pButton = m_pSet->CreateWidgetButton(0,0,_W(""),this);
		m_pButton->SetImage(m_pSkin->GetGfx(eGuiSkinGfx_ComboBoxButtonIcon), false);
		m_pButton->AddCallback(eGuiMessage_MouseDown, this, kGuiCallback(ButtonPress));

		m_pSlider = m_pSet->CreateWidgetSlider(eWidgetSliderOrientation_Vertical,0,0,0,this);
		m_pSlider->AddCallback(eGuiMessage_SliderMove,this,kGuiCallback(SliderMove));
		m_pSlider->AddCallback(eGuiMessage_LostFocus,this,kGuiCallback(SliderLostFocus));
		m_pSlider->SetEnabled(false);
		m_pSlider->SetVisible(false);

		AddCallback(eGuiMessage_OnDraw,this,kGuiCallback(DrawText));

		OnChangeSize();
	}

	//--------------------------------------------------------------

	void cWidgetComboBox::OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion)
	{
		if (m_bMenuOpen)
		{
			m_pSet->DrawGfx(m_pGfxBackground,GetGlobalPosition() + cVector3f(0,m_vSize.y-m_fMenuHeight,1),
				cVector2f(m_vSize.x, m_fMenuHeight));

			DrawBordersAndCorners(NULL, m_vGfxBorders, m_vGfxCorners,
				GetGlobalPosition() + cVector3f(0,m_vSize.y-m_fMenuHeight,1.4f),
				cVector2f(m_vSize.x, m_fMenuHeight));
		}
	}

	//--------------------------------------------------------------

	bool cWidgetComboBox::OnMouseMove(cGuiMessageData &a_Data)
	{
		if (m_bMenuOpen==false) return false;

		if (GetMouseIsOver()==false) return false;

		cVector3f vLocalPos = WorldToLocalPosition(a_Data.m_vPos);

		if (vLocalPos.y <= m_pText->GetSize().y) return false;
		if (m_pSlider->IsEnabled() && vLocalPos.x >= m_vSize.x - 20) return false;

		float fToTextStart = 2 + m_pText->GetSize().y + m_vGfxCorners[0]->GetActiveSize().y;
		int lSelection = (int)((vLocalPos.y - fToTextStart) / (m_vDefaultFontSize.y + 2));
		if (lSelection < 0) lSelection = 0;

		lSelection = lSelection + m_lFirstItem;

		if (lSelection >= (int)m_vItems.size()) lSelection = (int)m_vItems.size()-1;

		m_lMouseOverSelection = lSelection;

		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetComboBox::OnMouseDown(cGuiMessageData &a_Data)
	{
		if (m_bMenuOpen==false) return false;

		cVector3f vLocal = WorldToLocalPosition(a_Data.m_vPos);

		if (vLocal.y < m_pText->GetSize().y)
			CloseMenu();

		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetComboBox::OnMouseUp(cGuiMessageData &a_Data)
	{
		if (m_bMenuOpen==false) return false;

		cVector3f vLocal = WorldToLocalPosition(a_Data.m_vPos);

		if (vLocal.y > m_pText->GetSize().y)
		{
			SetSelectedItem(m_lMouseOverSelection);
			CloseMenu();
		}

		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetComboBox::OnMouseEnter(cGuiMessageData &a_Data)
	{
		return false;
	}

	bool cWidgetComboBox::OnMouseLeave(cGuiMessageData &a_Data)
	{
		return false;
	}

	bool cWidgetComboBox::OnLostFocus(cGuiMessageData &a_Data)
	{
		cVector3f vLocal = WorldToLocalPosition(a_Data.m_vPos);

		if (m_bMenuOpen &&m_pSlider->IsEnabled()==false)
			CloseMenu();

		return false;
	}
}