#include "gui/GuiSet.h"

#include "math/Math.h"
#include "system/LowLevelSystem.h"

#include "graphics/LowLevelGraphics.h"
#include "graphics/Graphics.h"
#include "graphics/FontData.h"

#include "resources/Resources.h"

#include "resources/ImageManager.h"
#include "resources/ResourceImage.h"

#include "resources/FileSearcher.h"

#include "scene/Scene.h"
#include "scene/Camera3D.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/Widget.h"
#include "gui/GuiMaterial.h"
#include "gui/GuiGfxElement.h"

#include "gui/GuiPopUpMessageBox.h"

#include "gui/WidgetWindow.h"
#include "gui/WidgetFrame.h"
#include "gui/WidgetButton.h"
#include "gui/WidgetLabel.h"
#include "gui/WidgetSlider.h"
#include "gui/WidgetTextBox.h"
#include "gui/WidgetCheckBox.h"
#include "gui/WidgetImage.h"
#include "gui/WidgetListBox.h"
#include "gui/WidgetComboBox.h"

namespace efe
{
	bool cGuiRenderObjectCompare::operator()(const cGuiRenderObject &a_ObjectA,const cGuiRenderObject &a_ObjectB)
	{
		float fZA = a_ObjectA.m_vPos.z;
		float fZB = a_ObjectB.m_vPos.z;
		if (fZA != fZB)
			return fZA < fZB;

		cGuiClipRegion *pClipA = a_ObjectA.m_pClipRegion;
		cGuiClipRegion *pClipB = a_ObjectB.m_pClipRegion;
		if (pClipA != pClipB)
			return pClipA > pClipB;

		//Material
		iGuiMaterial *pMaterialA = a_ObjectA.m_pCustomMaterial ? a_ObjectA.m_pCustomMaterial : a_ObjectA.m_pGfx->m_pMaterial;
		iGuiMaterial *pMaterialB = a_ObjectB.m_pCustomMaterial ? a_ObjectB.m_pCustomMaterial : a_ObjectB.m_pGfx->m_pMaterial;
		if (pMaterialA != pMaterialB)
			return pMaterialA > pMaterialB;

		//Texture
		iTexture *pTextureA = a_ObjectA.m_pGfx->m_vTextures[0];
		iTexture *pTextureB = a_ObjectB.m_pGfx->m_vTextures[0];
		if (pTextureA != pTextureB)
			return pTextureA > pTextureB;

		return false;
	}

	//////////////////////////////////////////////////////////////
	// CLIP REGION
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGuiClipRegion::~cGuiClipRegion()
	{
		Clear();
	}

	void cGuiClipRegion::Clear()
	{
		STLDeleteAll(m_lstChildren);
	}

	cGuiClipRegion *cGuiClipRegion::CreateChild(const cVector3f &a_vPos, const cVector2f &a_vSize)
	{
		cGuiClipRegion *pRegion = efeNew(cGuiClipRegion, ());

		if (m_Rect.w<0)
			pRegion->m_Rect = cRectf(cVector2f(a_vPos.x, a_vPos.y), a_vSize);
		else
		{
			cRectf temp = cRectf(cVector2f(a_vPos.x, a_vPos.y), a_vSize);
			pRegion->m_Rect = cMath::ClipRect(temp,m_Rect);
			if (pRegion->m_Rect.w < 0) pRegion->m_Rect.w = 0;
			if (pRegion->m_Rect.h < 0) pRegion->m_Rect.h = 0;
		}

		m_lstChildren.push_back(pRegion);

		return pRegion;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGuiSet::cGuiSet(const tString &a_sName, cGui *a_pGui, cGuiSkin *a_pSkin,
			cResources *a_pResources, cGraphics *a_pGraphics,
			cSound *a_pSound, cScene *a_pScene)
	{
		m_pGui = a_pGui;
		m_pSkin = a_pSkin;

		m_sName = a_sName;

		m_pResources = a_pResources;
		m_pGraphics = a_pGraphics;
		m_pSound = a_pSound;
		m_pScene = a_pScene;

		m_pGfxCurrentPointer = NULL;

		m_pFocusedWidget = NULL;

		m_pAttentionWidget = NULL;

		m_vDrawOffset = 0;

		m_vVirtualSize = m_pGraphics->GetLowLevel()->GetVirtualSize();
		m_fVirtualMinZ = -1000;
		m_fVirtualMaxZ = 1000;

		m_bActive = true;
		m_bDrawMouse = true;
		m_fMouseZ = 20;

		m_bIs3D = false;
		m_v3DSize = 1;
		m_mtx3DTransform = cMatrixf::Identity;
		m_bCullBackface = false;

		m_pWidgetRoot = efeNew(iWidget, (eWidgetType_Root, this, m_pSkin));
		m_pWidgetRoot->AddCallback(eGuiMessage_OnDraw, this, kGuiCallback(DrawMouse));

		m_pCurrentClipRegion = &m_BaseClipRegion;

		m_bDestroyingSet = false;

		m_lDrawPrio = 0;

		for (int i=0; i<3; ++i) m_vMouseDown[i] = false;

		SetSkin(a_pSkin);

		m_pNoDepth = m_pGraphics->GetLowLevel()->CreateDepthStencilState(false, false);
		m_pDepthEnabled = m_pGraphics->GetLowLevel()->CreateDepthStencilState(true, false);
	}

	//--------------------------------------------------------------

	cGuiSet::~cGuiSet()
	{
		m_bDestroyingSet = true;

		//STLDeleteAll(m_lstPopUps);
		STLDeleteAll(m_lstWidgets);
		efeDelete(m_pWidgetRoot);

		m_bDestroyingSet = false;

		efeDelete(m_pNoDepth);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cGuiSet::Update(float a_fTimeStep)
	{
		//if (m_lstPopUps

		tWidgetListIt it = m_lstWidgets.begin();
		for (;it != m_lstWidgets.end(); ++it)
		{
			iWidget *pWidget = *it;
			pWidget->Update(a_fTimeStep);
		}
	}

	//--------------------------------------------------------------

	void cGuiSet::DrawAll(float a_fTimeStep)
	{
		if (m_bActive == false) return;

		SetCurrentClipRegion(&m_BaseClipRegion);
		m_pWidgetRoot->Draw(a_fTimeStep, &m_BaseClipRegion);

		SetCurrentClipRegion(&m_BaseClipRegion);
	}

	//--------------------------------------------------------------

#undef SendMessage

	bool cGuiSet::SendMessage(eGuiMessage a_Message, cGuiMessageData &a_Data)
	{
		switch(a_Message)
		{
		case eGuiMessage_MouseMove:			return OnMouseMove(a_Data);
		case eGuiMessage_MouseDown:			return OnMouseDown(a_Data);
		case eGuiMessage_MouseUp:			return OnMouseUp(a_Data);
		case eGuiMessage_MouseDoubleClick:	return OnMouseDoubleClick(a_Data);

		case eGuiMessage_KeyPress:			return OnKeyPress(a_Data);
		}

		return false;
	}

#define SendMessage

	//--------------------------------------------------------------

	void cGuiSet::Render()
	{
		iLowLevelGraphics *pLowLevelGraphics = m_pGraphics->GetLowLevel();

		if (m_bIs3D)
		{
			cCamera3D *pCam = static_cast<cCamera3D*>(m_pScene->GetCamera());

			pLowLevelGraphics->SetDepthState(m_pDepthEnabled);

			cMatrixf mtxPreMul = cMath::MatrixScale(cVector3f(	m_v3DSize.x / m_vVirtualSize.x,
																-m_v3DSize.y / m_vVirtualSize.y,
																m_v3DSize.z / (m_fVirtualMaxZ - m_fVirtualMinZ)));

			cMatrixf mtxModel = cMath::MatrixMul(m_mtx3DTransform, mtxPreMul);
			mtxModel = cMath::MatrixMul(pCam->GetViewMatrix(), mtxModel);
		}
		else
		{
			pLowLevelGraphics->SetDepthState(m_pNoDepth);
			pLowLevelGraphics->Set2DMode();
		}

		RenderClipRegion();

		m_BaseClipRegion.Clear();
	}

	//--------------------------------------------------------------

	void cGuiSet::DrawGfx(cGuiGfxElement *a_pGfx, const cVector3f &a_vPos, const cVector2f &a_vSize,
					const cColor &a_Color,
					eGuiMaterial a_Material)
	{
		if (m_pCurrentClipRegion == NULL) return;
		if (m_pCurrentClipRegion->m_Rect.w==0 || m_pCurrentClipRegion->m_Rect.h==0) return;

		cVector3f vAbsPos = a_vPos + a_pGfx->GetOffset() + m_vDrawOffset;
		if ((m_pCurrentClipRegion->m_Rect.w)>0)
		{
			cRectf gfxRect;
			gfxRect.x = vAbsPos.x;
			gfxRect.y = vAbsPos.y;
			if (a_vSize.x < 0)
			{
				gfxRect.w = a_pGfx->GetImageSize().x;
				gfxRect.h = a_pGfx->GetImageSize().y;
			}
			else
			{
				gfxRect.w = a_vSize.x;
				gfxRect.h = a_vSize.y;
			}

			if (cMath::BoxCollision(m_pCurrentClipRegion->m_Rect, gfxRect)==false)return;
		}

		a_pGfx->Flush();

		cGuiRenderObject object;

		object.m_pGfx = a_pGfx;
		object.m_pClipRegion = m_pCurrentClipRegion;
		object.m_vPos = vAbsPos;
		if (a_vSize.x < 0)	object.m_vSize = a_pGfx->GetImageSize();
		else				object.m_vSize = a_vSize;
		object.m_Color = a_Color;
		if (a_Material != eGuiMaterial_LastEnum)	object.m_pCustomMaterial = m_pGui->GetMaterial(a_Material);
		else										object.m_pCustomMaterial = NULL;

		m_setRenderObjects.insert(object);
	}

	//--------------------------------------------------------------

	void cGuiSet::DrawFont(const tWString &a_sText,
						iFontData *a_pFont, const cVector3f &a_vPos,
						const cVector2f &a_vSize, const cColor &a_Color,
						eFontAlign a_Align,
						eGuiMaterial a_Material)
	{
		int lCount = 0;
		float lXAdd = 0;
		cVector3f vPos = a_vPos;

		if (a_Align == eFontAlign_Center)
			vPos.x -= a_pFont->GetLength(a_vSize, a_sText.c_str())/2;
		else if(a_Align == eFontAlign_Right)
			vPos.x -= a_pFont->GetLength(a_vSize, a_sText.c_str());

		while(a_sText[lCount] != 0)
		{
			wchar_t lGlyphNum = ((wchar_t)a_sText[lCount]);
			if (lGlyphNum < a_pFont->GetFirstChar() ||
				lGlyphNum > a_pFont->GetLastChar())
			{
				lCount++;
				continue;
			}
			lGlyphNum -= a_pFont->GetFirstChar();

			cGlyph *pGlyph = a_pFont->GetGlyph(lGlyphNum);
			if (pGlyph)
			{
				cVector2f vOffset(pGlyph->m_vOffset * a_vSize);
				cVector2f vSize(pGlyph->m_vSize * a_vSize);

				DrawGfx(pGlyph->m_pGuiGfx, vPos + vOffset, vSize, a_Color, a_Material);

				vPos.x += pGlyph->m_fAdvance*a_vSize.x;
			}
			lCount++;
		}
	}

	//--------------------------------------------------------------

	cWidgetWindow *cGuiSet::CreateWidgetWindow(	const cVector3f &a_vLocalPos,
												const cVector2f &a_vSize,
												const tWString &a_sText,
												iWidget *a_pParent,
												const tString &a_sName)
	{
		cWidgetWindow *pWindow = efeNew(cWidgetWindow, (this, m_pSkin));
		pWindow->SetPosition(a_vLocalPos);
		pWindow->SetSize(a_vSize);
		pWindow->SetText(a_sText);
		pWindow->SetName(a_sName);
		AddWidget(pWindow, a_pParent);
		return pWindow;
	}

	cWidgetFrame *cGuiSet::CreateWidgetFrame(	const cVector3f &a_vLocalPos,
											const cVector2f &a_vSize,
											bool a_bDrawFrame,
											iWidget *a_pParent,
											const tString &a_sName)
	{
		cWidgetFrame *pFrame = efeNew(cWidgetFrame, (this, m_pSkin));
		pFrame->SetPosition(a_vLocalPos);
		pFrame->SetSize(a_vSize);
		pFrame->SetDrawFrame(a_bDrawFrame);
		pFrame->SetName(a_sName);
		AddWidget(pFrame, a_pParent);
		return pFrame;
	}

	cWidgetButton *cGuiSet::CreateWidgetButton(	const cVector3f &a_vLocalPos,
												const cVector2f &a_vSize,
												const tWString &a_sText,
												iWidget *a_pParent,
												const tString &a_sName)
	{
		cWidgetButton *pButton = efeNew(cWidgetButton, (this, m_pSkin));
		pButton->SetPosition(a_vLocalPos);
		pButton->SetSize(a_vSize);
		pButton->SetText(a_sText);
		pButton->SetName(a_sName);
		AddWidget(pButton, a_pParent);
		return pButton;
	}

	cWidgetSlider *cGuiSet::CreateWidgetSlider(	eWidgetSliderOrientation a_Orientation,
												const cVector3f &a_vLocalPos,
												const cVector2f &a_vSize,
												int a_lMaxValue,
												iWidget *a_pParent,
												const tString &a_sName)
	{
		cWidgetSlider *pSlider = efeNew(cWidgetSlider, (this,m_pSkin,a_Orientation));
		pSlider->SetPosition(a_vLocalPos);
		pSlider->SetSize(a_vSize);
		pSlider->SetMaxValue(a_lMaxValue);
		pSlider->SetName(a_sName);
		AddWidget(pSlider,a_pParent);
		return pSlider;
	}

	cWidgetLabel *cGuiSet::CreateWidgetLabel(	const cVector3f &a_vLocalPos,
												const cVector2f &a_vSize,
												const tWString &a_sText,
												iWidget *a_pParent,
												const tString &a_sName)
	{
		cWidgetLabel *pLabel = efeNew(cWidgetLabel, (this, m_pSkin));
		pLabel->SetPosition(a_vLocalPos);
		pLabel->SetSize(a_vSize);
		pLabel->SetText(a_sText);
		pLabel->SetName(a_sName);
		AddWidget(pLabel, a_pParent);
		return pLabel;
	}

	cWidgetTextBox *cGuiSet::CreateWidgetTextBox(	const cVector3f &a_vLocalPos,
												const cVector2f &a_vSize,
												const tWString &a_sText,
												iWidget *a_pParent,
												const tString &a_sName)
	{
		cWidgetTextBox *pTextBox = efeNew(cWidgetTextBox, (this, m_pSkin));
		pTextBox->SetPosition(a_vLocalPos);
		pTextBox->SetSize(a_vSize);
		pTextBox->SetText(a_sText);
		pTextBox->SetName(a_sName);
		AddWidget(pTextBox, a_pParent);
		return pTextBox;
	}

	cWidgetCheckBox *cGuiSet::CreateWidgetCheckBox(	const cVector3f &a_vLocalPos,
												const cVector2f &a_vSize,
												const tWString &a_sText,
												iWidget *a_pParent,
												const tString &a_sName)
	{
		cWidgetCheckBox *pCheckBox = efeNew(cWidgetCheckBox, (this, m_pSkin));
		pCheckBox->SetPosition(a_vLocalPos);
		pCheckBox->SetSize(a_vSize);
		pCheckBox->SetText(a_sText);
		pCheckBox->SetName(a_sName);
		AddWidget(pCheckBox, a_pParent);
		return pCheckBox;
	}

	cWidgetImage *cGuiSet::CreateWidgetImage(const tString &a_sFile,
											const cVector3f &a_vLocalPos,
											const cVector2f &a_vSize,
											eGuiMaterial a_Material,
											bool a_bAnimate,
											iWidget *a_pParent,
											const tString &a_sName)
	{
		cWidgetImage *pImage = efeNew(cWidgetImage, (this, m_pSkin));

		cGuiGfxElement *pGfx = NULL;
		if (a_sFile != "")
		{
			if (a_bAnimate)
				pGfx = m_pGui->CreateGfxImageBuffer(a_sFile,a_Material,true);
			else
				pGfx = m_pGui->CreateGfxImage(a_sFile,a_Material);
		}

		pImage->SetPosition(a_vLocalPos);

		if (pGfx && a_vSize.x < 0)
			pImage->SetSize(pGfx->GetImageSize());
		else
			pImage->SetSize(a_vSize);

		pImage->SetImage(pGfx);

		pImage->SetName(a_sName);

		AddWidget(pImage,a_pParent);
		return pImage;
	}

	cWidgetListBox *cGuiSet::CreateWidgetListBox(const cVector3f &a_vLocalPos,
											const cVector2f &a_vSize,
											iWidget *a_pParent,
											const tString &a_sName)
	{
		cWidgetListBox *pListBox = efeNew(cWidgetListBox, (this, m_pSkin));
		pListBox->SetPosition(a_vLocalPos);
		pListBox->SetSize(a_vSize);
		pListBox->SetName(a_sName);
		AddWidget(pListBox, a_pParent);
		return pListBox;
	}

	cWidgetComboBox *cGuiSet::CreateWidgetComboBox(	const cVector3f &a_vLocalPos,
												const cVector2f &a_vSize,
												const tWString &a_sText,
												iWidget *a_pParent,
												const tString &a_sName)
	{
		cWidgetComboBox *pComboBox = efeNew(cWidgetComboBox, (this,m_pSkin));
		pComboBox->SetPosition(a_vLocalPos);
		pComboBox->SetSize(a_vSize);
		pComboBox->SetText(a_sText);
		pComboBox->SetName(a_sName);
		AddWidget(pComboBox, a_pParent);
		return pComboBox;
	}

	void cGuiSet::DestroyWidget(iWidget *a_pWidget)
	{
		if (a_pWidget == m_pFocusedWidget) m_pFocusedWidget = NULL;
		STLFindAndDelete(m_lstWidgets, a_pWidget);
	}

	//--------------------------------------------------------------

	void cGuiSet::CreatePopUpMessageBox(const tWString &a_sLabel, const tWString &a_sText,
										const tWString &a_sButton1, const tWString &a_sButton2,
										void *a_pCallbackObject, tGuiCallbackFunc a_pCallback)
	{
		cGuiPopUpMessageBox *pMessageBox = efeNew(cGuiPopUpMessageBox, (this, a_sLabel, a_sText,
																a_sButton1, a_sButton2,
																a_pCallbackObject, a_pCallback));

	}

	//--------------------------------------------------------------

	void cGuiSet::DestroyPopUp(iGuiPopUp *a_pPopUp)
	{
		m_lstPopUps.push_back(a_pPopUp);
	}

	//--------------------------------------------------------------

	void cGuiSet::SetVirtualSize(const cVector2f &a_vSize, float a_fMinZ, float a_fMaxZ)
	{
		m_vVirtualSize = a_vSize;
		m_fVirtualMinZ = a_fMinZ;
		m_fVirtualMaxZ = a_fMaxZ;
	}

	//--------------------------------------------------------------

	void cGuiSet::SetFocusedWidget(iWidget *a_pWidget)
	{
		if (m_pFocusedWidget)
		{
			cGuiMessageData data = cGuiMessageData(m_vMousePos,0);
			m_pFocusedWidget->ProcessMessage(eGuiMessage_LostFocus,data);
		}

		m_pFocusedWidget = a_pWidget;
		if (m_pFocusedWidget)
		{
			cGuiMessageData data = cGuiMessageData(m_vMousePos,0);
			m_pFocusedWidget->ProcessMessage(eGuiMessage_GotFocus,data);
		}
	}

	//--------------------------------------------------------------

	void cGuiSet::SetAttentionWidget(iWidget *a_pWidget)
	{
		if (m_pAttentionWidget == a_pWidget) return;

		m_pAttentionWidget = a_pWidget;

		if (m_pFocusedWidget && m_pFocusedWidget->IsConnectedTo(m_pAttentionWidget)==false)
		{
			cGuiMessageData data = cGuiMessageData(m_vMousePos, 0);
			m_pFocusedWidget->ProcessMessage(eGuiMessage_LostFocus, data);
			m_pFocusedWidget = NULL;
		}

		if (m_pAttentionWidget && m_pFocusedWidget == NULL)
		{
			m_pFocusedWidget = a_pWidget;
			if (m_pFocusedWidget)
			{
				cGuiMessageData data = cGuiMessageData(m_vMousePos, 0);
				m_pFocusedWidget->ProcessMessage(eGuiMessage_GotFocus, data);
			}
		}
	}

	//--------------------------------------------------------------

	void cGuiSet::SetCurrentPointer(cGuiGfxElement *a_pGfx)
	{
		m_pGfxCurrentPointer = a_pGfx;
	}

	//--------------------------------------------------------------

	void cGuiSet::SetIs3D(bool a_bX)
	{
		m_bIs3D = a_bX;
	}

	void cGuiSet::Set3DSize(const cVector3f &a_vSize)
	{
		m_v3DSize;
	}

	void cGuiSet::Set3DTransform(const cMatrixf &a_mtxTransform)
	{
		m_mtx3DTransform = a_mtxTransform;
	}

	//--------------------------------------------------------------

	bool cGuiSet::HasFocus()
	{
		return m_pGui->GetFocusedSet() == this;
	}

	//--------------------------------------------------------------

	void cGuiSet::SetSkin(cGuiSkin *a_pSkin)
	{
		m_pSkin = a_pSkin;

		if (m_pSkin)
			m_pGfxCurrentPointer = m_pSkin->GetGfx(eGuiSkinGfx_PointerNormal);
		else
			m_pGfxCurrentPointer = NULL;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

#define kLogRender (false)
	static void SetClipArea(iLowLevelGraphics *pLowLevelGraphics, cGuiClipRegion *a_pRegion)
	{
		cRectf &clipRect = a_pRegion->m_Rect;

		if (a_pRegion->m_Rect.w > 0)
		{
			cPlanef plane;
			
			cVector2f vVirtualSize = pLowLevelGraphics->GetVirtualSize();
			cVector2f vScreenSize = pLowLevelGraphics->GetScreenSize();

			cRectl vScissorRect((int)((clipRect.x / vVirtualSize.x) * vScreenSize.x), (int)((clipRect.y / vVirtualSize.y) * vScreenSize.y),
								(int)((clipRect.w / vVirtualSize.x) * vScreenSize.x), (int)((clipRect.h / vVirtualSize.y) * vScreenSize.y));

			//pLowLevelGraphics->SetScissorRect(vScissorRect);

			if (kLogRender) Log("-- Clip region: %d Clipping: x %f y %f w %f h %f\n", a_pRegion,
				a_pRegion->m_Rect.x, a_pRegion->m_Rect.y,
				a_pRegion->m_Rect.w, a_pRegion->m_Rect.h);
		}
		else if(kLogRender) Log("-- Clip region: %d No clipping!\n", a_pRegion);
	}

	void cGuiSet::RenderClipRegion()
	{
		iLowLevelGraphics *pLowLevelGraphics = m_pGraphics->GetLowLevel();

		if (kLogRender)Log("----------------\n");

		tGuiRenderObjectSet &setRenderObjects = m_setRenderObjects;
		if (setRenderObjects.empty())
		{
			if (kLogRender)Log("----------------\n");
			return;
		}

		tGuiRenderObjectSetIt it = setRenderObjects.begin();

		iGuiMaterial *pLastMaterial = NULL;
		iTexture *pLastTexture = NULL;
		cGuiClipRegion *pLastClipRegion = NULL;

		cGuiGfxElement *pGfx = it->m_pGfx;
		iGuiMaterial *pMaterial = it->m_pCustomMaterial ? it->m_pCustomMaterial : pGfx->m_pMaterial;
		iTexture *pTexture = pGfx->m_vTextures[0];
		cGuiClipRegion *pClipRegion = it->m_pClipRegion;

		int lIdxAdd = 0;
		int lcount = setRenderObjects.size();
		//Log("			COUNT = %d\n", lcount);

		while(it != setRenderObjects.end())
		{
			if (pLastMaterial != pMaterial)
			{
				pMaterial->BeforeRender();
				if (kLogRender) Log("Material %s before\n", pMaterial->GetName().c_str());
			}

			if (pLastClipRegion != pClipRegion)
				SetClipArea(pLowLevelGraphics, pClipRegion);

			pLowLevelGraphics->SetBatchTexture(pTexture);
			if (kLogRender) Log("Texture %d\n", pTexture);

			do
			{
				cGuiRenderObject object = *it;
				cGuiGfxElement *pGfx = object.m_pGfx;

				if (kLogRender)
				{
					if (pGfx->m_vImages[0])
						Log(" gfx: %d '%s'\n", pGfx, pGfx->m_vImages[0]->GetName().c_str());
					else
						Log(" gfx: %d 'null'\n");
				}

				for (int i=0;i<4;i++)
				{
					cVertex &vtx = pGfx->m_vVtx[i];
					cVector3f &vVtxPos = vtx.pos;
					cVector3f &vPos = object.m_vPos;
					cVector3f vFinPos(vVtxPos.x * object.m_vSize.x + vPos.x,
						vVtxPos.y * object.m_vSize.y + vPos.y,
						vPos.z); 
					pLowLevelGraphics->AddVertexToBatch_Raw(
						cVector3f(vVtxPos.x * object.m_vSize.x + vPos.x,
						vVtxPos.y * object.m_vSize.y + vPos.y,
						vPos.z),
						vtx.col * object.m_Color,
						vtx.tex);
				}

				pLowLevelGraphics->AddIndexToBatch(lIdxAdd + 0);
				pLowLevelGraphics->AddIndexToBatch(lIdxAdd + 1);
				pLowLevelGraphics->AddIndexToBatch(lIdxAdd + 2);
				pLowLevelGraphics->AddIndexToBatch(lIdxAdd + 2);
				pLowLevelGraphics->AddIndexToBatch(lIdxAdd + 3);
				pLowLevelGraphics->AddIndexToBatch(lIdxAdd + 0);

				lIdxAdd += 4;

				pLastMaterial = pMaterial;
				pLastTexture = pTexture;
				pLastClipRegion = pClipRegion;

				++it; if (it == setRenderObjects.end()) break;

				pGfx = it->m_pGfx;
				pMaterial = it->m_pCustomMaterial ? it->m_pCustomMaterial : pGfx->m_pMaterial;
				pTexture = it->m_pGfx->m_vTextures[0];
				pClipRegion = it->m_pClipRegion;
			}
			while(	pTexture == pLastTexture &&
					pMaterial == pLastMaterial &&
					pClipRegion == pLastClipRegion);

			pLowLevelGraphics->FlushQuadBatch();

			pLowLevelGraphics->ClearBatch();
			lIdxAdd = 0;

			if (pLastClipRegion != pClipRegion || it == setRenderObjects.end())
			{
				if (pLastClipRegion->m_Rect.w > 0)
				{}
			}

			if (pLastMaterial != pMaterial  || it == setRenderObjects.end())
			{
				pLastMaterial->AfterRender();
				if (kLogRender) Log("Material %d '%s' after. new: %d '%s'\n", pLastMaterial, pLastMaterial->GetName().c_str(),
																			pMaterial, pMaterial->GetName().c_str());
			}
		}

		m_setRenderObjects.clear();

		if (kLogRender) Log("---------- END %d ----------\n");
	}

	//--------------------------------------------------------------

	void cGuiSet::AddWidget(iWidget *a_pWidget, iWidget *a_pParent)
	{
		m_lstWidgets.push_front(a_pWidget);

		if (a_pParent)
			a_pParent->AttachChild(a_pWidget);
		else
			m_pWidgetRoot->AttachChild(a_pWidget);

		a_pWidget->Init();
	}

	//--------------------------------------------------------------

	bool cGuiSet::OnMouseMove(cGuiMessageData &a_Data)
	{
		m_vMousePos = a_Data.m_vPos;

		a_Data.m_lVal = 0;
		if (m_vMouseDown[0]) a_Data.m_lVal |= eGuiMouseButton_Left;
		if (m_vMouseDown[1]) a_Data.m_lVal |= eGuiMouseButton_Middle;
		if (m_vMouseDown[2]) a_Data.m_lVal |= eGuiMouseButton_Right;

		bool bRet = false;
		bool bPointerSet = false;
		tWidgetListIt it = m_lstWidgets.begin();
		for (; it != m_lstWidgets.end(); ++it)
		{
			iWidget *pWidget = *it;

			if (pWidget->PointIsInside(m_vMousePos, false))
			{
				if (pWidget->GetMouseIsOver()==false)
				{
					pWidget->SetMouseIsOver(true);
					if (pWidget->ProcessMessage(eGuiMessage_MouseEnter, a_Data))
						bRet = true;
				}

				if (bPointerSet == false && pWidget->GetPointerGfx())
				{
					if (m_pAttentionWidget &&
						pWidget->IsConnectedTo(m_pAttentionWidget)==false)
					{
					}
					else
					{
						if (m_pGfxCurrentPointer != pWidget->GetPointerGfx())
						{
							if (pWidget->IsEnabled())
								SetCurrentPointer(pWidget->GetPointerGfx());
						}
						bPointerSet = true;
					}
				}
			}
			else
			{
				if (pWidget->GetMouseIsOver())
				{
					pWidget->SetMouseIsOver(false);
					pWidget->ProcessMessage(eGuiMessage_MouseLeave, a_Data);

					if (m_pFocusedWidget == pWidget && pWidget->PointIsInside(m_vMousePos, false))
					{
						pWidget->SetMouseIsOver(true);
						if (pWidget->ProcessMessage(eGuiMessage_MouseEnter, a_Data)) bRet = true;
					}
				}
			}

			if (pWidget->GetMouseIsOver() || m_pFocusedWidget == pWidget)
			{
				if (pWidget->ProcessMessage(eGuiMessage_MouseMove, a_Data))
					bRet = true;
			}
		}

		return bRet;
	}

	bool cGuiSet::OnMouseDown(cGuiMessageData &a_Data)
	{
		m_vMouseDown[cMath::Log2ToInt(a_Data.m_lVal)] = true;

		a_Data.m_vPos = m_vMousePos;

		iWidget *pOldFocus = m_pFocusedWidget;

		bool bRet = false;
		tWidgetListIt it = m_lstWidgets.begin();
		for (;it != m_lstWidgets.end(); ++it)
		{
			iWidget *pWidget = *it;

			if (m_pAttentionWidget && pWidget->IsConnectedTo(m_pAttentionWidget)==false)
				continue;

			if (pWidget->GetMouseIsOver())
			{
				if (m_pFocusedWidget != pWidget)
				{
					if (pWidget->ProcessMessage(eGuiMessage_GotFocus, a_Data))
						m_pFocusedWidget = pWidget;
				}
				else
					m_pFocusedWidget = pWidget;

				if (pWidget->ProcessMessage(eGuiMessage_MouseDown, a_Data))
				{
					bRet = true;
					break;
				}
			}
		}

		if (bRet == false)
			m_pFocusedWidget = NULL;

		if (m_pFocusedWidget != pOldFocus)
			if (pOldFocus) pOldFocus->ProcessMessage(eGuiMessage_LostFocus, a_Data);

		return bRet;
	}

	bool cGuiSet::OnMouseUp(cGuiMessageData &a_Data)
	{
		m_vMouseDown[cMath::Log2ToInt(a_Data.m_lVal)] = false;

		a_Data.m_vPos = m_vMousePos;

		bool bRet = false;

		if (m_pFocusedWidget)
			bRet = m_pFocusedWidget->ProcessMessage(eGuiMessage_MouseUp, a_Data);

		if (bRet == false)
		{
			tWidgetListIt it = m_lstWidgets.begin();
			for (;it != m_lstWidgets.end(); ++it)
			{
				iWidget *pWidget = *it;

				if (m_pAttentionWidget && pWidget->IsConnectedTo(m_pAttentionWidget)==false)
					continue;

				if (pWidget != m_pFocusedWidget && pWidget->GetMouseIsOver())
				{
					if (pWidget->ProcessMessage(eGuiMessage_MouseUp, a_Data))
					{
						bRet = true;
						break;
					}
				}
			}
		}

		return bRet;
	}

	bool cGuiSet::OnMouseDoubleClick(cGuiMessageData &a_Data)
	{
		a_Data.m_vPos = m_vMousePos;

		bool bRet = false;
		tWidgetListIt it = m_lstWidgets.begin();
		for (; it != m_lstWidgets.end(); ++it)
		{
			iWidget *pWidget = *it;

			if (m_pAttentionWidget && pWidget->IsConnectedTo(m_pAttentionWidget)==false)
				continue;

			if (pWidget->GetMouseIsOver())
			{
				if (pWidget->ProcessMessage(eGuiMessage_MouseDoubleClick, a_Data))
				{
					bRet = true;
					break;
				}
			}
		}

		return bRet;
	}

	bool cGuiSet::OnKeyPress(cGuiMessageData &a_Data)
	{
		a_Data.m_vPos = m_vMousePos;

		bool bRet = false;

		if (m_pFocusedWidget)
			bRet = m_pFocusedWidget->ProcessMessage(eGuiMessage_KeyPress, a_Data);

		if (bRet == false)
		{
			tWidgetListIt it = m_lstWidgets.begin();
			for (; it != m_lstWidgets.end(); ++it)
			{
				iWidget *pWidget = *it;

				if (m_pAttentionWidget && pWidget->IsConnectedTo(m_pAttentionWidget)==false)
					continue;

				if (pWidget->GetMouseIsOver() && m_pFocusedWidget != pWidget)
				{
					if (pWidget->ProcessMessage(eGuiMessage_KeyPress, a_Data))
					{
						bRet = true;
						break;
					}
				}
			}
		}
		return bRet;
	}

	bool cGuiSet::DrawMouse(iWidget *a_pWidget, cGuiMessageData &a_Data)
	{
		if (HasFocus() && m_bDrawMouse && m_pGfxCurrentPointer)
		{
			DrawGfx(m_pGfxCurrentPointer, cVector3f(m_vMousePos.x, m_vMousePos.y, m_fMouseZ),
				m_pGfxCurrentPointer->GetImageSize(), cColor(1,1));
		}

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiSet, DrawMouse);
}