#include "gui/WidgetButton.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "system/String.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

#include "graphics/FontData.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cWidgetButton::cWidgetButton(cGuiSet *a_pSet, cGuiSkin *a_pSkin) : iWidget(eWidgetType_Button, a_pSet, a_pSkin)
	{
		m_bPressed = false;
		m_pImage = NULL;
	}

	//--------------------------------------------------------------

	cWidgetButton::~cWidgetButton()
	{
		if (m_pImage && m_bDestroyImage)
			m_pGui->DestroyGfx(m_pImage);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetButton::SetImage(cGuiGfxElement *a_pImage, bool a_bDestroyImage)
	{
		m_pImage = a_pImage;
		m_bDestroyImage = a_bDestroyImage;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetButton::OnLoadGraphics()
	{
		m_pGfxBackgroundUp = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpBackground);

		m_vGfxBordersUp[0] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderRight);
		m_vGfxBordersUp[1] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderLeft);
		m_vGfxBordersUp[2] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderUp);
		m_vGfxBordersUp[3] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderDown);

		m_vGfxCornersUp[0] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerLU);
		m_vGfxCornersUp[1] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerRU);
		m_vGfxCornersUp[2] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerRD);
		m_vGfxCornersUp[3] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerLD);

		m_pGfxBackgroundDown = m_pSkin->GetGfx(eGuiSkinGfx_ButtonDownBackground);

		m_vGfxBordersDown[0] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonDownBorderRight);
		m_vGfxBordersDown[1] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonDownBorderLeft);
		m_vGfxBordersDown[2] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonDownBorderUp);
		m_vGfxBordersDown[3] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonDownBorderDown);

		m_vGfxCornersDown[0] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonDownCornerLU);
		m_vGfxCornersDown[1] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonDownCornerRU);
		m_vGfxCornersDown[2] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonDownCornerRD);
		m_vGfxCornersDown[3] = m_pSkin->GetGfx(eGuiSkinGfx_ButtonDownCornerLD);
	}

	//--------------------------------------------------------------

	void cWidgetButton::OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion)
	{
		cVector3f vOffset = 0;
		if (m_bPressed) vOffset = m_pSkin->GetAttribute(eGuiSkinAttribute_ButtonPressedContentOffset);

		if (m_pImage)
			m_pSet->DrawGfx(m_pImage, GetGlobalPosition() + 
			cVector3f(	m_vSize.x/2 - m_pImage->GetImageSize().x/2,
						m_vSize.y/2 - m_pImage->GetImageSize().y/2,0.2f) + vOffset);

		if (IsEnabled())
		{
			cVector3f vGlobPos = GetGlobalPosition();
			cVector3f vPos = cVector3f(	m_vSize.x/2, m_vSize.y/2 - m_vDefaultFontSize.y/2,0.5f) + vOffset;
			DrawDefaultText(m_sText, GetGlobalPosition() +
							cVector3f(	m_vSize.x/2, m_vSize.y/2 - m_vDefaultFontSize.y/2,0.5f) + vOffset,
							eFontAlign_Center);
		}
		else
			DrawSkinText(m_sText, eGuiSkinFont_Disabled, GetGlobalPosition()+
							cVector3f(m_vSize.x/2, m_vSize.y/2 - m_vDefaultFontSize.y/2,0.5f)+vOffset,
							eFontAlign_Center);

		if (m_bPressed)
			DrawBordersAndCorners(m_pGfxBackgroundDown, m_vGfxBordersDown, m_vGfxCornersDown,
									GetGlobalPosition(), m_vSize);
		else
			DrawBordersAndCorners(m_pGfxBackgroundUp, m_vGfxBordersUp, m_vGfxCornersUp,
									GetGlobalPosition(), m_vSize);
	}

	//--------------------------------------------------------------

	bool cWidgetButton::OnMouseMove(cGuiMessageData &a_Data)
	{
		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetButton::OnMouseDown(cGuiMessageData &a_Data)
	{
		m_bPressed = true;
		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetButton::OnMouseUp(cGuiMessageData &a_Data)
	{
		if (GetMouseIsOver()==false) return false;

		if (m_bPressed) ProcessMessage(eGuiMessage_ButtonPressed, a_Data);

		m_bPressed = false;

		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetButton::OnMouseEnter(cGuiMessageData &a_Data)
	{
		return false;
	}

	//--------------------------------------------------------------

	bool cWidgetButton::OnMouseLeave(cGuiMessageData &a_Data)
	{
		m_bPressed = false;

		return true;
	}
}