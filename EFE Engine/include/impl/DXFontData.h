#ifndef EFE_D3D11_FONTDATA_H
#define EFE_D3D11_FONTDATA_H

#include "graphics/FontData.h"
#include <ft2build.h>
#include <freetype/freetype.h>

//struct FT_Face;

namespace efe
{
	class cDXFontData : public iFontData
	{
	public:
		cDXFontData(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics);
		cDXFontData();

		bool CreateFromFontFile(const tString &a_sFileName, int a_lSize, unsigned short a_lFirstChar,
							unsigned short a_lLastChar);
		bool CreateFromBitmapFile(const tString &a_sNameFile);

	private:
		cGlyph *RenderGlyph(FT_Face &a_Font, unsigned short a_Char, int a_lFontSize);
	};
};
#endif