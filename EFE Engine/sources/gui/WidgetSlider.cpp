#include "gui/WidgetSlider.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "system/String.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

#include "gui/WidgetButton.h"

#include "graphics/FontData.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTOR
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	cWidgetSlider::cWidgetSlider(cGuiSet *a_pSet, cGuiSkin *a_pSkin, eWidgetSliderOrientation a_Orientation)
		: iWidget(eWidgetType_Slider, a_pSet, a_pSkin)
	{
		m_Orientation = a_Orientation;

		m_bPressed = false;

		m_lValue = 0;
		m_lMaxValue = 10;
		m_lButtonValueAdd = 1;
		m_lBarValueSize = 1;

		for (int i=0; i<2; i++) m_vButtons[i] = NULL;

		m_fButtonSize = m_pSkin->GetAttribute(eGuiSkinAttribute_SliderButtonSize).x;

		LoadGraphics();
	}

	//-------------------------------------------------------------

	cWidgetSlider::~cWidgetSlider()
	{
		if (m_pSet->IsDestroyingSet() == false)
			for (int i=0; i<2; i++) m_pSet->DestroyWidget(m_vButtons[i]);
	}

	//-------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	void cWidgetSlider::SetValue(int a_lValue)
	{
		if (m_lValue == a_lValue) return;

		m_lValue = a_lValue;
		if (m_lValue < 0) m_lValue = 0;
		if (m_lValue > m_lMaxValue) m_lValue = m_lMaxValue;

		cGuiMessageData data = cGuiMessageData(m_lValue);
		ProcessMessage(eGuiMessage_SliderMove, data);
		UpdateBarProperties();
	}

	//-------------------------------------------------------------

	void cWidgetSlider::SetMaxValue(int a_lMax)
	{
		if (m_lMaxValue == a_lMax) return;

		m_lMaxValue = a_lMax;
		if (m_lMaxValue < 0) m_lMaxValue = 0;

		if (m_lBarValueSize > m_lMaxValue) m_lBarValueSize = m_lMaxValue;

		UpdateBarProperties();
	}

	//-------------------------------------------------------------

	void cWidgetSlider::SetButtonValueAdd(int a_lAdd)
	{
		m_lButtonValueAdd = a_lAdd;
	}

	//-------------------------------------------------------------

	void cWidgetSlider::SetBarValueSize(int a_lSize)
	{
		if (m_lBarValueSize == a_lSize) return;

		m_lBarValueSize = a_lSize;
		if (m_lBarValueSize > m_lMaxValue+1) m_lBarValueSize = m_lMaxValue+1;

		UpdateBarProperties();
	}

	//-------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	void cWidgetSlider::UpdateBarProperties()
	{
		if (m_Orientation == eWidgetSliderOrientation_Vertical)
		{
			m_fSliderSize = m_vSize.y - m_fButtonSize*2;

			m_vBarSize = cVector2f(m_vSize.x,
				((float)m_lBarValueSize / (float)(m_lMaxValue+1)) * (m_fSliderSize));

			float fMinSize = m_vGfxCorners[0]->GetActiveSize().y + m_vGfxCorners[3]->GetActiveSize().y + 2;
			if (m_vBarSize.y < fMinSize) m_vBarSize.y = fMinSize;

			if (m_lMaxValue > 0)
				m_fValueStep = (1 / (float)(m_lMaxValue)) * (m_fSliderSize - m_vBarSize.y);
			else
				m_fValueStep = 0;

			m_vBarPos = cVector3f(0, m_fButtonSize + m_fValueStep*(float)m_lValue,0.2f);
		}
		else
		{
			m_fSliderSize = m_vSize.x - m_fButtonSize*2;

			m_vBarSize = cVector2f(((float)m_lBarValueSize / (float)(m_lMaxValue+1)) * (m_fSliderSize),
				m_vSize.y);

			float fMinSize = m_vGfxCorners[0]->GetActiveSize().x + m_vGfxCorners[3]->GetActiveSize().x + 2;
			if (m_vBarSize.x < fMinSize) m_vBarSize.x = fMinSize;

			if (m_lMaxValue > 0)
				m_fValueStep = (1 / (float)(m_lMaxValue)) * (m_fSliderSize - m_vBarSize.x);
			else
				m_fValueStep = 0;

			m_vBarPos = cVector3f(m_fButtonSize + m_fValueStep*(float)m_lValue,0,0.2f);
		}

		m_BarRect.x = GetGlobalPosition().x + m_vBarPos.x;
		m_BarRect.y = GetGlobalPosition().y + m_vBarPos.y;
		m_BarRect.w = m_vBarSize.x;
		m_BarRect.h = m_vBarSize.y;

		m_fMaxPos = m_fButtonSize + m_fValueStep * (float)m_lMaxValue;
		m_fMinPos = m_fButtonSize;
	}

	//-------------------------------------------------------------

	bool cWidgetSlider::ArrowButtonDown(iWidget *a_pWidget, cGuiMessageData &a_pData)
	{
		int i;
		for (i=0; i<2; ++i) if (m_vButtons[i] == a_pWidget) break;

		if (i==0) SetValue(m_lValue-1);
		if (i==1) SetValue(m_lValue+1);

		cGuiMessageData data = cGuiMessageData(m_lValue);
		ProcessMessage(eGuiMessage_SliderMove, data);

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetSlider, ArrowButtonDown)

	//-------------------------------------------------------------

	void cWidgetSlider::OnInit()
	{
		m_vButtons[0] = m_pSet->CreateWidgetButton(0,0,_W(""), this);
		m_vButtons[1] = m_pSet->CreateWidgetButton(0,0,_W(""), this);

		for (int i=0; i<2; i++)
		{
			m_vButtons[i]->SetImage(m_vGfxArrow[i],false);
			
			m_vButtons[i]->AddCallback(eGuiMessage_MouseDown, this, kGuiCallback(ArrowButtonDown));
		}

		OnChangeSize();
	}

	//-------------------------------------------------------------

	void cWidgetSlider::OnLoadGraphics()
	{
		m_pGfxButtonBackground = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpBackground);

		m_vGfxBorders[0] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderRight);
		m_vGfxBorders[1] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderLeft);
		m_vGfxBorders[2] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderUp);
		m_vGfxBorders[3] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderDown);

		m_vGfxCorners[0] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerLU);
		m_vGfxCorners[1] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerRU);
		m_vGfxCorners[2] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerRD);
		m_vGfxCorners[3] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerLD);

		// Vertical
		if (m_Orientation == eWidgetSliderOrientation_Vertical)
		{
			m_vGfxArrow[0] = m_pSkin->GetGfx(eGuiSkinGfx_SliderVertArrowUp);
			m_vGfxArrow[1] = m_pSkin->GetGfx(eGuiSkinGfx_SliderVertArrowDown);
			m_pGfxBackground = m_pSkin->GetGfx(eGuiSkinGfx_SliderVertBackground);
		}
		// Horizontal
		else
		{
			m_vGfxArrow[0] = m_pSkin->GetGfx(eGuiSkinGfx_SliderHoriArrowUp);
			m_vGfxArrow[1] = m_pSkin->GetGfx(eGuiSkinGfx_SliderHoriArrowDown);
			m_pGfxBackground = m_pSkin->GetGfx(eGuiSkinGfx_SliderHoriBackground);
		}

		m_fButtonSize = m_pSkin->GetAttribute(eGuiSkinAttribute_SliderButtonSize).x;
	}

	//-------------------------------------------------------------

	void cWidgetSlider::OnChangeSize()
	{
		if (m_vButtons[0] == NULL || m_vButtons[1] == NULL) return;

		// Vertical
		if (m_Orientation == eWidgetSliderOrientation_Vertical)
		{
			m_vButtons[0]->SetPosition(cVector3f(0,0,0.2f));
			m_vButtons[0]->SetSize(cVector2f(m_vSize.x, m_fButtonSize));

			m_vButtons[1]->SetPosition(cVector3f(0,m_vSize.y - m_fButtonSize,0.2f));
			m_vButtons[1]->SetSize(cVector2f(m_vSize.x, m_fButtonSize));
		}
		// Horizontal
		else
		{
			m_vButtons[0]->SetPosition(cVector3f(0,0,0.2f));
			m_vButtons[0]->SetSize(cVector2f(m_fButtonSize, m_vSize.y));

			m_vButtons[1]->SetPosition(cVector3f(m_vSize.x - m_fButtonSize,0,0.2f));
			m_vButtons[1]->SetSize(cVector2f(m_fButtonSize, m_vSize.y));
		}

		UpdateBarProperties();
	}

	//-------------------------------------------------------------

	void cWidgetSlider::OnChangePosition()
	{
		UpdateBarProperties();
	}

	//-------------------------------------------------------------

	void cWidgetSlider::OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion)
	{
		// Vertical
		if (m_Orientation == eWidgetSliderOrientation_Vertical)
		{
			m_pSet->DrawGfx(m_pGfxBackground, GetGlobalPosition() + 
				cVector3f(0,m_fButtonSize,0.1f),
				cVector2f(m_vSize.x,m_vSize.y - m_fButtonSize*2));
		}
		// Horizontal
		else
		{
			m_pSet->DrawGfx(m_pGfxBackground, GetGlobalPosition() + 
				cVector3f(m_fButtonSize,0,0.1f),
				cVector2f(m_vSize.x - m_fButtonSize*2,m_vSize.y));
		}

		DrawBordersAndCorners(m_pGfxButtonBackground, m_vGfxBorders, m_vGfxCorners,
			GetGlobalPosition() + m_vBarPos, m_vBarSize);
	}

	//-------------------------------------------------------------

	bool cWidgetSlider::OnMouseMove(cGuiMessageData &a_Data)
	{
		if (m_bPressed)
		{
			int lVal;
			// Vertical
			if (m_Orientation == eWidgetSliderOrientation_Vertical)
			{
				m_vBarPos.y = WorldToLocalPosition(a_Data.m_vPos).y + m_vRelMousePos.y;

				if (m_vBarPos.y > m_fMaxPos) m_vBarPos.y = m_fMaxPos;
				if (m_vBarPos.y < m_fMinPos) m_vBarPos.y = m_fMinPos;

				m_BarRect.y = GetGlobalPosition().y + m_vBarPos.y;
				lVal = (int)((m_vBarPos.y-m_fButtonSize) / m_fValueStep + 0.5f);
			}
			// Horizontal
			else
			{
				m_vBarPos.x = WorldToLocalPosition(a_Data.m_vPos).x + m_vRelMousePos.x;

				if (m_vBarPos.x > m_fMaxPos) m_vBarPos.x = m_fMaxPos;
				if (m_vBarPos.x < m_fMinPos) m_vBarPos.x = m_fMinPos;

				m_BarRect.x = GetGlobalPosition().x + m_vBarPos.x;
				lVal = (int)((m_vBarPos.x-m_fButtonSize) / m_fValueStep + 0.5f);
			}

			if (lVal > m_lMaxValue) lVal = m_lMaxValue;
			if (lVal < 0) lVal = 0;
			if (lVal != m_lValue)
			{
				m_lValue = lVal;
				cGuiMessageData data = cGuiMessageData(m_lValue);
				ProcessMessage(eGuiMessage_SliderMove, data);
			}
		}

		return true;
	}

	//-------------------------------------------------------------

	bool cWidgetSlider::OnMouseDown(cGuiMessageData &a_Data)
	{
		if (cMath::PointBoxCollision(a_Data.m_vPos, m_BarRect))
		{
			m_bPressed = true;
			cVector3f vRel = m_vBarPos - WorldToLocalPosition(a_Data.m_vPos);
			m_vRelMousePos.x = vRel.x;
			m_vRelMousePos.y = vRel.y;
		}

		return true;
	}

	//-------------------------------------------------------------

	bool cWidgetSlider::OnMouseUp(cGuiMessageData &a_Data)
	{
		if (m_bPressed)
		{
			m_bPressed = false;
			UpdateBarProperties();
		}

		return true;
	}

	//-------------------------------------------------------------

	bool cWidgetSlider::OnMouseEnter(cGuiMessageData &a_Data)
	{
		return false;
	}

	//-------------------------------------------------------------

	bool cWidgetSlider::OnMouseLeave(cGuiMessageData &a_Data)
	{
		return false;
	}

	bool cWidgetSlider::OnLostFocus(cGuiMessageData &a_Data)
	{
		if (m_bPressed)
		{
			m_bPressed = false;
			UpdateBarProperties();
		}

		return false;
	}
}