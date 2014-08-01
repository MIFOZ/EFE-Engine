#include "gui/Widget.h"

#include "system/LowLevelSystem.h"

#include "gui/GuiSet.h"
#include "gui/GuiSkin.h"
#include "gui/GuiGfxElement.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iWidget::iWidget(eWidgetType a_Type, cGuiSet *a_pSet, cGuiSkin *a_pSkin)
	{
		m_pSet = a_pSet;
		m_pSkin = a_pSkin;
		m_pGui = a_pSet->GetGui();

		m_Type = a_Type;

		m_vCallbackLists.resize(eGuiMessage_LastEnum);

		m_pParent = NULL;

		m_vPosition = 0;
		m_vSize = 0;

		m_bClipsGraphics = false;

		m_bMouseIsOver = false;

		m_sText = _W("");

		m_bPositionIsUpdated = true;

		m_lPositionCount = 0;

		m_bConnectedToChildren = true;

		if (m_pSkin) m_pPointerGfx = m_pSkin->GetGfx(eGuiSkinGfx_PointerNormal);
		else m_pPointerGfx = NULL;
	}

	iWidget::~iWidget()
	{
		tWidgetListIt it = m_lstChildren.begin();
		while(it != m_lstChildren.end())
		{
			RemoveChild(*it);
			it = m_lstChildren.begin();
		}

		if (m_pParent) m_pParent->RemoveChild(this);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void iWidget::Update(float a_fTimeStep)
	{
		OnUpdate(a_fTimeStep);
	}

	//--------------------------------------------------------------

	void iWidget::Draw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion)
	{
		if (m_bVisible==false) return;

		OnDraw(a_fTimeStep, a_pClipRegion);

		cGuiClipRegion *pChildRegion = a_pClipRegion;
		if (m_bClipsGraphics)
		{
			pChildRegion = pChildRegion->CreateChild(GetGlobalPosition(), m_vSize);
			m_pSet->SetCurrentClipRegion(pChildRegion);
		}

		OnDrawAfterClip(a_fTimeStep, a_pClipRegion);

		cGuiMessageData data;
		data.m_fVal = a_fTimeStep;
		data.m_pData = a_pClipRegion;
		ProcessMessage(eGuiMessage_OnDraw, data);

		tWidgetListIt it = m_lstChildren.begin();
		for (; it != m_lstChildren.end(); ++it)
		{
			iWidget *pChild = *it;

			pChild->Draw(a_fTimeStep, a_pClipRegion);
		}

		if (m_bClipsGraphics) m_pSet->SetCurrentClipRegion(a_pClipRegion);
	}

	//--------------------------------------------------------------

	bool iWidget::ProcessMessage(eGuiMessage a_Message, cGuiMessageData &a_Data)
	{
		if (IsEnabled()==false) return false;

		a_Data.m_Message = a_Message;

		bool bRet = false;
		bRet = OnMessage(a_Message, a_Data);

		if (bRet==false)
		{
			switch(a_Message)
			{
			case eGuiMessage_MouseMove:			bRet = OnMouseMove(a_Data);break;
			case eGuiMessage_MouseDown:			bRet = OnMouseDown(a_Data);break;
			case eGuiMessage_MouseUp:			bRet = OnMouseUp(a_Data);break;
			case eGuiMessage_MouseDoubleClick:	bRet = OnMouseDoubleClick(a_Data);break;
			case eGuiMessage_MouseEnter:		bRet = OnMouseEnter(a_Data);break;
			case eGuiMessage_MouseLeave:		bRet = OnMouseLeave(a_Data);break;
			case eGuiMessage_GotFocus:			bRet = OnGotFocus(a_Data);break;
			case eGuiMessage_LostFocus:			bRet = OnLostFocus(a_Data);break;
			case eGuiMessage_KeyPress:			bRet = OnKeyPress(a_Data);break;
			}
		}

		if (ProcessCallbacks(a_Message, a_Data))
			bRet = true;

		return bRet;
	}

	//--------------------------------------------------------------

	void iWidget::AddCallback(eGuiMessage a_Message, void *a_pObject, tGuiCallbackFunc a_pFunc)
	{
		m_vCallbackLists[a_Message].push_back(cWidgetCallback(a_pObject, a_pFunc));
	}

	//--------------------------------------------------------------

	void iWidget::Init()
	{
		OnInit();
		LoadGraphics();
	}

	//--------------------------------------------------------------

	bool iWidget::PointIsInside(const cVector2f &a_vPoint, bool a_bOnlyClipped)
	{
		if (m_pParent && m_pParent->ClipsGraphics())
		{
			if (m_pParent->PointIsInside(a_vPoint, true)==false)
				return false;
		}

		if (a_bOnlyClipped && m_bClipsGraphics==false) return true;

		cVector3f vGlobalPos = GetGlobalPosition();

		if (a_vPoint.x < vGlobalPos.x || a_vPoint.x > vGlobalPos.x + m_vSize.x ||
			a_vPoint.y < vGlobalPos.y || a_vPoint.y > vGlobalPos.y + m_vSize.y)
			return false;
		else
			return true;
	}

	//--------------------------------------------------------------

	void iWidget::AttachChild(iWidget *a_pChild)
	{
		if (a_pChild->m_pParent)
		{
			iWidget *pParent = a_pChild->m_pParent;
			pParent->RemoveChild(a_pChild);
			a_pChild->SetPosition(a_pChild->m_vPosition + pParent->GetGlobalPosition());
			a_pChild->SetPosition(a_pChild->m_vPosition - GetGlobalPosition());
		}
		a_pChild->m_pParent = this;
		a_pChild->SetPositionUpdated();
		m_lstChildren.push_back(a_pChild);
	}

	void iWidget::RemoveChild(iWidget *a_pChild)
	{
		tWidgetListIt it = m_lstChildren.begin();
		for (; it != m_lstChildren.end(); ++it)
		{
			iWidget *pChild = *it;

			if (pChild == a_pChild)
			{
				m_lstChildren.erase(it);

				pChild->m_pParent = NULL;
				pChild->SetPositionUpdated();
				pChild->SetPosition(pChild->m_vPosition + GetGlobalPosition());

				break;
			}
		}
	}

	//--------------------------------------------------------------

	void iWidget::SetEnabled(bool a_bX)
	{
		if (m_bEnabled == a_bX) return;

		m_bEnabled = a_bX;
	}

	//--------------------------------------------------------------

	bool iWidget::IsEnabled()
	{
		if (m_pParent)
		{
			if (m_pParent->IsEnabled()) return m_bEnabled;
			else						return false;
		}

		return m_bEnabled;
	}

	//--------------------------------------------------------------

	void iWidget::SetVisible(bool a_bX)
	{
		if (m_bVisible == a_bX) return;

		m_bVisible = a_bX;
	}

	//--------------------------------------------------------------

	bool iWidget::IsVisible()
	{
		if (m_pParent)
		{
			if (m_pParent->IsVisible()) return m_bVisible;
			else						return false;
		}

		return m_bVisible;
	}

	//--------------------------------------------------------------

	void iWidget::SetText(const tWString &a_sText)
	{
		if (a_sText == m_sText) return;

		m_sText = a_sText;

		OnChangeText();
		cGuiMessageData data = cGuiMessageData();
		ProcessMessage(eGuiMessage_TextChange, data);
	}

	//--------------------------------------------------------------

	void iWidget::SetPosition(const cVector3f &a_vPos)
	{
		m_vPosition = a_vPos;

		SetPositionUpdated();
	}

	//--------------------------------------------------------------

	void iWidget::SetGlobalPosition(const cVector3f &a_vPos)
	{
		SetPosition(a_vPos - m_pParent->GetGlobalPosition());
	}

	//--------------------------------------------------------------

	const cVector3f &iWidget::GetLocalPosition()
	{
		return m_vPosition;
	}

	//--------------------------------------------------------------

	const cVector3f &iWidget::GetGlobalPosition()
	{
		if (m_pParent)
		{
			if (m_bPositionIsUpdated)
			{
				m_bPositionIsUpdated = false;
				m_vGlobalPosition = m_pParent->GetGlobalPosition() + m_vPosition;
			}
			return m_vGlobalPosition;
		}
		else
			return m_vPosition;
	}

	//--------------------------------------------------------------

	void iWidget::SetSize(const cVector2f &a_vSize)
	{
		m_vSize = a_vSize;

		OnChangeSize();
	}

	//--------------------------------------------------------------

	bool iWidget::ClipsGraphics()
	{
		if (m_pParent && m_pParent->ClipsGraphics()) return true;

		return m_bClipsGraphics;
	}

	bool iWidget::IsConnectedTo(iWidget *a_pWidget, bool a_bIsStartWidget)
	{
		if (a_bIsStartWidget == false && m_bConnectedToChildren==false) return false;

		if (a_pWidget==NULL) return false;
		if (a_pWidget==this) return true;

		if (m_pParent) return m_pParent->IsConnectedTo(a_pWidget,false);

		return false;
	}

	cGuiGfxElement *iWidget::GetPointerGfx()
	{
		return m_pPointerGfx;
	}

	//--------------------------------------------------------------

	bool iWidget::OnGotFocus(cGuiMessageData &a_Data)
	{
		return m_bEnabled;
	}

	//--------------------------------------------------------------

	void iWidget::SetPositionUpdated()
	{
		m_bPositionIsUpdated = true;
		m_lPositionCount++;

		OnChangePosition();

		tWidgetListIt it = m_lstChildren.begin();
		for (; it != m_lstChildren.end(); ++it)
		{
			iWidget *pChild = *it;
			pChild->SetPositionUpdated();
		}
	}

	//--------------------------------------------------------------

	void iWidget::LoadGraphics()
	{
		if (m_pSkin)
		{
			m_pDefaultFont = m_pSkin->GetFont(eGuiSkinFont_Default);

			m_pDefaultFontType = m_pDefaultFont->m_pFont;
			m_DefaultFontColor = m_pDefaultFont->m_Color;
			m_vDefaultFontSize = m_pDefaultFont->m_vSize;
		}
		else
			m_pDefaultFont = NULL;

		OnLoadGraphics();
	}

	//--------------------------------------------------------------

	cVector3f iWidget::WorldToLocalPosition(const cVector3f &a_vPos)
	{
		return a_vPos - GetGlobalPosition();
	}

	//--------------------------------------------------------------

	cVector2f iWidget::GetPosRelativeToMouse(cGuiMessageData &a_Data)
	{
		cVector3f vTemp = GetGlobalPosition() - a_Data.m_vPos;
		return cVector2f(vTemp.x, vTemp.y);
	}

	//--------------------------------------------------------------

	void iWidget::DrawBordersAndCorners(cGuiGfxElement *a_pBackgorund,
									cGuiGfxElement **a_pBorderVec, cGuiGfxElement **a_pCornerVec,
									const cVector3f &a_vPosition, const cVector2f &a_vSize)
	{
		m_pSet->SetDrawOffset(a_vPosition);

		if(a_pBackgorund)
			m_pSet->DrawGfx(a_pBackgorund, cVector3f(a_pCornerVec[0]->GetActiveSize().x,
													a_pCornerVec[0]->GetActiveSize().y,0),
									a_vSize - a_pCornerVec[2]->GetActiveSize()-a_pCornerVec[0]->GetActiveSize(),
									cColor(1,1));

		// Borders
		//Right
		m_pSet->DrawGfx(a_pCornerVec[0],
			cVector3f(	a_vSize.x - a_pBorderVec[0]->GetActiveSize().x,
			a_pCornerVec[1]->GetActiveSize().y,0),
			cVector2f(	a_pBorderVec[0]->GetImageSize().x,
			a_vSize.y - (a_pCornerVec[2]->GetActiveSize().y +
			a_pCornerVec[1]->GetActiveSize().y)));
		//Left
		m_pSet->DrawGfx(a_pCornerVec[1],
			cVector3f(	0,a_pCornerVec[0]->GetActiveSize().y,0),
			cVector2f(	a_pBorderVec[1]->GetImageSize().x,
			a_vSize.y - (a_pCornerVec[3]->GetActiveSize().y +
			a_pCornerVec[0]->GetActiveSize().y)));

		//Up
		m_pSet->DrawGfx(a_pBorderVec[2],
			cVector3f(	a_pCornerVec[0]->GetActiveSize().x,0,0),
			cVector2f(	a_vSize.x - (a_pCornerVec[0]->GetActiveSize().x+
			a_pCornerVec[1]->GetActiveSize().x),
			a_pBorderVec[2]->GetImageSize().y));

		//Down
		m_pSet->DrawGfx(a_pCornerVec[3],
			cVector3f(	a_pCornerVec[3]->GetActiveSize().x,
			a_vSize.y - a_pBorderVec[3]->GetActiveSize().y,0),
			cVector2f(	a_vSize.x - (a_pCornerVec[2]->GetActiveSize().x+
			a_pCornerVec[3]->GetActiveSize().x),
			a_pBorderVec[3]->GetImageSize().y));

		// Corners
		//Left Up
		m_pSet->DrawGfx(a_pCornerVec[0], cVector3f(0,0,0));
		//Right Up
		m_pSet->DrawGfx(a_pCornerVec[1], cVector3f(	a_vSize.x - a_pCornerVec[1]->GetActiveSize().x,0,0));

		//Right Down
		m_pSet->DrawGfx(a_pCornerVec[2], cVector3f(	a_vSize.x - a_pCornerVec[2]->GetActiveSize().x,
			a_vSize.y - a_pCornerVec[2]->GetActiveSize().y,0));
		//Left Down
		m_pSet->DrawGfx(a_pCornerVec[3], cVector3f(0,a_vSize.y - a_pCornerVec[3]->GetActiveSize().y,0));

		m_pSet->SetDrawOffset(0);
	}

	//--------------------------------------------------------------

	void iWidget::DrawSkinText(const tWString &a_sText, eGuiSkinFont a_Font,
								const cVector3f &a_vPosition, eFontAlign a_Align)
	{
		cGuiSkinFont *pFont = m_pSkin->GetFont(a_Font);
		m_pSet->DrawFont(a_sText, pFont->m_pFont, a_vPosition, pFont->m_vSize, pFont->m_Color,
			a_Align);
	}

	void iWidget::DrawDefaultText(	const tWString &a_sText,
									const cVector3f &a_vPosition, eFontAlign a_Align)
	{
		if (m_pDefaultFontType==NULL) return;

		m_pSet->DrawFont(a_sText, m_pDefaultFontType, a_vPosition, m_vDefaultFontSize,
			m_DefaultFontColor, a_Align);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool iWidget::ProcessCallbacks(eGuiMessage a_Message, cGuiMessageData &a_Data)
	{
		tWidgetCallbackList &lstCallbacks = m_vCallbackLists[a_Message];

		if (lstCallbacks.empty()) return false;

		bool bRet = false;
		tWidgetCallbackListIt it = lstCallbacks.begin();
		for (; it != lstCallbacks.end(); ++it)
		{
			cWidgetCallback &callback = *it;

			bool bX = (callback.m_pFunc)(callback.m_pObject, this, a_Data);
			if (bX) bRet = true;
		}

		return bRet;
	}
}