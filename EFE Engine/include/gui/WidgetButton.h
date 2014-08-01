#ifndef EFE_WIDGET_BUTTON_H
#define EFE_WIDGET_BUTTON_H

#include "gui/Widget.h"
namespace efe
{
	class cGuiSkinFont;

	class cWidgetButton : public iWidget
	{
	public:
		cWidgetButton(cGuiSet *a_pSet, cGuiSkin *a_pSkin);
		virtual ~cWidgetButton();

		void SetImage(cGuiGfxElement *a_pImage, bool a_bDestroyImage=true);
		cGuiGfxElement *GetImage(){return m_pImage;}

	protected:
		void OnLoadGraphics();

		void OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion);

		bool OnMouseMove(cGuiMessageData &a_Data);
		bool OnMouseDown(cGuiMessageData &a_Data);
		bool OnMouseUp(cGuiMessageData &a_Data);
		bool OnMouseEnter(cGuiMessageData &a_Data);
		bool OnMouseLeave(cGuiMessageData &a_Data);

		bool m_bPressed;

		cGuiGfxElement *m_pImage;
		bool m_bDestroyImage;

		cGuiGfxElement *m_pGfxBackgroundUp;
		cGuiGfxElement *m_vGfxBordersUp[4];
		cGuiGfxElement *m_vGfxCornersUp[4];

		cGuiGfxElement *m_pGfxBackgroundDown;
		cGuiGfxElement *m_vGfxBordersDown[4];
		cGuiGfxElement *m_vGfxCornersDown[4];
	};
};
#endif