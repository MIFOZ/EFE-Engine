#ifndef EFE_WIDGET_LIST_BOX_H
#define EFE_WIDGET_LIST_BOX_H

#include "gui/Widget.h"
#include "gui/WidgetBaseClasses.h"

namespace efe
{
	class cGuiSkinFont;

	class cWidgetSlider;

	class cWidgetListBox : public iWidget, public iWidgetItemContainer
	{
	public:
		cWidgetListBox(cGuiSet *a_pSet, cGuiSkin *a_pSkin);
		virtual ~cWidgetListBox();

		void SetBackgroundZ(float a_fZ){m_fBackgroundZ = a_fZ;}
		float GetBackgroundZ(){return m_fBackgroundZ;}

		void SetSelectedItem(int a_lX,bool a_bMoveList = false);
		int GetSelectedItem(){return m_lSelectedItem;}

	protected:
		// Own functions
		void UpdateProperties();

		bool DrawText(iWidget *a_pWidget, cGuiMessageData &a_Data);
		kGuiCallbackDeclarationEnd(DrawText);

		bool MoveSlider(iWidget *a_pWidget, cGuiMessageData &a_Data);
		kGuiCallbackDeclarationEnd(MoveSlider);

		// Implemented functions
		void OnInit();
		void OnLoadGraphics();
		void OnChangeSize();

		void OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion);

		bool OnMouseMove(cGuiMessageData &a_Data);
		bool OnMouseDown(cGuiMessageData &a_Data);
		bool OnMouseUp(cGuiMessageData &a_Data);
		bool OnMouseEnter(cGuiMessageData &a_Data);
		bool OnMouseLeave(cGuiMessageData &a_Data);

		bool OnKeyPress(cGuiMessageData &a_Data);

		// Data
		float m_fBackgroundZ;
		float m_fSliderWidth;

		int m_lFirstItem;
		int m_lMaxItems;

		int m_lSelectedItem;

		cGuiGfxElement *m_pGfxBackground;

		cGuiGfxElement *m_pGfxSelection;

		cGuiGfxElement *m_vGfxBorders[4];
		cGuiGfxElement *m_vGfxCorners[4];

		cWidgetSlider *m_pSlider;
	};
};

#endif