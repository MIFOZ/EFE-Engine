#ifndef EFE_GUI_SET_H
#define EFE_GUI_SET_H

#include <list>
#include "gui/GuiTypes.h"
#include "graphics/GraphicsTypes.h"

namespace efe
{
	class cResources;
	class cGraphics;
	class cSound;
	class cScene;

	class iFontData;

	class cGui;
	class cGuiSkin;
	class iGuiMaterial;
	class iGuiPopUp;
	class iWidget;

	class cWidgetWindow;
	class cWidgetFrame;
	class cWidgetButton;
	class cWidgetLabel;
	class cWidgetSlider;
	class cWidgetTextBox;
	class cWidgetCheckBox;
	class cWidgetImage;
	class cWidgetListBox;
	class cWidgetComboBox;

	class iDepthStencilState;

	typedef std::list<iGuiPopUp*> tGuiPopUpList;
	typedef tGuiPopUpList::iterator tGuiPopUpListIt;

	class cGuiClipRegion;
	class cGuiRenderObject
	{
	public:
		cGuiGfxElement *m_pGfx;
		cVector3f m_vPos;
		cVector2f m_vSize;
		cColor m_Color;
		iGuiMaterial *m_pCustomMaterial;
		cGuiClipRegion *m_pClipRegion;
	};

	class cGuiRenderObjectCompare
	{
	public:
		bool operator()(const cGuiRenderObject &a_ObjectA,const cGuiRenderObject &a_ObjectB);
	};

	typedef std::multiset<cGuiRenderObject, cGuiRenderObjectCompare> tGuiRenderObjectSet;
	typedef tGuiRenderObjectSet::iterator tGuiRenderObjectSetIt;

	typedef std::list<cGuiClipRegion*> tGuiClipRegionList;
	typedef tGuiClipRegionList::iterator tGuiClipRegionListIt;

	class cGuiClipRegion
	{
	public:
		cGuiClipRegion() : m_Rect(0,0,-1,-1){}
		~cGuiClipRegion();

		void Clear();
		cGuiClipRegion *CreateChild(const cVector3f &a_vPos, const cVector2f &a_vSize);

		cRectf m_Rect;

		tGuiClipRegionList m_lstChildren;
	};

	class cGuiSet
	{
	public:
		cGuiSet(const tString &a_sName, cGui *a_pGui, cGuiSkin *a_pSkin,
			cResources *a_pResources, cGraphics *a_pGraphics,
			cSound *a_pSound, cScene *a_pScene);
		~cGuiSet();

		//General
		void Update(float a_fTimeStep);

		void DrawAll(float a_fTimeStep);

#undef SendMessage
		bool SendMessage(eGuiMessage a_Message, cGuiMessageData &a_Data);


		// Rendering
		void Render();

		void SetDrawOffset(const cVector3f &a_vOffset){m_vDrawOffset = a_vOffset;}
		void SetCurrentClipRegion(cGuiClipRegion *a_pRegion){m_pCurrentClipRegion = a_pRegion;}

		void DrawGfx(cGuiGfxElement *a_pGfx,
					const cVector3f &a_vPos,
					const cVector2f &a_vSize=-1,
					const cColor &a_Color = cColor(1,1),
					eGuiMaterial a_Material = eGuiMaterial_LastEnum);
		void DrawFont(const tWString &a_sText,
						iFontData *a_pFont, const cVector3f &a_vPos,
						const cVector2f &a_vSize, const cColor &a_Color,
						eFontAlign a_Align = eFontAlign_Left,
						eGuiMaterial a_Material = eGuiMaterial_FontNormal);

		// Widget Creation
		cWidgetWindow *CreateWidgetWindow(	const cVector3f &a_vLocalPos=0,
											const cVector2f &a_vSize=0,
											const tWString &a_sText=_W(""),
											iWidget *a_pParent=NULL,
											const tString &a_sName = "");

		cWidgetFrame *CreateWidgetFrame(	const cVector3f &a_vLocalPos=0,
											const cVector2f &a_vSize=0,
											bool a_bDrawFrame=false,
											iWidget *a_pParent=NULL,
											const tString &a_sName = "");

		cWidgetButton *CreateWidgetButton(	const cVector3f &a_vLocalPos=0,
											const cVector2f &a_vSize=0,
											const tWString &a_sText=_W(""),
											iWidget *a_pParent=NULL,
											const tString &a_sName = "");

		cWidgetLabel *CreateWidgetLabel(	const cVector3f &a_vLocalPos=0,
											const cVector2f &a_vSize=0,
											const tWString &a_sText=_W(""),
											iWidget *a_pParent=NULL,
											const tString &a_sName = "");

		cWidgetSlider *CreateWidgetSlider(	eWidgetSliderOrientation a_Orientation,
											const cVector3f &a_vLocalPos=0,
											const cVector2f &a_vSize=0,
											int a_lMaxValue=10,
											iWidget *a_pParent=NULL,
											const tString &a_sName = "");

		cWidgetTextBox *CreateWidgetTextBox(const cVector3f &a_vLocalPos=0,
											const cVector2f &a_vSize=0,
											const tWString &a_sText=_W(""),
											iWidget *a_pParent=NULL,
											const tString &a_sName = "");

		cWidgetCheckBox *CreateWidgetCheckBox(const cVector3f &a_vLocalPos=0,
											const cVector2f &a_vSize=0,
											const tWString &a_sText=_W(""),
											iWidget *a_pParent=NULL,
											const tString &a_sName = "");

		cWidgetImage *CreateWidgetImage(const tString &a_sFile="",
											const cVector3f &a_vLocalPos=0,
											const cVector2f &a_vSize=-1,
											eGuiMaterial a_Material = eGuiMaterial_Alpha,
											bool a_bAnimate = false,
											iWidget *a_pParent=NULL,
											const tString &a_sName = "");

		cWidgetListBox *CreateWidgetListBox(const cVector3f &a_vLocalPos=0,
											const cVector2f &a_vSize=0,
											iWidget *a_pParent=NULL,
											const tString &a_sName = "");

		cWidgetComboBox *CreateWidgetComboBox(	const cVector3f &a_vLocalPos=0,
												const cVector2f &a_vSize=0,
												const tWString &a_sText=_W(""),
												iWidget *a_pParent=NULL,
												const tString &a_sName = "");

		void DestroyWidget(iWidget *a_pWidget);

		// Popup
		void CreatePopUpMessageBox(	const tWString &a_sLabel, const tWString &a_sText,
									const tWString &a_sButton1, const tWString &a_sButton2,
									void *a_pCallbackObject, tGuiCallbackFunc a_pCallback);

		void DestroyPopUp(iGuiPopUp *a_pPopUp);

		//Properties
		cGui *GetGui(){return m_pGui;}

		void SetVirtualSize(const cVector2f &a_vSize, float a_fMinZ, float a_fMaxZ);
		const cVector2f &GetVirtualSize(){return m_vVirtualSize;}

		void SetFocusedWidget(iWidget *a_pWidget);
		iWidget *GetFocusedWidget(){return m_pFocusedWidget;}

		void SetAttentionWidget(iWidget *a_pWidget);
		iWidget *GetAttentionWidget(){return m_pAttentionWidget;}

		int GetDrawPriority(){return m_lDrawPrio;}
		void SetDrawPriority(int a_lPrio){m_lDrawPrio = a_lPrio;}

		void SetCurrentPointer(cGuiGfxElement *a_pGfx);
		cGuiGfxElement *GetCurrentPointer(){return m_pGfxCurrentPointer;}

		void SetIs3D(bool a_bX);
		bool Is3D(){return m_bIs3D;}

		void Set3DSize(const cVector3f &a_vSize);
		cVector3f Get3DSize() {return m_v3DSize;}

		void Set3DTransform(const cMatrixf &a_mtxTransform);
		cMatrixf Get3DTransform() {return m_mtx3DTransform;}

		bool HasFocus();

		void SetSkin(cGuiSkin *a_pSkin);
		cGuiSkin *GetSkin(){return m_pSkin;}

		bool IsDestroyingSet(){return m_bDestroyingSet;}

	private:
		void RenderClipRegion();

		void AddWidget(iWidget *a_pWidget, iWidget *a_pParent);

		bool OnMouseMove(cGuiMessageData &a_Data);
		bool OnMouseDown(cGuiMessageData &a_Data);
		bool OnMouseUp(cGuiMessageData &a_Data);
		bool OnMouseDoubleClick(cGuiMessageData &a_Data);

		bool OnKeyPress(cGuiMessageData &a_Data);

		bool DrawMouse(iWidget *a_pWidget, cGuiMessageData &a_Data);
		kGuiCallbackDeclarationEnd(DrawMouse);

		cGui *m_pGui;
		cGuiSkin *m_pSkin;

		tString m_sName;

		cResources *m_pResources;
		cGraphics *m_pGraphics;
		cSound *m_pSound;
		cScene *m_pScene;

		iWidget *m_pAttentionWidget;

		iWidget *m_pFocusedWidget;

		iWidget *m_pWidgetRoot;
		tWidgetList m_lstWidgets;

		tGuiRenderObjectSet m_setRenderObjects;

		cVector2f m_vVirtualSize;
		float m_fVirtualMinZ;
		float m_fVirtualMaxZ;

		cVector3f m_vDrawOffset;

		bool m_bCullBackface;
		bool m_bIs3D;
		cVector3f m_v3DSize;
		cMatrixf m_mtx3DTransform;
		int m_lDrawPrio;

		bool m_bActive;
		bool m_bDrawMouse;
		float m_fMouseZ;
		cGuiGfxElement *m_pGfxCurrentPointer;

		bool m_vMouseDown[3];
		cVector2f m_vMousePos;

		tGuiPopUpList m_lstPopUps;

		cGuiClipRegion m_BaseClipRegion;
		cGuiClipRegion *m_pCurrentClipRegion;

		bool m_bDestroyingSet;

		iDepthStencilState *m_pNoDepth;
		iDepthStencilState *m_pDepthEnabled;
	};
};
#endif