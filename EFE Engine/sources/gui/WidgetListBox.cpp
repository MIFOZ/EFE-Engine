#include "gui/WidgetListBox.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

#include "gui/WidgetSlider.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cWidgetListBox::cWidgetListBox(cGuiSet *a_pSet, cGuiSkin *a_pSkin) : iWidget(eWidgetType_ListBox, a_pSet, a_pSkin)
	{
		m_bClipsGraphics = true;

		m_fBackgroundZ = -0.5;

		m_lFirstItem = 0;
		m_lMaxItems = 1;

		m_lSelectedItem = -1;

		m_fSliderWidth = m_pSkin->GetAttribute(eGuiSkinAttribute_ListBoxSliderWidth).x;

		m_pSlider = NULL;
	}

	//--------------------------------------------------------------

	cWidgetListBox::~cWidgetListBox()
	{
		if (m_pSet->IsDestroyingSet() == false)
			m_pSet->DestroyWidget(m_pSlider);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetListBox::SetSelectedItem(int a_lX,bool a_bMoveList)
	{
		if (m_lSelectedItem == a_lX) return;

		m_lSelectedItem = a_lX;

		if (a_bMoveList && m_lSelectedItem >= m_lFirstItem + m_lMaxItems)
		{
			while(m_lSelectedItem >= m_lFirstItem + m_lMaxItems)
			{
				m_lFirstItem++;
			}
			m_pSlider->SetValue(m_lFirstItem);
		}
		if (a_bMoveList && m_lSelectedItem < m_lFirstItem &&  m_lSelectedItem >= 0)
		{
			while(m_lSelectedItem < m_lFirstItem)
			{
				m_lFirstItem--;
			}
			m_pSlider->SetValue(m_lSelectedItem);
		}

		cGuiMessageData data = cGuiMessageData(m_lSelectedItem);
		ProcessMessage(eGuiMessage_SelectionChange,data);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetListBox::UpdateProperties()
	{
		m_lMaxItems = (int)(m_vSize.y / (m_vDefaultFontSize.y + 2));

		if ((int)m_vItems.size() > m_lMaxItems)
		{
			m_pSlider->SetBarValueSize(m_lMaxItems);
			m_pSlider->SetMaxValue((int)m_vItems.size() - m_lMaxItems);
		}
		else
		{
			m_pSlider->SetMaxValue(0);
			m_pSlider->SetBarValueSize(1);
		}
	}

	//--------------------------------------------------------------

	bool cWidgetListBox::DrawText(iWidget *a_pWidget, cGuiMessageData &a_Data)
	{
		cVector3f vPosition = GetGlobalPosition() + cVector3f(3,2,0);
		for (int i = m_lFirstItem; i < (int)m_vItems.size(); ++i)
		{
			if (i - m_lFirstItem > m_lMaxItems) break;

			if (i == m_lSelectedItem)
			{
				m_pSet->DrawGfx(m_pGfxSelection, vPosition - cVector3f(3,0,0),
					cVector2f(m_vSize.x, m_vDefaultFontSize.y));
			}

			DrawDefaultText(m_vItems[i],vPosition,eFontAlign_Left);
			vPosition.y += m_vDefaultFontSize.y + 2;
		}

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetListBox, DrawText)

	//--------------------------------------------------------------

	bool cWidgetListBox::MoveSlider(iWidget *a_pWidget, cGuiMessageData &a_Data)
	{
		m_lFirstItem = a_Data.m_lVal;

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetListBox, MoveSlider)

	//--------------------------------------------------------------

	void cWidgetListBox::OnInit()
	{
		m_pSlider = m_pSet->CreateWidgetSlider(eWidgetSliderOrientation_Vertical,0,0,0,this);
		m_pSlider->AddCallback(eGuiMessage_SliderMove,this,kGuiCallback(MoveSlider));

		AddCallback(eGuiMessage_OnDraw,this,kGuiCallback(DrawText));

		OnChangeSize();
	}

	//--------------------------------------------------------------

	void cWidgetListBox::OnLoadGraphics()
	{
		m_pGfxBackground = m_pSkin->GetGfx(eGuiSkinGfx_ListBoxBackground);
		m_pGfxSelection = m_pSkin->GetGfx(eGuiSkinGfx_TextBoxSelectedTextBack);

		m_vGfxBorders[0] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderRight);
		m_vGfxBorders[1] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderLeft);
		m_vGfxBorders[2] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderUp);
		m_vGfxBorders[3] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderDown);

		m_vGfxCorners[0] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerLU);
		m_vGfxCorners[1] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerRU);
		m_vGfxCorners[2] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerRD);
		m_vGfxCorners[3] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerLD);
	}

	//--------------------------------------------------------------

	void cWidgetListBox::OnChangeSize()
	{
		if (m_pSlider)
		{
			m_pSlider->SetSize(cVector2f(m_fSliderWidth,m_vSize.y));
			m_pSlider->SetPosition(cVector3f(m_vSize.x - m_fSliderWidth, 0, 0.2f));

			UpdateProperties();
		}
	}

	//--------------------------------------------------------------

	void cWidgetListBox::OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion)
	{
		// Background
		m_pSet->DrawGfx(m_pGfxBackground,GetGlobalPosition() + cVector3f(0,0,m_fBackgroundZ),
			m_vSize);

		// Borders
		DrawBordersAndCorners(NULL,  m_vGfxBorders,m_vGfxCorners,
			GetGlobalPosition() - 
			cVector3f(	m_vGfxCorners[0]->GetActiveSize().x,
						m_vGfxCorners[0]->GetActiveSize().y,0),
			m_vSize +	m_vGfxCorners[0]->GetActiveSize() +
						m_vGfxCorners[2]->GetActiveSize());
	}

	//--------------------------------------------------------------

	bool cWidgetListBox::OnMouseMove(cGuiMessageData &a_Data)
	{
		return true;
	}

	//--------------------------------------------------------------
	
	bool cWidgetListBox::OnMouseDown(cGuiMessageData &a_Data)
	{
		cVector3f vLocalPos = WorldToLocalPosition(a_Data.m_vPos);

		int lSelection = (int)((vLocalPos.y - 2) / (m_vDefaultFontSize.y + 2));
		if (lSelection < 0) lSelection = 0;

		lSelection = lSelection + m_lFirstItem;

		if (lSelection >= (int)m_vItems.size()) lSelection = (int)m_vItems.size() - 1;

		SetSelectedItem(lSelection);

		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetListBox::OnMouseUp(cGuiMessageData &a_Data)
	{
		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetListBox::OnMouseEnter(cGuiMessageData &a_Data)
	{
		return false;
	}

	//--------------------------------------------------------------

	bool cWidgetListBox::OnMouseLeave(cGuiMessageData &a_Data)
	{
		return false;
	}

	//--------------------------------------------------------------

	bool cWidgetListBox::OnKeyPress(cGuiMessageData &a_Data)
	{
		eKey key = a_Data.m_KeyPress.m_Key;

		//if (key == ekey

		return true;
	}
}