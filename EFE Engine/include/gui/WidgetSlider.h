#ifndef EFE_WIDGET_SLIDER_H
#define EFE_WIDGET_SLIDER_H

#include "gui/Widget.h"

namespace efe
{
	class cGuiSkinFont;

	class cWidgetButton;

	class cWidgetSlider : public iWidget
	{
	public:
		cWidgetSlider(cGuiSet *a_pSet, cGuiSkin *a_pSkin, eWidgetSliderOrientation a_Orientation);
		virtual ~cWidgetSlider();

		int GetValue(){return m_lValue;}
		void SetValue(int a_lValue);

		int GetMaxValue(){return m_lMaxValue;}
		void SetMaxValue(int a_lMax);

		int GetButtonValueAdd(){return m_lButtonValueAdd;}
		void SetButtonValueAdd(int a_lAdd);

		int GetBarValueSize(){return m_lBarValueSize;}
		void SetBarValueSize(int a_lSize);

		float GetButtonSize() const {return m_fButtonSize;}

	protected:
		// Own functions
		void UpdateBarProperties();

		bool ArrowButtonDown(iWidget *a_pWidget, cGuiMessageData &a_pData);
		kGuiCallbackDeclarationEnd(ArrowButtonDown);

		// Implemented functions
		void OnInit();
		void OnLoadGraphics();
		void OnChangeSize();
		void OnChangePosition();

		void OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion);

		bool OnMouseMove(cGuiMessageData &a_Data);
		bool OnMouseDown(cGuiMessageData &a_Data);
		bool OnMouseUp(cGuiMessageData &a_Data);
		bool OnMouseEnter(cGuiMessageData &a_Data);
		bool OnMouseLeave(cGuiMessageData &a_Data);

		bool OnLostFocus(cGuiMessageData &a_Data);

		//Data
		eWidgetSliderOrientation m_Orientation;

		bool m_bPressed;

		float m_fButtonSize;

		int m_lValue;
		int m_lMaxValue;
		int m_lButtonValueAdd;
		int m_lBarValueSize;
		float m_fValueStep;

		cVector3f m_vBarPos;
		cVector2f m_vBarSize;
		float m_fSliderSize;
		cRectf m_BarRect;
		float m_fMaxPos;
		float m_fMinPos;

		cVector2f m_vRelMousePos;

		cWidgetButton *m_vButtons[2];

		cGuiGfxElement *m_pGfxButtonBackground;
		cGuiGfxElement *m_vGfxBorders[4];
		cGuiGfxElement *m_vGfxCorners[4];

		cGuiGfxElement *m_vGfxArrow[2];
		cGuiGfxElement *m_pGfxBackground;
	};
};

#endif