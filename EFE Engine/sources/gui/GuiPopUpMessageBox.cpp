#include "gui/GuiPopUpMessageBox.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "graphics/FontData.h"

#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"

#include "gui/WidgetButton.h"
#include "gui/WidgetLabel.h"
#include "gui/WidgetWindow.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGuiPopUpMessageBox::cGuiPopUpMessageBox(cGuiSet *a_pSet,
			const tWString &a_sLabel, const tWString &a_sText,
			const tWString &a_sButton1, const tWString &a_sButton2,
			void *a_pCallbackObject, tGuiCallbackFunc a_pCallback)
			: iGuiPopUp(a_pSet)
	{
		m_pCallback = a_pCallback;
		m_pCallbackObject = a_pCallbackObject;

		cGuiSkinFont *pFont = m_pSkin->GetFont(eGuiSkinFont_Default);

		float fWindowMinLength = pFont->m_pFont->GetLength(pFont->m_vSize, a_sLabel.c_str());
		float fTextLength = pFont->m_pFont->GetLength(pFont->m_vSize, a_sText.c_str());

		if (fTextLength > fWindowMinLength) fWindowMinLength = fTextLength;

		float fWindowWidth = fWindowMinLength+40 > 200 ? fWindowMinLength+40 : 200;

		cVector2f vVirtSize = m_pSet->GetVirtualSize();

		float fWindowHeight = 90 + pFont->m_vSize.y;

		cVector3f vPos = cVector3f(vVirtSize.x/2 - fWindowWidth/2, vVirtSize.y/2 - fWindowHeight/2, 18);
		m_pWindow = m_pSet->CreateWidgetWindow(vPos, cVector2f(fWindowWidth, fWindowHeight), a_sLabel, NULL);

		if (a_sButton2 == _W(""))
		{
			vPos = cVector3f(fWindowWidth/2 - 40, 50 + pFont->m_vSize.y, 1);
			m_vButtons[0] = m_pSet->CreateWidgetButton(vPos, cVector2f(80, 30), a_sButton1, m_pWindow);
			m_vButtons[0]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(ButtonPress));

			m_vButtons[1] = NULL;
		}
		else
		{
			vPos = cVector3f(fWindowWidth/2 - (80*2+20)/2, 50 + pFont->m_vSize.y, 1);
			m_vButtons[0] = m_pSet->CreateWidgetButton(vPos, cVector2f(80, 30), a_sButton1, m_pWindow);
			m_vButtons[0]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(ButtonPress));

			vPos.x += 80+20;

			m_vButtons[1] = m_pSet->CreateWidgetButton(vPos, cVector2f(80, 30), a_sButton2, m_pWindow);
			m_vButtons[1]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(ButtonPress));
		}

		// Label
		vPos = cVector3f(20, 30, 1);
		m_pLabel = m_pSet->CreateWidgetLabel(	vPos, cVector2f(fWindowWidth-10, pFont->m_vSize.y),
												a_sText, m_pWindow);

		// Attention
		m_pPrevAttention = m_pSet->GetAttentionWidget();
		m_pSet->SetAttentionWidget(m_pWindow);
	}

	//--------------------------------------------------------------

	cGuiPopUpMessageBox::~cGuiPopUpMessageBox()
	{
		if (m_pWindow) m_pSet->DestroyWidget(m_pWindow);
		if (m_vButtons[0]) m_pSet->DestroyWidget(m_vButtons[0]);
		if (m_vButtons[1]) m_pSet->DestroyWidget(m_vButtons[1]);
		if (m_pLabel) m_pSet->DestroyWidget(m_pLabel);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cGuiPopUpMessageBox::ButtonPress(iWidget *a_pWidget, cGuiMessageData &a_Data)
	{
		int lButton = a_pWidget == m_vButtons[0] ? 0 : 1;

		m_pSet->SetAttentionWidget(m_pPrevAttention);

		if (m_pCallback && m_pCallbackObject)
		{
			cGuiMessageData data = cGuiMessageData(lButton);
			m_pCallback(m_pCallbackObject, a_pWidget, data);
		}

		SelfDestruct();

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpMessageBox, ButtonPress)
}