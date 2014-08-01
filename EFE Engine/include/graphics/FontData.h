#ifndef EFE_FONTDATA_H
#define EFE_FONTDATA_H

#include <vector>
#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"
#include "resources/ResourceBase.h"
#include "resources/LowLevelResources.h"

namespace efe
{
	class ImageManager;
	class iLowLevelGraphics;
	class cGraphicsDrawer;
	class cGfxObject;
	class iBitmap2D;
	class cGuiGfxElement;
	class cGui;

	class cGlyph
	{
	public:
		cGlyph(cGfxObject *a_pObject, cGuiGfxElement *a_pGuiGfx, const cVector2f &a_vOffset,
			const cVector2f &a_vSize, float a_fAdvance);
		~cGlyph();

		cGfxObject *m_pGfxObject;
		cGuiGfxElement *m_pGuiGfx;
		cVector2f m_vOffset;
		cVector2f m_vSize;
		float m_fAdvance;
	};

	typedef std::vector<cGlyph*> tGlyphVec;
	typedef tGlyphVec::iterator tGlyphVecIt;

	class iFontData : public iResourceBase
	{
	public:
		iFontData(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics);
		~iFontData();

		virtual bool CreateFromFontFile(const tString &a_sFileName, int a_lSzie, unsigned short a_lFirstChar,
			unsigned short a_lLastChar)=0;

		virtual bool CreateFromBitmapFile(const tString &a_sFileName)=0;

		bool Reload(){return false;}
		void Unload(){}
		void Destroy(){}

		void SetUp(cGraphicsDrawer *a_pGraphicsDrawer, iLowLevelResources *a_pLowLevelResources,
					cGui *a_pGui)
		{
			m_pGraphicsDrawer = a_pGraphicsDrawer;
			m_pLowLevelResources = a_pLowLevelResources;
			m_pGui = a_pGui;
		}

		__forceinline cGlyph *GetGlyph(int a_lNum)const{return m_vGlyphs[a_lNum];}

		__forceinline unsigned short GetFirstChar(){return m_lFirstChar;}
		__forceinline unsigned short GetLastChar(){return m_lLastChar;}

		__forceinline const cVector2f GetSizeRatio()const{return m_vSizeRatio;}

		void Draw(const cVector3f &a_vPos, const cVector2f &a_vSize, const cColor &a_Col, eFontAlign m_Align,
					const wchar_t *fmt, ...);

		void GetWordWrapRows(float a_fLength, float a_fFontHeight, cVector2f a_vSize, const tWString &a_sString,
			tWStringVec *a_pRowVec);

		float GetLength(const cVector2f &a_vSize, const wchar_t *sText);
	protected:
		iLowLevelGraphics *m_pLowLevelGraphics;
		iLowLevelResources *m_pLowLevelResources;
		cGraphicsDrawer *m_pGraphicsDrawer;
		cGui *m_pGui;

		tGlyphVec m_vGlyphs;

		float m_fHeight;
		unsigned short m_lFirstChar;
		unsigned short m_lLastChar;

		cVector2f m_vSizeRatio;

		cGlyph *CreateGlyph(iBitmap2D *a_pBmp, const cVector2l &a_vOffset, const cVector2l &a_vSize,
			const cVector2l &a_vFontSize, int a_lAdvance);
		void AddGlyph(cGlyph *a_pGlyph);
	};
};
#endif