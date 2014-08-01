#include "graphics/FontData.h"

#include "system/LowLevelSystem.h"

#include "graphics/GraphicsDrawer.h"
#include "graphics/GfxObject.h"

#include "resources/ResourceImage.h"

#include "gui/GuiGfxElement.h"
#include "gui/Gui.h"

#include <stdarg.h>

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGlyph::cGlyph(cGfxObject *a_pObject, cGuiGfxElement *a_pGuiGfx, const cVector2f &a_vOffset,
			const cVector2f &a_vSize, float a_fAdvance)
	{
		m_pGfxObject = a_pObject;
		m_pGuiGfx = a_pGuiGfx;
		m_vOffset = a_vOffset;
		m_vSize = a_vSize;
		m_fAdvance = a_fAdvance;
	}
	
	cGlyph::~cGlyph()
	{
		if (m_pGfxObject) efeDelete(m_pGfxObject);
		if (m_pGuiGfx) efeDelete(m_pGuiGfx);
	}

	//--------------------------------------------------------------

	iFontData::iFontData(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics) : iResourceBase(a_sName,0)
	{
		m_pLowLevelGraphics = a_pLowLevelGraphics;
	}

	//--------------------------------------------------------------

	iFontData::~iFontData()
	{
		for (int i=0;i<(int)m_vGlyphs.size();i++)
			if (m_vGlyphs[i]) efeDelete(m_vGlyphs[i]);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void iFontData::Draw(const cVector3f &a_vPos, const cVector2f &a_vSize, const cColor &a_Col, eFontAlign m_Align,
					const wchar_t *fmt, ...)
	{
		wchar_t sText[256];
		va_list ap;
		if (fmt==NULL) return;
		va_start(ap, fmt);
		vswprintf(sText, 255, fmt, ap);
		va_end(ap);

		int lCount = 0;
		int lXAdd = 0;
		cVector3f vPos = a_vPos;

		if (m_Align == eFontAlign_Center)
			vPos.x -= GetLength(a_vSize, sText) / 2;
		else if(m_Align == eFontAlign_Right)
			vPos.x -= GetLength(a_vSize, sText);

		while (sText[lCount] != 0)
		{
			wchar_t lGlyphNum = ((wchar_t)sText[lCount]);
			if (lGlyphNum<m_lFirstChar || lGlyphNum>m_lLastChar)
			{
				lCount++;
				continue;
			}
			lGlyphNum -= m_lFirstChar;

			cGlyph *pGlyph = m_vGlyphs[lGlyphNum];
			if (pGlyph)
			{
				cVector2f vOffset(pGlyph->m_vOffset * a_vSize);
				cVector2f vSize(pGlyph->m_vSize * a_vSize);

				m_pGraphicsDrawer->DrawGfxObject(pGlyph->m_pGfxObject, vPos + vOffset, vSize, a_Col);

				vPos.x += pGlyph->m_fAdvance*a_vSize.x;
			}
			lCount++;
		}
	}

	float iFontData::GetLength(const cVector2f &a_vSize, const wchar_t *sText)
	{
		int lCount=0;
		float lXAdd=0;
		float fLength = 0;
		while (sText[lCount] != 0)
		{
			unsigned short lGlyphNum = ((wchar_t)sText[lCount]);
			if (lGlyphNum<m_lFirstChar || lGlyphNum>m_lLastChar)
			{
				lCount++;
				continue;
			}
			lGlyphNum -= m_lFirstChar;

			cGlyph *pGlyph = m_vGlyphs[lGlyphNum];
			if (pGlyph)
			{
				cVector2f vOffset(pGlyph->m_vOffset * a_vSize);
				cVector2f vSize(pGlyph->m_vSize * a_vSize);

				fLength += pGlyph->m_fAdvance*a_vSize.x;
			}
			lCount++;
		}
		return fLength;
	}

	//--------------------------------------------------------------

	cGlyph *iFontData::CreateGlyph(iBitmap2D *a_pBmp, const cVector2l &a_vOffset, const cVector2l &a_vSize,
			const cVector2l &a_vFontSize, int a_lAdvance)
	{
		cGfxObject *pObject = m_pGraphicsDrawer->CreateGfxObject(a_pBmp,"fontnormal",false);

		cGuiGfxElement *pGuiGfx =  m_pGui->CreateGfxFilledRect(cColor(1,1),eGuiMaterial_FontNormal,false);
		cResourceImage *pImage = pObject->GetMaterial()->GetImage(eMaterialTexture_Diffuse);
		pImage->IncUserCount();
		pGuiGfx->AddImage(pImage);

		cVector2f vSize;
		vSize.x = ((float)a_vSize.x)/((float)a_vFontSize.x) * m_vSizeRatio.x;
		vSize.y = ((float)a_vSize.y)/((float)a_vFontSize.y) * m_vSizeRatio.y;

		cVector2f vOffset;
		vOffset.x = ((float)a_vOffset.x)/((float)a_vFontSize.x);
		vOffset.y = ((float)a_vOffset.y)/((float)a_vFontSize.y);

		float fAdvance = ((float)a_lAdvance)/((float)a_vFontSize.x) * m_vSizeRatio.x;

		cGlyph *pGlyph = efeNew(cGlyph, (pObject,pGuiGfx,vOffset,vSize,fAdvance));

		return pGlyph;
	}

	//--------------------------------------------------------------

	void iFontData::GetWordWrapRows(float a_fLength, float a_fFontHeight, cVector2f a_vSize, const tWString &a_sString,
			tWStringVec *a_pRowVec)
	{
		int rows = 0;

		unsigned int pos;
		unsigned int first_letter = 0;
		unsigned int last_space = 0;

		tUIntList RowLengthList;

		float fTextLength;

		for (pos = 0;pos < a_sString.size(); pos++)
		{
			if (a_sString[pos] == _W(' ') || a_sString[pos] == _W('\n'))
			{
				tWString temp = a_sString.substr(first_letter, pos-first_letter);
				fTextLength = GetLength(a_vSize, temp.c_str());

				bool nothing = true;
				if (fTextLength > a_fLength)
				{
					rows++;
					RowLengthList.push_back(last_space);
					first_letter = last_space + 1;
					last_space = pos;
					nothing = false;
				}
				if (a_sString[pos] == _W('\n'))
				{
					last_space = pos;
					first_letter = last_space + 1;
					RowLengthList.push_back(last_space);
					rows++;
					nothing = false;
				}
				if (nothing)
					last_space = pos;
			}
		}

		tWString temp = a_sString.substr(first_letter, pos-first_letter);
		fTextLength = GetLength(a_vSize, temp.c_str());

		if (fTextLength > a_fLength)
		{
			rows++;
			RowLengthList.push_back(last_space);
		}
		else
		{
			first_letter = 0;
			unsigned int i = 0;

			for (tUIntListIt it = RowLengthList.begin();it != RowLengthList.end();++it)
			{
				a_pRowVec->push_back(a_sString.substr(first_letter, *it-first_letter).c_str());
				i++;
				first_letter = *it+1;
			}
			a_pRowVec->push_back(a_sString.substr(first_letter).c_str());
		}
	}

	void iFontData::AddGlyph(cGlyph *a_pGlyph)
	{
		m_vGlyphs.push_back(a_pGlyph);
	}
}