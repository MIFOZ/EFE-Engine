#include "gui/WidgetWindow.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

#include "resources/Resources.h"
#include "resources/FontManager.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cWidgetWindow::cWidgetWindow(cGuiSet *a_pSet, cGuiSkin *a_pSkin) : iWidget(eWidgetType_Window, a_pSet, a_pSkin)
	{
		m_vRelMousePos = 0;
		m_bMoving = false;
		m_bStatic = false;
	}

	//--------------------------------------------------------------

	cWidgetWindow::~cWidgetWindow()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetWindow::SetStatic(bool a_bX)
	{
		m_bStatic = a_bX;

		if (m_bStatic==false)
			m_bMoving = false;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWidgetWindow::OnLoadGraphics()
	{
		m_pGfxBackground = m_pSkin->GetGfx(eGuiSkinGfx_WindowBackground);

		m_pGfxLabel = m_pSkin->GetGfx(eGuiSkinGfx_WindowLabel);

		m_vGfxBorders[0] = m_pSkin->GetGfx(eGuiSkinGfx_WindowBorderRight);
		m_vGfxBorders[1] = m_pSkin->GetGfx(eGuiSkinGfx_WindowBorderLeft);
		m_vGfxBorders[2] = m_pSkin->GetGfx(eGuiSkinGfx_WindowBorderUp);
		m_vGfxBorders[3] = m_pSkin->GetGfx(eGuiSkinGfx_WindowBorderDown);

		m_vGfxCorners[0] = m_pSkin->GetGfx(eGuiSkinGfx_WindowCornerLU);
		m_vGfxCorners[1] = m_pSkin->GetGfx(eGuiSkinGfx_WindowCornerRU);
		m_vGfxCorners[2] = m_pSkin->GetGfx(eGuiSkinGfx_WindowCornerRD);
		m_vGfxCorners[3] = m_pSkin->GetGfx(eGuiSkinGfx_WindowCornerLD);

		m_pLabelFont = m_pSkin->GetFont(eGuiSkinFont_WindowLabel);

		m_vLabelTextOffset = m_pSkin->GetAttribute(eGuiSkinAttribute_WindowLabelTextOffset);
	}

	//--------------------------------------------------------------

	void cWidgetWindow::OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion)
	{
		cVector2f vLabelSize;
		vLabelSize.x = m_vSize.x - (m_vGfxCorners[0]->GetActiveSize().x + m_vGfxCorners[1]->GetActiveSize().x);
		vLabelSize.y = m_pLabelFont->m_vSize.y + m_vLabelTextOffset.y*2;

		m_pSet->DrawGfx(m_pGfxLabel, GetGlobalPosition() +
			cVector3f(m_vGfxCorners[0]->GetActiveSize().x, m_vGfxCorners[0]->GetActiveSize().y, 0.2f),
			vLabelSize);

		DrawSkinText(m_sText, eGuiSkinFont_WindowLabel, GetGlobalPosition() +
			cVector3f(	m_vGfxCorners[0]->GetActiveSize().x+m_vLabelTextOffset.x,
						m_vGfxCorners[0]->GetActiveSize().y+m_vLabelTextOffset.y,0.4f));

		DrawBordersAndCorners(m_pGfxBackground, m_vGfxBorders, m_vGfxCorners, GetGlobalPosition(), m_vSize);
	}

	//--------------------------------------------------------------

	bool cWidgetWindow::OnMouseMove(cGuiMessageData &a_Data)
	{
		if (m_bMoving) SetGlobalPosition(m_vRelMousePos + cVector3f(a_Data.m_vPos.x, a_Data.m_vPos.y, 0));
		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetWindow::OnMouseDown(cGuiMessageData &a_Data)
	{
		if (m_bStatic) return false;

		cRectf labelRect;
		labelRect.w = m_vSize.x - (m_vGfxCorners[0]->GetActiveSize().x + m_vGfxCorners[1]->GetActiveSize().x);
		labelRect.h = m_pLabelFont->m_vSize.y + m_vLabelTextOffset.y*2;
		labelRect.x = GetGlobalPosition().x + m_vGfxCorners[0]->GetActiveSize().x;
		labelRect.y = GetGlobalPosition().y + m_vGfxCorners[0]->GetActiveSize().y;

		if (cMath::PointBoxCollision(a_Data.m_vPos, labelRect) && a_Data.m_lVal  & eGuiMouseButton_Left)
		{
			m_bMoving = true;
			m_vRelMousePos = GetPosRelativeToMouse(a_Data);
			m_vRelMousePos.z = GetGlobalPosition().z;
		}

		return true;
	}
	
	//--------------------------------------------------------------

	bool cWidgetWindow::OnMouseUp(cGuiMessageData &a_Data)
	{
		if (a_Data.m_lVal & eGuiMouseButton_Left) m_bMoving = false;

		return true;
	}

	//--------------------------------------------------------------

	bool cWidgetWindow::OnMouseEnter(cGuiMessageData &a_Data)
	{
		return false;
	}

	//--------------------------------------------------------------

	bool cWidgetWindow::OnMouseLeave(cGuiMessageData &a_Data)
	{
		cVector3f vLastGlobal = GetGlobalPosition();

		if (m_bMoving) SetGlobalPosition(m_vRelMousePos + cVector3f(a_Data.m_vPos.x,a_Data.m_vPos.y,0));

		if (PointIsInside(a_Data.m_vPos, false)==false)
		{
			SetGlobalPosition(vLastGlobal);
			m_bMoving = false;
		}

		return false;
	}

	//--------------------------------------------------------------

}