#ifndef EFE_WIDGET_WINDOW_H
#define EFE_WIDGET_WINDOW_H

#include <list>
#include <vector>

#include "gui/Widget.h"

namespace efe
{
	class cGuiSkinFont;

	class cWidgetWindow : public iWidget
	{
	public:
		cWidgetWindow(cGuiSet *a_pSet, cGuiSkin *a_pSkin);
		virtual ~cWidgetWindow();

		void SetStatic(bool a_bX);
		bool GetStatic(){return m_bStatic;}

	protected:
		void OnLoadGraphics();

		void OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion);

		bool OnMouseMove(cGuiMessageData &a_Data);
		bool OnMouseDown(cGuiMessageData &a_Data);
		bool OnMouseUp(cGuiMessageData &a_Data);
		bool OnMouseEnter(cGuiMessageData &a_Data);
		bool OnMouseLeave(cGuiMessageData &a_Data);

		cGuiSkinFont *m_pLabelFont;

		cGuiGfxElement *m_pGfxBackground;
		cGuiGfxElement *m_pGfxLabel;

		cGuiGfxElement *m_vGfxBorders[4];
		cGuiGfxElement *m_vGfxCorners[4];

		cVector3f m_vLabelTextOffset;

		bool m_bStatic;

		cVector3f m_vRelMousePos;
		bool m_bMoving;
	};
};
#endif