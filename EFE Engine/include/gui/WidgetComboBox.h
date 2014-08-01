#ifndef EFE_WIDGET_COMBO_BOX_H
#define EFE_WIDGET_COMBO_BOX_H

#include "gui/Widget.h"
#include "gui/WidgetBaseClasses.h"

namespace efe
{
	class cGuiSkinFont;

	class cWidgetTextBox;
	class cWidgetButton;
	class cWidgetSlider;

	class cWidgetComboBox : public iWidget, public iWidgetItemContainer
	{
	public:
		cWidgetComboBox(cGuiSet *a_pSet, cGuiSkin *a_pSkin);
		virtual ~cWidgetComboBox();

		void SetSelectedItem(int a_lX, bool a_bMoveList = false);
		int GetSelectedItem(){return m_lSelectedItem;}

		void SetCanEdit(bool a_bX);
		bool GetCanEdit();

		void SetMaxShownItems(int a_lX);
		int GetMaxShownItems(){return m_lMaxItems;}

	protected:
		// Own functions
		void UpdateProperties();

		void OpenMenu();
		void CloseMenu();

		bool ButtonPress(iWidget *a_pWidget, cGuiMessageData &a_Data);
		kGuiCallbackDeclarationEnd(ButtonPress);

		bool DrawText(iWidget *a_pWidget, cGuiMessageData &a_Data);
		kGuiCallbackDeclarationEnd(DrawText);

		bool SliderMove(iWidget *a_pWidget, cGuiMessageData &a_Data);
		kGuiCallbackDeclarationEnd(SliderMove);

		bool SliderLostFocus(iWidget *a_pWidget, cGuiMessageData &a_Data);
		kGuiCallbackDeclarationEnd(SliderLostFocus);

		//Implemented functions
		void OnLoadGraphics();
		void OnChangeSize();
		void OnChangeText();
		void OnInit();

		void OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion);

		bool OnMouseMove(cGuiMessageData &a_Data);
		bool OnMouseDown(cGuiMessageData &a_Data);
		bool OnMouseUp(cGuiMessageData &a_Data);
		bool OnMouseEnter(cGuiMessageData &a_Data);
		bool OnMouseLeave(cGuiMessageData &a_Data);

		bool OnLostFocus(cGuiMessageData &a_Data);

		// Data
		cWidgetTextBox *m_pText;
		cWidgetButton *m_pButton;
		cWidgetSlider *m_pSlider;

		bool m_bMenuOpen;
		float m_fMenuHeight;

		float m_fButtonWidth;
		float m_fSliderWidth;

		int m_lMouseOverSelection;
		int m_lSelectedItem;
		int m_lFirstItem;
		int m_lMaxItems;
		int m_lItemsShown;

		cGuiGfxElement *m_pGfxBackground;

		cGuiGfxElement *m_pGfxSelection;

		cGuiGfxElement *m_vGfxBorders[4];
		cGuiGfxElement *m_vGfxCorners[4];
	};
};

#endif