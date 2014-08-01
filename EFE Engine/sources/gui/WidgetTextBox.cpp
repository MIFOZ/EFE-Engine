#include "gui/WidgetTextBox.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "graphics/FontData.h"

#include "system/String.h"

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

	cWidgetTextBox::cWidgetTextBox(cGuiSet *a_pSet, cGuiSkin *a_pSkin) : iWidget(eWidgetType_TextBox, a_pSet, a_pSkin)
	{
		LoadGraphics();

		m_lMarkerCharPos = -1;
		m_lSelectedTextEnd = -1;

		m_lFirstVisibleChar = 0;
		m_lVisibleCharSize = 0;

		m_fTextMaxSize = 0;
		m_fMaxTextSizeNeg = 0;

		m_lMaxCharacters = -1;

		m_lVisibleCharSize = 0;

		m_bPressed = false;

		m_bCanEdit = true;

		m_pPointerGfx = m_pSkin->GetGfx(eGuiSkinGfx_PointerText);
	}

	//--------------------------------------------------------------

	cWidgetTextBox::~cWidgetTextBox()
	{
	
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetTextBox::SetMaxTextLength(int a_lLength)
	{
		if (m_lMaxCharacters == a_lLength) return;

		m_lMaxCharacters = a_lLength;

		if (m_lMaxCharacters >= 0 && (int)m_sText.size() > m_lMaxCharacters)
		{
			SetText(cString::SubW(m_sText, 0, m_lMaxCharacters));

			if (m_lSelectedTextEnd >= m_lMaxCharacters) m_lSelectedTextEnd = m_lMaxCharacters-1;
			if (m_lMarkerCharPos >= m_lMaxCharacters) m_lMarkerCharPos = m_lMaxCharacters-1;

			OnChangeText();
		}
	}

	//--------------------------------------------------------------

	cVector2f cWidgetTextBox::GetBackgroundSize()
	{
		return cVector2f(m_vSize.x - m_vGfxCorners[0]->GetActiveSize().x -
									m_vGfxCorners[1]->GetActiveSize().x,
						m_vSize.y - m_vGfxCorners[0]->GetActiveSize().y -
									m_vGfxCorners[2]->GetActiveSize().y);
	}

	//--------------------------------------------------------------

	void cWidgetTextBox::SetCanEdit(bool a_bX)
	{
		m_bCanEdit = a_bX;

		if (m_bCanEdit) m_pPointerGfx = m_pSkin->GetGfx(eGuiSkinGfx_PointerText);
		else			m_pPointerGfx = m_pSkin->GetGfx(eGuiSkinGfx_PointerNormal);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	int cWidgetTextBox::WorldToCharPos(const cVector2f &a_vWorldPos)
	{
		float fTextPos =	WorldToLocalPosition(a_vWorldPos).x - 
							m_vGfxCorners[0]->GetActiveSize().x + 3;

		int lMarkerCharPos;
		if (fTextPos > 0)
			lMarkerCharPos = GetLastCharInSize(m_lFirstVisibleChar, fTextPos, 3.0f);
		else
			lMarkerCharPos = m_lFirstVisibleChar;

		return lMarkerCharPos;
	}

	//--------------------------------------------------------------

	float cWidgetTextBox::CharToLocalPos(int a_lChar)
	{
		float fMarkerPos = -2;
		if (a_lChar > 0 && a_lChar - m_lFirstVisibleChar > 0)
			fMarkerPos = m_pDefaultFontType->GetLength(m_vDefaultFontSize, 
			cString::SubW(m_sText, m_lFirstVisibleChar, a_lChar - m_lFirstVisibleChar).c_str());

		return fMarkerPos;

	}

	void cWidgetTextBox::SetMarkerPos(int a_lPos)
	{
		m_lMarkerCharPos = a_lPos;
		if (m_lMarkerCharPos < 0) m_lMarkerCharPos = 0;
		if (m_lMarkerCharPos > (int)m_sText.size() && m_sText.size() > 0) m_lMarkerCharPos = (int)m_sText.size();

		if (m_lMarkerCharPos > m_lFirstVisibleChar + m_lVisibleCharSize)
		{
			m_lFirstVisibleChar = GetFirstCharInSize(m_lMarkerCharPos, m_fTextMaxSize, 0)+1;
			if (m_sText.size()<=1) m_lFirstVisibleChar = 0;
			OnChangeText();
		}
		else if(m_lMarkerCharPos < m_lFirstVisibleChar)
		{
			m_lFirstVisibleChar = m_lMarkerCharPos;
			OnChangeText();
		}
	}

	//--------------------------------------------------------------

	int cWidgetTextBox::GetLastCharInSize(int a_lStartPos, float a_fMaxSize, float a_fLengthAdd)
	{
		int lCharPos = (int)m_sText.size();
		float fLength = 0;
		int lFirst = m_pDefaultFontType->GetFirstChar();
		int lLast = m_pDefaultFontType->GetLastChar();
		for (int i=a_lStartPos;i<(int)m_sText.size();++i)
		{
			//if (i < lFirst || i > lLast) continue;

			cGlyph *pGlyph = m_pDefaultFontType->GetGlyph(m_sText[i] - lFirst);
			if (pGlyph==NULL) continue;

			fLength += pGlyph->m_fAdvance * m_vDefaultFontSize.x;
			if (fLength + a_fLengthAdd >= a_fMaxSize)
			{
				lCharPos = i;
				break;
			}
		}

		return lCharPos;
	}

	//--------------------------------------------------------------

	int cWidgetTextBox::GetFirstCharInSize(int a_lStartPos, float a_fMaxSize, float a_fLengthAdd)
	{
		int lCharPos = 0;
		float fLength = 0;
		int lFirst = m_pDefaultFontType->GetFirstChar();
		int lLast = m_pDefaultFontType->GetLastChar();
		for (int i=a_lStartPos;i>=0;--i)
		{
			if (i < lFirst || i > lLast) continue;

			cGlyph *pGlyph = m_pDefaultFontType->GetGlyph(m_sText[i] - lFirst);
			if (pGlyph==NULL) continue;

			fLength += pGlyph->m_fAdvance * m_vDefaultFontSize.x;
			if (fLength + a_fLengthAdd >= a_fMaxSize)
			{
				lCharPos = i;
				break;
			}
		}

		return lCharPos;
	}

	//--------------------------------------------------------------

	void cWidgetTextBox::OnLoadGraphics()
	{
		m_pGfxMarker = m_pSkin->GetGfx(eGuiSkinGfx_TextBoxMarker);

		m_pGfxSelectedTextBack = m_pSkin->GetGfx(eGuiSkinGfx_TextBoxSelectedTextBack);

		m_pGfxBackground = m_pSkin->GetGfx(eGuiSkinGfx_TextBoxBackground);

		m_vGfxBorders[0] = m_pSkin->GetGfx(eGuiSkinGfx_FrameBorderRight);
		m_vGfxBorders[1] = m_pSkin->GetGfx(eGuiSkinGfx_FrameBorderLeft);
		m_vGfxBorders[2] = m_pSkin->GetGfx(eGuiSkinGfx_FrameBorderUp);
		m_vGfxBorders[3] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonDownBorderDown);

		m_vGfxCorners[0] = m_pSkin->GetGfx(eGuiSkinGfx_FrameCornerLU);
		m_vGfxCorners[1] = m_pSkin->GetGfx(eGuiSkinGfx_FrameCornerRU);
		m_vGfxCorners[2] = m_pSkin->GetGfx(eGuiSkinGfx_FrameCornerRD);
		m_vGfxCorners[3] = m_pSkin->GetGfx(eGuiSkinGfx_FrameCornerLD);

		OnChangeSize();
	}

	//--------------------------------------------------------------

	void cWidgetTextBox::OnChangeSize()
	{
		m_vSize.y = m_vGfxBorders[0]->GetActiveSize().y +
					m_vGfxBorders[2]->GetActiveSize().y +
					m_vDefaultFontSize.y + 2 * 2;

		m_fTextMaxSize = m_vSize.x - m_vGfxBorders[0]->GetActiveSize().x -
			m_vGfxBorders[1]->GetActiveSize().x - 3 * 2 -
			m_fMaxTextSizeNeg;

		OnChangeText();
	}

	//--------------------------------------------------------------

	void cWidgetTextBox::OnChangeText()
	{
		if (m_sText==_W(""))
			m_lFirstVisibleChar = 0;
		else
			m_lVisibleCharSize = GetLastCharInSize(	m_lFirstVisibleChar, m_fTextMaxSize,0) -
													m_lFirstVisibleChar;

		if (m_lMaxCharacters >= 0 && (int)m_sText.size() > m_lMaxCharacters)
		{
			SetText(cString::SubW(m_sText, 0, m_lMaxCharacters));

			if (m_lSelectedTextEnd >= m_lMaxCharacters) m_lSelectedTextEnd = m_lMaxCharacters-1;
			if (m_lMarkerCharPos >= m_lMaxCharacters) m_lMarkerCharPos = m_lMaxCharacters-1;
		}
	}

	//--------------------------------------------------------------

	void cWidgetTextBox::OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion)
	{
		cVector3f vTextAdd = cVector3f(3,2,0.2f) + m_vGfxCorners[0]->GetActiveSize();
		DrawDefaultText(cString::SubW(m_sText, m_lFirstVisibleChar, m_lVisibleCharSize),
						GetGlobalPosition() + vTextAdd,
						eFontAlign_Left);

		//Marker
		if (m_lMarkerCharPos >= 0)
		{
			float fMarkerPos = CharToLocalPos(m_lMarkerCharPos);
			m_pSet->DrawGfx(m_pGfxMarker, GetGlobalPosition() + vTextAdd + cVector3f(fMarkerPos,0,0.1f),
							cVector2f(2, m_vDefaultFontSize.y));

			//Selected text
			if (m_lSelectedTextEnd >= 0)
			{
				float fSelectEnd = CharToLocalPos(m_lSelectedTextEnd);

				float fPos = fSelectEnd < fMarkerPos ? fSelectEnd : fMarkerPos;
				float fEnd = fSelectEnd > fMarkerPos ? fSelectEnd : fMarkerPos;

				if (fPos < 0)fPos = 0;
				if (fEnd > m_fTextMaxSize) fEnd = m_fTextMaxSize;

				float fSize = fEnd - fPos;

				m_pSet->DrawGfx(m_pGfxSelectedTextBack, GetGlobalPosition() + 
								vTextAdd + cVector3f(fPos,0,0.2f),
								cVector2f(fSize, m_vDefaultFontSize.y));
			}
		}

		DrawBordersAndCorners(m_pGfxBackground, m_vGfxBorders, m_vGfxCorners,
								GetGlobalPosition(), m_vSize);
	}

	//--------------------------------------------------------------

	bool cWidgetTextBox::OnMouseMove(cGuiMessageData &a_Data)
	{
		if (m_bPressed)
		{
			int lPos = WorldToCharPos(a_Data.m_vPos);
			if (lPos != m_lMarkerCharPos)
			{
				if (m_lSelectedTextEnd == -1) m_lSelectedTextEnd = m_lMarkerCharPos;
				SetMarkerPos(lPos);
			}
		}

		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetTextBox::OnMouseDown(cGuiMessageData &a_Data)
	{
		if ((a_Data.m_lVal & eGuiMouseButton_Left) == 0) return true;
		if (m_bCanEdit==false) return true;

		SetMarkerPos(WorldToCharPos(a_Data.m_vPos));
		m_lSelectedTextEnd = -1;
		m_bPressed = true;
		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetTextBox::OnMouseDoubleClick(cGuiMessageData &a_Data)
	{
		if ((a_Data.m_lVal & eGuiMouseButton_Left) == 0) return true;
		if (m_bCanEdit==false) return true;

		SetMarkerPos(WorldToCharPos(a_Data.m_vPos));

		if (m_sText[m_lMarkerCharPos] == _W(' ')) return true;

		m_lSelectedTextEnd = 0;
		for (size_t i = m_lMarkerCharPos; i > 0;  --i)
		{
			if (m_sText[i] == _W(' '))
			{
				m_lSelectedTextEnd = (int)i + 1;
				break;
			}
		}

		for (size_t i = m_lMarkerCharPos+1; i < (int)m_sText.size(); ++i)
		{
			if (m_sText[i] == _W(' ') || i == (int)m_sText.size()-1)
			{
				if (i==(int)m_sText.size()-1)	SetMarkerPos((int)m_sText.size()-1);
				else							SetMarkerPos((int)i);
				break;
			}
		}

		m_bPressed = false;

		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetTextBox::OnMouseUp(cGuiMessageData &a_Data)
	{
		if ((a_Data.m_lVal & eGuiMouseButton_Left) == 0) return true;
		if (m_bCanEdit==false) return true;

		m_bPressed = false;
		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetTextBox::OnMouseEnter(cGuiMessageData &a_Data)
	{
		m_pSet->SetCurrentPointer(m_pSkin->GetGfx(eGuiSkinGfx_PointerText));

		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetTextBox::OnMouseLeave(cGuiMessageData &a_Data)
	{
		m_pSet->SetCurrentPointer(m_pSkin->GetGfx(eGuiSkinGfx_PointerNormal));
		m_bPressed = false;

		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetTextBox::OnLostFocus(cGuiMessageData &a_Data)
	{
		m_lMarkerCharPos = -1;
		m_lSelectedTextEnd = -1;
		return false;
	}

	bool cWidgetTextBox::OnKeyPress(cGuiMessageData &a_Data)
	{
		if (m_bCanEdit==false) return true;
		if (m_lMarkerCharPos < 0) return false;

		eKey key = a_Data.m_KeyPress.m_Key;
		int mod = a_Data.m_KeyPress.m_lModifier;

		if (m_pGfxMarker) m_pGfxMarker->SetAnimationTime(0);

		//Copy / Paste / Cut
		if (mod & eKeyModifier_CTRL)
		{
			int lStart = m_lMarkerCharPos < m_lSelectedTextEnd ? m_lMarkerCharPos : m_lSelectedTextEnd;
			int lEnd = m_lMarkerCharPos > m_lSelectedTextEnd ? m_lMarkerCharPos : m_lSelectedTextEnd;
			int lSelectSize = lEnd - lStart;

			// Select all
			if (key == eKey_a)
			{
				m_lSelectedTextEnd = 0;
				m_lMarkerCharPos = (int)m_sText.size()-1;
			}
			// Copy
			else if (key == eKey_c)
			{
				if (m_lSelectedTextEnd >= 0)
					CopyTextToClipboard(cString::SubW(m_sText, lStart, lSelectSize));
			}
			// Cut
			else if (key == eKey_x)
			{
				if (m_lSelectedTextEnd >= 0)
				{
					CopyTextToClipboard(cString::SubW(m_sText, lStart, lSelectSize));
					SetText(cString::SubW(m_sText, 0, lStart) + cString::SubW(m_sText, lEnd));
					m_lSelectedTextEnd = -1;
				}
			}
			// Paste
			else if (key == eKey_v)
			{
				tWString sExtra = LoadTextFromClipboard();

				if (m_lSelectedTextEnd < 0)
				{
					if (m_lMaxCharacters == -1 ||
						(int)m_sText.size() + (int)sExtra.size() <= m_lMaxCharacters)
					{
						SetText(cString::SubW(m_sText, 0, m_lMarkerCharPos) + sExtra +
							cString::SubW(m_sText, m_lMarkerCharPos));

						SetMarkerPos(m_lMarkerCharPos + (int)sExtra.size());
					}
				}
				else
				{
					if (m_lMaxCharacters < 0 ||
						(int)sExtra.size() <= lSelectSize ||
						(int)m_sText.size() + (int)sExtra.size() - lSelectSize <= m_lMaxCharacters)
					{
						SetText(cString::SubW(m_sText, 0, lStart) + sExtra +
							cString::SubW(m_sText, lEnd));

						m_lSelectedTextEnd = -1;
						SetMarkerPos(lStart + (int)sExtra.size());
					}
				}
			}
		}
		// Arrows keys
		else if (key == eKey_LEFT || key == eKey_RIGHT)
		{
			if (mod & eKeyModifier_SHIFT)
			{
				if (m_lSelectedTextEnd == -1)
					m_lSelectedTextEnd = m_lMarkerCharPos;

				if (key == eKey_LEFT)	SetMarkerPos(m_lMarkerCharPos-1);
				else					SetMarkerPos(m_lMarkerCharPos+1);
			}
			else
			{
				if (key == eKey_LEFT)	SetMarkerPos(m_lMarkerCharPos-1);
				else					SetMarkerPos(m_lMarkerCharPos+1);

				m_lSelectedTextEnd = -1;
			}
		}
		// Delete and backspace
		else if (key == eKey_DELETE || key == eKey_BACKSPACE)
		{
			if (m_lSelectedTextEnd >= 0)
			{
				int lStart = m_lMarkerCharPos < m_lSelectedTextEnd ? m_lMarkerCharPos : m_lSelectedTextEnd;
				int lEnd = m_lMarkerCharPos > m_lSelectedTextEnd ? m_lMarkerCharPos : m_lSelectedTextEnd;

				SetText(cString::SubW(m_sText, 0, lStart) + cString::SubW(m_sText,lEnd));

				m_lSelectedTextEnd = -1;
				SetMarkerPos(lStart);
			}
			else
			{
				if (key == eKey_DELETE)
					SetText(cString::SubW(m_sText,0, m_lMarkerCharPos) +
							cString::SubW(m_sText,m_lMarkerCharPos+1));
				else
				{
					SetText(cString::SubW(m_sText,0, m_lMarkerCharPos-1) +
							cString::SubW(m_sText,m_lMarkerCharPos));
					SetMarkerPos(m_lMarkerCharPos-1);
				}
			}
		}
		// Character
		else
		{
			int lFirstFontChar = m_pDefaultFontType->GetFirstChar();
			int lLastFontChar = m_pDefaultFontType->GetLastChar();
			wchar_t unicode = a_Data.m_KeyPress.m_lUnicode;

			if (unicode >= lFirstFontChar && unicode <= lLastFontChar &&
				m_pDefaultFontType->GetGlyph(unicode - lFirstFontChar))
			{
				if (m_lSelectedTextEnd < 0)
				{
					if (m_lMaxCharacters == -1 || (int)m_sText.size() < m_lMaxCharacters)
					{
						SetText(	cString::SubW(m_sText, 0, m_lMarkerCharPos) + unicode +
								 cString::SubW(m_sText,m_lMarkerCharPos));

						SetMarkerPos(m_lMarkerCharPos+1);
					}
				}
				else
				{
					int lStart = m_lMarkerCharPos < m_lSelectedTextEnd ? m_lMarkerCharPos : m_lSelectedTextEnd;
					int lEnd = m_lMarkerCharPos > m_lSelectedTextEnd ? m_lMarkerCharPos : m_lSelectedTextEnd;

					SetText(cString::SubW(m_sText, 0, lStart) + unicode +
						cString::SubW(m_sText,lEnd));

					m_lSelectedTextEnd = -1;

					SetMarkerPos(lStart + 1);
				}
			}
		}

		return true;
	}
}