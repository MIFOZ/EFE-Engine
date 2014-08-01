#ifndef EFE_WIDGET_TEXT_BOX_H
#define EFE_WIDGET_TEXT_BOX_H

#include "gui/Widget.h"

namespace efe
{
	class cGuiSkinFont;

	class cWidgetTextBox : public iWidget
	{
	public:
		cWidgetTextBox(cGuiSet *a_pSet, cGuiSkin *a_pSkin);
		virtual ~cWidgetTextBox();

		void SetMaxTextLength(int a_lLength);
		int GetMaxTextLength(){return m_lMaxCharacters;}

		cVector2f GetBackgroundSize();

		void SetCanEdit(bool a_bX);
		bool GetCanEdit(){return m_bCanEdit;}

	protected:
		int WorldToCharPos(const cVector2f &a_vWorldPos);
		float CharToLocalPos(int a_lChar);
		void SetMarkerPos(int a_lPos);

		int GetLastCharInSize(int a_lStartPos, float a_fMaxSize, float a_fLengthAdd);
		int GetFirstCharInSize(int a_lStartPos, float a_fMaxSize, float a_fLengthAdd);

		void OnLoadGraphics();
		void OnChangeSize();
		void OnChangeText();

		void OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion);

		bool OnMouseMove(cGuiMessageData &a_Data);
		bool OnMouseDown(cGuiMessageData &a_Data);
		bool OnMouseDoubleClick(cGuiMessageData &a_Data);
		bool OnMouseUp(cGuiMessageData &a_Data);
		bool OnMouseEnter(cGuiMessageData &a_Data);
		bool OnMouseLeave(cGuiMessageData &a_Data);

		bool OnLostFocus(cGuiMessageData &a_Data);

		bool OnKeyPress(cGuiMessageData &a_Data);

		// Data
		bool m_bPressed;
		int m_lMarkerCharPos;
		int m_lSelectedTextEnd;
		int m_lFirstVisibleChar;
		int m_lVisibleCharSize;
		float m_fTextMaxSize;

		int m_lMaxCharacters;

		float m_fMaxTextSizeNeg;

		bool m_bCanEdit;

		cGuiGfxElement *m_pGfxMarker;
		cGuiGfxElement *m_pGfxSelectedTextBack;

		cGuiGfxElement *m_pGfxBackground;

		cGuiGfxElement *m_vGfxBorders[4];
		cGuiGfxElement *m_vGfxCorners[4];
	};
};
#endif