#include "graphics/GraphicsDrawer.h"
#include "system/LowLevelSystem.h"
#include "impl/DX11Bitmap2D.h"
#include "impl/tinyXML/tinyxml.h"

#include "impl/DXFontData.h"

#pragma comment(lib, "freetype255d.lib")

#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

namespace efe
{
	cDXFontData::cDXFontData(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics)
		: iFontData(a_sName, a_pLowLevelGraphics)
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cDXFontData::CreateFromBitmapFile(const tString &a_sFileName)
	{
		tString sPath = cString::GetFilePath(a_sFileName);

		TiXmlDocument *pXmlDoc = efeNew(TiXmlDocument, (a_sFileName.c_str()));

		if (pXmlDoc->LoadFile()==false)
		{
			Error("Couldn't load angle code font file '%s'\n", a_sFileName.c_str());
			efeDelete(pXmlDoc);
			return false;
		}

		TiXmlElement *pRootElem = pXmlDoc->RootElement();

		TiXmlElement *pCommonElem = pRootElem->FirstChildElement("common");

		int lLineHeight = cString::ToInt(pCommonElem->Attribute("lineHeight"),0);
		int lBase = cString::ToInt(pCommonElem->Attribute("base"),0);

		m_lFirstChar = 0;
		m_lLastChar = 3000;

		m_fHeight = (float)lLineHeight;

		m_vGlyphs.resize(3000, NULL);

		m_vSizeRatio.x = (float)lBase / (float)lLineHeight;
		m_vSizeRatio.y = 1;

		std::vector<cDX11Bitmap2D *> vBitmaps;

		TiXmlElement *pPagesRootElem = pRootElem->FirstChildElement("pages");

		TiXmlElement *pPagesElem = pPagesRootElem->FirstChildElement("page");
		for (; pPagesElem != NULL; pPagesElem = pPagesElem->NextSiblingElement("page"))
		{
			tString sFileName = pPagesElem->Attribute("file");
			tString sFilePath = cString::SetFilePath(sFileName, sPath);

			cDX11Bitmap2D *pBitmap = static_cast<cDX11Bitmap2D *> (m_pLowLevelResources->LoadBitmap2D(sFilePath));
			if (pBitmap==NULL)
			{
				Error("Couldn't load bitmap %s for FNT file '%s'\n", sFilePath.c_str(), a_sFileName.c_str());
				efeDelete(pXmlDoc);
				return false;
			}
			vBitmaps.push_back(pBitmap);
		}

		TiXmlElement *pCharsRootElem = pRootElem->FirstChildElement("chars");

		TiXmlElement *pCharElem = pCharsRootElem->FirstChildElement("char");
		for (;pCharElem != NULL; pCharElem = pCharElem->NextSiblingElement("char"))
		{
			int lId = cString::ToInt(pCharElem->Attribute("id"),0);
			int lX = cString::ToInt(pCharElem->Attribute("x"),0);
			int lY = cString::ToInt(pCharElem->Attribute("y"),0);

			int lW = cString::ToInt(pCharElem->Attribute("width"),0);
			int lH = cString::ToInt(pCharElem->Attribute("height"),0);

			int lXOffset = cString::ToInt(pCharElem->Attribute("xoffset"),0);
			int lYOffset = cString::ToInt(pCharElem->Attribute("yoffset"),0);

			int lAdvance = cString::ToInt(pCharElem->Attribute("xadvance"),0);

			int lPage = cString::ToInt(pCharElem->Attribute("page"),0);

			cDX11Bitmap2D *pSourceBitmap = vBitmaps[lPage];
		}
		return NULL;
	}

	bool cDXFontData::CreateFromFontFile(const tString &a_sFileName, int a_lSize, unsigned short a_lFirstChar,
							unsigned short a_lLastChar)
	{
		cGlyph *pGlyph = NULL;

		m_lFirstChar = a_lFirstChar;
		m_lLastChar = a_lLastChar;

		FT_Library lib;
		FT_Init_FreeType(&lib);

		FT_Face face;
		FT_New_Face(lib, a_sFileName.c_str(), 0, &face);
		FT_Set_Char_Size(face, a_lSize << 6, a_lSize << 6, 96, 96);

		FT_Select_Charmap(face,FT_ENCODING_UNICODE);

		m_vSizeRatio = 1;

		START_TIMING(FontData)
		
		for (int i=a_lFirstChar;i<=a_lLastChar;i++)
		{
			pGlyph = RenderGlyph(face, i, a_lSize);
			AddGlyph(pGlyph);
		}

		STOP_TIMING(FontData);

		m_fHeight = (float)(face->height>>6);

		//FT_Done_Glyph(face->glyph);
		return true;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGlyph *cDXFontData::RenderGlyph(FT_Face &a_Font, unsigned short a_Char, int a_lFontSize)
	{
		FT_Load_Char(a_Font, a_Char, FT_LOAD_RENDER);

		cVector2l vSize(a_Font->glyph->metrics.width>>6,a_Font->glyph->metrics.height>>6);

		int lAdvance = a_Font->glyph->advance.x>>6;

		cDX11Bitmap2D *pBmp = static_cast<cDX11Bitmap2D*>(m_pLowLevelGraphics->CreateBitmap2D(vSize, eFormat_RGBA8));

		pBmp->FillRect(cRectl(),cColor(1,0,0,1));

		unsigned char *pSrc = a_Font->glyph->bitmap.buffer;
		unsigned char *pDest = pBmp->GetPixels();

		int lSize = pBmp->GetWidth()*pBmp->GetHeight();

		for (int i=0;i<lSize;i++)
		{
			int lIdx = i*4;
			pDest[lIdx+0] = pSrc[i];
			pDest[lIdx+1] = pSrc[i];
			pDest[lIdx+2] = pSrc[i];
			pDest[lIdx+3] = pSrc[i];
		}

		tString lIdx = cString::ToString(a_Char);

		//pBmp->SaveBmp("Glyph_"+lIdx+".bmp");

		cVector2l vOffset(a_Font->glyph->metrics.horiBearingX>>6, a_lFontSize - (a_Font->glyph->metrics.horiBearingY>>6));

		cGlyph *pGlyph = CreateGlyph(pBmp, vOffset, vSize, a_lFontSize, lAdvance);

		efeDelete(pBmp);

		return pGlyph;
	}
}