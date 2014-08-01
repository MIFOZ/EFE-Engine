#ifndef EFE_WIDGET_H
#define EFE_WIDGET_H

#include <list>
#include <vector>
#include "gui/GuiTypes.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	class cGui;
	class cGuiSet;
	class cGuiSkin;
	class cGuiSkinFont;

	class cGuiGfxElement;
	class cGuiClipRegion;

	class iFontData;

	struct cWidgetCallback
	{
		cWidgetCallback(void *a_pObject, tGuiCallbackFunc a_pFunc)
		{
			m_pObject = a_pObject;
			m_pFunc = a_pFunc;
		}

		void *m_pObject;
		tGuiCallbackFunc m_pFunc;
	};

	typedef std::list<cWidgetCallback> tWidgetCallbackList;
	typedef tWidgetCallbackList::iterator tWidgetCallbackListIt;

	class iWidget
	{
		friend class cGuiSet;
	public:
		iWidget(eWidgetType a_Type, cGuiSet *a_pSet, cGuiSkin *a_pSkin);
		virtual ~iWidget();

		//General
		void Update(float a_fTimeStep);

		void Draw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion);

		bool ProcessMessage(eGuiMessage a_Message, cGuiMessageData &a_Data);

		void AddCallback(eGuiMessage a_Message, void *a_pObject, tGuiCallbackFunc a_pFunc);

		eWidgetType GetType(){return m_Type;}

		void Init();

		bool PointIsInside(const cVector2f &a_vPoint, bool a_bOnlyClipped);

		//Hierarchy
		void AttachChild(iWidget *a_pChild);
		void RemoveChild(iWidget *a_pChild);

		//Properties

		void SetEnabled(bool a_bX);
		bool IsEnabled();
		void SetVisible(bool a_bX);
		bool IsVisible();

		void SetName(const tString &a_sName){m_sName = a_sName;}
		const tString &GetName(){return m_sName;}

		void SetText(const tWString &a_sText);
		const tWString &GetText(){return m_sText;}

		void SetPosition(const cVector3f &a_vPos);
		void SetGlobalPosition(const cVector3f &a_vPos);
		const cVector3f &GetLocalPosition();
		const cVector3f &GetGlobalPosition();

		void SetSize(const cVector2f &a_vSize);
		cVector2f GetSize(){return m_vSize;}

		bool ClipsGraphics();

		bool GetMouseIsOver(){return m_bMouseIsOver;}

		bool IsConnectedTo(iWidget *a_pWidget, bool a_bIsStartWidget=true);

		cGuiGfxElement *GetPointerGfx();

	protected:
		virtual void OnLoadGraphics(){}

		virtual void OnChangeSize(){}
		virtual void OnChangePosition(){}
		virtual void OnChangeText(){}

		virtual void OnInit(){}

		virtual void OnDraw(float a_fTimeStep, cGuiClipRegion *a_pClipRegion){}
		virtual void OnDrawAfterClip(float a_fTimeStep, cGuiClipRegion *a_pClipRegion){}

		virtual void OnUpdate(float a_fTimeStep){}

		virtual bool OnMessage(eGuiMessage a_Message, cGuiMessageData &a_Data){return false;}
		virtual bool OnMouseMove(cGuiMessageData &a_Data){return false;}
		virtual bool OnMouseDown(cGuiMessageData &a_Data){return false;}
		virtual bool OnMouseUp(cGuiMessageData &a_Data){return false;}
		virtual bool OnMouseDoubleClick(cGuiMessageData &a_Data){return false;}
		virtual bool OnMouseEnter(cGuiMessageData &a_Data){return false;}
		virtual bool OnMouseLeave(cGuiMessageData &a_Data){return false;}

		virtual bool OnGotFocus(cGuiMessageData &a_Data);
		virtual bool OnLostFocus(cGuiMessageData &a_Data){return false;}

		virtual bool OnKeyPress(cGuiMessageData &a_Data){return false;}

		// Private helper functions
		cVector3f WorldToLocalPosition(const cVector3f &a_vPos);
		cVector2f GetPosRelativeToMouse(cGuiMessageData &a_Data);

		void DrawBordersAndCorners(cGuiGfxElement *a_pBackgorund,
									cGuiGfxElement **a_pBorderVec, cGuiGfxElement **a_pCornerVec,
									const cVector3f &a_vPosition, const cVector2f &a_vSize);


		void DrawSkinText(const tWString &a_sText, eGuiSkinFont a_Font, const cVector3f &a_vPosition,
							eFontAlign a_Align = eFontAlign_Left);

		void DrawDefaultText(	const tWString &a_sText,
								const cVector3f &a_vPosition, eFontAlign a_Align);

		void SetPositionUpdated();

		void LoadGraphics();

		cGuiSet *m_pSet;
		cGuiSkin *m_pSkin;
		cGui *m_pGui;

		tWString m_sText;
		cVector3f m_vPosition;
		cVector3f m_vGlobalPosition;
		cVector2f m_vSize;

		tString m_sName;

		eWidgetType m_Type;

		int m_lPositionCount;

		cGuiSkinFont *m_pDefaultFont;
		iFontData *m_pDefaultFontType;
		cColor m_DefaultFontColor;
		cVector2f m_vDefaultFontSize;

		iWidget *m_pParent;

		tWidgetList m_lstChildren;

		bool m_bEnabled;
		bool m_bVisible;

		bool m_bMouseIsOver;

		bool m_bClipsGraphics;

		cGuiGfxElement *m_pPointerGfx;

		bool m_bConnectedToChildren;

	private:
		void SetMouseIsOver(bool a_bX){m_bMouseIsOver = a_bX;}
		bool ProcessCallbacks(eGuiMessage a_Message, cGuiMessageData &a_Data);

		std::vector<tWidgetCallbackList> m_vCallbackLists;

		bool m_bPositionIsUpdated;
	};
};
#endif