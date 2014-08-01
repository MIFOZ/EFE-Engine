#ifndef EFE_WIDGET_FRAME_H
#define EFE_WIDGET_FRAME_H

#include "gui/Widget.h"

namespace efe
{
	class cGuiSkinFont;

	class cWidgetFrame : public iWidget
	{
	public:
		cWidgetFrame(cGuiSet *a_pSet, cGuiSkin *a_pSkin);
		virtual ~cWidgetFrame();

		void SetDrawFrame(bool a_bX){m_bDrawFrame = a_bX;}
		bool GetDrawFrame(){return m_bDrawFrame;}

		void SetDrawBackground(bool a_bX){m_bDrawBackground = a_bX;}
		bool GetDrawBackground(){return m_bDrawBackground;}

		void SetBackgroundZ(float a_fZ){m_fBackgroundZ = a_fZ;}
		float GetBackgroundZ(){return m_fBackgroundZ;}

		void SetBackgroundColor(const cColor &a_Color){m_BackgroundColor = a_Color;}
		const cColor &GetBackgroundColor(){return m_BackgroundColor;}
	protected:
		// Implemented functions
		void OnLoadGraphics();

		void OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion);
		void OnDrawAfterClip(float a_fTimeStep, cGuiClipRegion *a_pClipRegion);

		bool OnMouseMove(cGuiMessageData &a_Data);
		bool OnMouseDown(cGuiMessageData &a_Data);
		bool OnMouseUp(cGuiMessageData &a_Data);
		bool OnMouseEnter(cGuiMessageData &a_Data);
		bool OnMouseLeave(cGuiMessageData &a_Data);

		// Data
		bool m_bDrawFrame;
		bool m_bDrawBackground;
		float m_fBackgroundZ;
		cColor m_BackgroundColor;

		cGuiGfxElement *m_pGfxBackground;

		cGuiGfxElement *m_vGfxBorders[4];
		cGuiGfxElement *m_vGfxCorners[4];
	};
};

#endif