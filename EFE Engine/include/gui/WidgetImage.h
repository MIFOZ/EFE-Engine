#ifndef EFE_WIDGET_IMAGE_H
#define EFE_WIDGET_IMAGE_H

#include "gui/Widget.h"

namespace efe
{
	class cGuiSkinFont;

	class cWidgetImage : public iWidget
	{
	public:
		cWidgetImage(cGuiSet *a_pSet, cGuiSkin *a_pSkin);
		virtual ~cWidgetImage();

		void SetImage(cGuiGfxElement *a_pGfx);
		cGuiGfxElement *GetImage();

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
		cGuiGfxElement *m_pGfxImage;
		cColor m_Color;
	};
};

#endif