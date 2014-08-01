#include "gui/WidgetLabel.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "graphics/FontData.h"

#include "math/Math.h"

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

	cWidgetLabel::cWidgetLabel(cGuiSet *a_pSet, cGuiSkin *a_pSkin) : iWidget(eWidgetType_Label, a_pSet, a_pSkin)
	{
		m_pWordWrap = false;
		m_TextAlign = eFontAlign_Left;

		m_lMaxCharacters = -1;
	}

	cWidgetLabel::~cWidgetLabel()
	{
		
	}
	
	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetLabel::SetMaxTextLength(int a_lLength)
	{
		if (m_lMaxCharacters == a_lLength) return;

		m_lMaxCharacters = a_lLength;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetLabel::OnLoadGraphics()
	{
		
	}

	void cWidgetLabel::OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion)
	{
		cVector3f vOffset = 0;

		if (m_TextAlign == eFontAlign_Right) vOffset.x += m_vSize.x;

		if (m_pWordWrap)
		{
			int lChars = 0;
			bool bEnable = IsEnabled();
			float fHeight = m_vDefaultFontSize.x+2;
			tWStringVec vRows;
			m_pDefaultFontType->GetWordWrapRows(m_vSize.x, fHeight,
								m_vDefaultFontSize, m_sText,
								&vRows);

			for (size_t i = 0; i < vRows.size(); ++i)
			{
				bool bBreak = false;
				if (m_lMaxCharacters >= 0)
				{
					if (lChars + (int)vRows[i].length() > m_lMaxCharacters)
					{
						vRows[i] = cString::SubW(vRows[i], 0, m_lMaxCharacters - lChars);
						bBreak = true;
					}
					lChars += (int)vRows[i].length();
				}

				if (bEnable)
					DrawDefaultText(vRows[i], GetGlobalPosition()+vOffset, m_TextAlign);
				else
					DrawSkinText(vRows[i], eGuiSkinFont_Disabled, GetGlobalPosition()+vOffset, m_TextAlign);
				vOffset.y += fHeight;

				if (bBreak) break;
			}
		}
		else
		{
			if (m_lMaxCharacters >= 0 && (int)m_sText.length() > m_lMaxCharacters)
			{
				if (IsEnabled())
					DrawDefaultText(cString::SubW(m_sText, 0, m_lMaxCharacters), GetGlobalPosition()+vOffset, m_TextAlign);
				else
					DrawSkinText(cString::SubW(m_sText, 0, m_lMaxCharacters), eGuiSkinFont_Disabled, GetGlobalPosition()+vOffset, m_TextAlign);
			}
			else
				if (IsEnabled())
					DrawDefaultText(m_sText, GetGlobalPosition()+vOffset, m_TextAlign);
				else
					DrawSkinText(m_sText, eGuiSkinFont_Disabled, GetGlobalPosition()+vOffset, m_TextAlign);
		}
	}

	bool cWidgetLabel::OnMouseMove(cGuiMessageData &a_Data)
	{
		return false;
	}

	bool cWidgetLabel::OnMouseDown(cGuiMessageData &a_Data)
	{
		return false;
	}

	bool cWidgetLabel::OnMouseUp(cGuiMessageData &a_Data)
	{
		return false;
	}

	bool cWidgetLabel::OnMouseEnter(cGuiMessageData &a_Data)
	{
		return false;
	}

	bool cWidgetLabel::OnMouseLeave(cGuiMessageData &a_Data)
	{
		return false;
	}
}