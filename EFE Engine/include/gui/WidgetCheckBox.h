#ifndef EFE_WIDGET_CHECK_BOX_H
#define EFE_WIDGET_CHECK_BOX_H

#include "gui/Widget.h"

namespace efe
{
	class cGuiSkinElement;

	class cWidgetCheckBox : public iWidget
	{
	public:
		cWidgetCheckBox(cGuiSet *a_pSet, cGuiSkin *a_pSkin);
		virtual ~cWidgetCheckBox();

		void SetChecked(bool a_bX);
		bool IsChecked(){return m_bChecked;}

	protected:
		// Implemented functions
		void OnLoadGraphics();
		void OnChangeSize();

		void OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion);

		bool OnMouseMove(cGuiMessageData &a_Data);
		bool OnMouseDown(cGuiMessageData &a_Data);
		bool OnMouseUp(cGuiMessageData &a_Data);
		bool OnMouseEnter(cGuiMessageData &a_Data);
		bool OnMouseLeave(cGuiMessageData &a_Data);

		// Data
		bool m_bChecked;
		bool m_bPressed;
		
		cGuiGfxElement *m_vGfxBox[2][2];
	};
};

#endif