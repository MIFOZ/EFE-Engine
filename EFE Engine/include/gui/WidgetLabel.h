#ifndef EFE_WIDGET_LABEL_H
#define EFE_WIDGET_LABEL_H

#include "gui/Widget.h"

namespace efe
{
	class cGuiSkinFont;

	class cWidgetLabel : public iWidget
	{
	public:
		cWidgetLabel(cGuiSet *a_pSet, cGuiSkin *a_pSkin);
		virtual ~cWidgetLabel();

		void SetTextAlign(eFontAlign a_Type){m_TextAlign = a_Type;}

		void SetMaxTextLength(int a_lLength);


	protected:
		void OnLoadGraphics();

		void OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion);

		bool OnMouseMove(cGuiMessageData &a_Data);
		bool OnMouseDown(cGuiMessageData &a_Data);
		bool OnMouseUp(cGuiMessageData &a_Data);
		bool OnMouseEnter(cGuiMessageData &a_Data);
		bool OnMouseLeave(cGuiMessageData &a_Data);

		eFontAlign m_TextAlign;
		bool m_pWordWrap;

		int m_lMaxCharacters;
	};
};
#endif
