#include "gui/Gui.h"

#include "graphics/Graphics.h"
#include "sound/Sound.h"

#include "gui/GuiSet.h"
#include "gui/GuiSkin.h"
#include "gui/GuiGfxElement.h"

#include "resources/Resources.h"
#include "resources/TextureManager.h"
#include "resources/ImageManager.h"
#include "resources/ResourceImage.h"
//#include "resources/FrameBitmap.h"
#include "resources/FileSearcher.h"

#include "gui/GuiMaterialBasicTypes.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGui::cGui() : iUpdateable("EFE_Gui")
	{
		m_pSetInFocus = NULL;

		m_lLastRenderTime = 0;
	}

	cGui::~cGui()
	{
		Log("Exiting Gui Module\n");
		Log("-----------------------------------------------------\n");

		STLMapDeleteAll(m_mapSets);
		STLMapDeleteAll(m_mapSkins);

		STLDeleteAll(m_lstGfxElements);

		for (int i=0;i<eGuiMaterial_LastEnum;i++)
			if (m_vMaterials[i]) efeDelete(m_vMaterials[i]);

		Log("-----------------------------------------------------\n\n");
	}
	
	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cGui::Init(cResources *a_pResources, cGraphics *a_pGraphics,
			cSound *a_pSound, cScene *a_pScene)
	{
		m_pResources = a_pResources;
		m_pGraphics = a_pGraphics;
		m_pSound = a_pSound;
		m_pScene = a_pScene;

		for (int i=0;i<eGuiMaterial_LastEnum;i++) m_vMaterials[i]=NULL;

		m_vMaterials[eGuiMaterial_Diffuse] = efeNew(cGuiMaterial_Diffuse, (m_pGraphics->GetLowLevel()));
		m_vMaterials[eGuiMaterial_Alpha] = efeNew(cGuiMaterial_Alpha, (m_pGraphics->GetLowLevel()));
		m_vMaterials[eGuiMaterial_FontNormal] = efeNew(cGuiMaterial_FontNormal, (m_pGraphics->GetLowLevel()));
		m_vMaterials[eGuiMaterial_Additive] = efeNew(cGuiMaterial_Additive, (m_pGraphics->GetLowLevel()));
		m_vMaterials[eGuiMaterial_Modulative] = efeNew(cGuiMaterial_Modulative, (m_pGraphics->GetLowLevel()));
		m_vMaterials[eGuiMaterial_Inverse] = efeNew(cGuiMaterial_Inverse, (m_pGraphics->GetLowLevel()));

		GenerateSkinTypeStrings();
	}

	//--------------------------------------------------------------
	
	void cGui::Update(float a_fTimeStep)
	{
		tGuiSetMapIt it = m_mapSets.begin();
		for (;it!=m_mapSets.end();++it)
		{
			cGuiSet *pSet = it->second;
			pSet->Update(a_fTimeStep);
		}

		tGuiGfxElementListIt gfxIt = m_lstGfxElements.begin();
		for (;gfxIt!=m_lstGfxElements.end();++gfxIt)
		{
			cGuiGfxElement *pGfx = *gfxIt;
			pGfx->Update(a_fTimeStep);
		}
	}

	//--------------------------------------------------------------

	void cGui::OnPostSceneDraw()
	{
		unsigned long lCurrentTime = GetApplicationTime();
		if (m_lLastRenderTime > lCurrentTime) m_lLastRenderTime = lCurrentTime;

		float fTimeStep = (float) (lCurrentTime - m_lLastRenderTime) / 1000.0f;
		m_lLastRenderTime = lCurrentTime;

		tGuiSetMapIt setIt  = m_mapSets.begin();
		for (;setIt != m_mapSets.end(); ++setIt)
		{
			cGuiSet *pSet = setIt->second;
			pSet->DrawAll(fTimeStep);
		}

		setIt = m_mapSets.begin();
		for (;setIt != m_mapSets.end(); ++setIt)
		{
			cGuiSet *pSet = setIt->second;
			if (pSet->Is3D()) pSet->Render();
		}
	}

	//--------------------------------------------------------------

	void cGui::OnPostGUIDraw()
	{
		typedef std::multimap<int, cGuiSet*> tPrioMap;
		tPrioMap mapSortedSets;

		tGuiSetMapIt it = m_mapSets.begin();
		for (;it != m_mapSets.end();++it)
		{
			cGuiSet *pSet = it->second;

			mapSortedSets.insert(tPrioMap::value_type(pSet->GetDrawPriority(),pSet));
		}

		tPrioMap::iterator SortIt = mapSortedSets.begin();
		for (;SortIt != mapSortedSets.end(); ++SortIt)
		{
			cGuiSet *pSet = SortIt->second;
			if (pSet->Is3D()==false) pSet->Render();
		}
	}

	//--------------------------------------------------------------

	iGuiMaterial *cGui::GetMaterial(eGuiMaterial a_Type)
	{
		return m_vMaterials[a_Type];
	}

	//--------------------------------------------------------------

	cGuiSkin *cGui::CreateSkin(const tString &a_sFile)
	{
		tString sName = cString::SetFileExt(a_sFile, "");
		cGuiSkin *pSkin = efeNew(cGuiSkin, (sName, this));

		tString sPath = m_pResources->GetFileSearcher()->GetFilePath(a_sFile);

		if (pSkin->LoadFromFile(sPath)==false)
		{
			efeDelete(pSkin);
			return NULL;
		}

		m_mapSkins.insert(tGuiSkinMap::value_type(sName, pSkin));
		return pSkin;
	}

	//--------------------------------------------------------------

	eGuiSkinGfx cGui::GetSkinGfxFromString(const tString &a_sType)
	{
		tGuiSkinGfxMapIt it = m_mapSkinGfxStrings.find(a_sType);
		if (it == m_mapSkinGfxStrings.end())
		{
			Warning("Skin gfx type '%s' does not exist!\n", a_sType.c_str());
			return eGuiSkinGfx_LastEnum;
		}

		return it->second;
	}

	eGuiSkinFont cGui::GetSkinFontFromString(const tString &a_sType)
	{
		tGuiSkinFontMapIt it = m_mapSkinFontStrings.find(a_sType);
		if (it == m_mapSkinFontStrings.end())
		{
			Warning("Skin gfx type '%s' does not exist!\n", a_sType.c_str());
			return eGuiSkinFont_LastEnum;
		}

		return it->second;
	}

	eGuiSkinAttribute cGui::GetSkinAttributeFromString(const tString &a_sType)
	{
		tGuiSkinAttributeMapIt it = m_mapSkinAttributeStrings.find(a_sType);
		if (it == m_mapSkinAttributeStrings.end())
		{
			Warning("Skin gfx type '%s' does not exist!\n", a_sType.c_str());
			return eGuiSkinAttribute_LastEnum;
		}

		return it->second;
	}

	//--------------------------------------------------------------

	cGuiSet *cGui::CreateSet(const tString &a_sName, cGuiSkin *a_pSkin)
	{
		cGuiSet *pSet = efeNew(cGuiSet, (a_sName,this,a_pSkin,m_pResources,m_pGraphics,m_pSound,m_pScene));

		m_mapSets.insert(tGuiSetMap::value_type(a_sName, pSet));

		return pSet;
	}

	//--------------------------------------------------------------

	cGuiSet *cGui::GetSetFromName(const tString &a_sName)
	{
		tGuiSetMapIt it = m_mapSets.find(a_sName);
		if (it==m_mapSets.end()) return NULL;

		return it->second;
	}

	//--------------------------------------------------------------

	void cGui::SetFocus(cGuiSet *a_pSet)
	{
		if (m_pSetInFocus = a_pSet) return;

		m_pSetInFocus = a_pSet;
	}

	//--------------------------------------------------------------

	void cGui::SetFocusByName(const tString &a_sSetName)
	{
		cGuiSet *pSet = GetSetFromName(a_sSetName);

		if (pSet) SetFocus(pSet);
	}

	void cGui::DestroySet(cGuiSet *a_pSet)
	{
		if (a_pSet==NULL) return;

		tGuiSetMapIt it = m_mapSets.begin();
		for (; it != m_mapSets.end();)
		{
			if (a_pSet == it->second)
			{
				efeDelete(a_pSet);
				m_mapSets.erase(it++);
				break;
			}
			else
				++it;
		}
	}

	//--------------------------------------------------------------

	cGuiGfxElement *cGui::CreateGfxFilledRect(const cColor &a_Color, eGuiMaterial a_Material, bool a_bAddToList)
	{
		cGuiGfxElement *pGfxElem = efeNew(cGuiGfxElement, (this));

		pGfxElem->SetColor(a_Color);
		pGfxElem->SetMaterial(GetMaterial(a_Material));

		if (a_bAddToList) m_lstGfxElements.push_back(pGfxElem);

		return pGfxElem;
	}

	//--------------------------------------------------------------

	cGuiGfxElement *cGui::CreateGfxImage(const tString &a_sFile, eGuiMaterial a_Material,
			const cColor &a_Color,bool a_bAddToList)
	{
		cResourceImage *pImage = m_pResources->GetImageManager()->CreateImage(a_sFile);
		if (pImage==NULL)
		{
			Error("Could not load image '%s'!\n", a_sFile.c_str());
			return NULL;
		}

		cGuiGfxElement *pGfxElem = efeNew(cGuiGfxElement, (this));

		pGfxElem->SetColor(a_Color);
		pGfxElem->SetMaterial(GetMaterial(a_Material));
		pGfxElem->AddImage(pImage);

		if (a_bAddToList) m_lstGfxElements.push_back(pGfxElem);

		return pGfxElem;
	}

	//--------------------------------------------------------------

	cGuiGfxElement *cGui::CreateGfxTexture(const tString &a_sFile, eGuiMaterial a_Material,
										const cColor &a_Color, bool a_bMipMaps, bool a_bAddToList)
	{
		iTexture *pTexture = m_pResources->GetTextureManager()->Create2D(a_sFile, a_bMipMaps, false);
		if (pTexture==NULL)
		{
			Error("Could not load texture '%s'!\n", a_sFile.c_str());
			return NULL;
		}

		cGuiGfxElement *pGfxElem = efeNew(cGuiGfxElement, (this));

		pGfxElem->SetColor(a_Color);
		pGfxElem->SetMaterial(GetMaterial(a_Material));
		pGfxElem->AddTexture(pTexture);

		if (a_bAddToList) m_lstGfxElements.push_back(pGfxElem);

		return pGfxElem;
	}

	//--------------------------------------------------------------

	cGuiGfxElement *cGui::CreateGfxImageBuffer(const tString &a_sFile, eGuiMaterial a_Material,
		bool a_bCreateAnimation,
		const cColor &a_Color,bool a_bAddToList)
	{
		tString sName = cString::SetFileExt(a_sFile, "");
		tString sExt = cString::GetFileExt(a_sFile);

		std::vector<cResourceImage *> vImages;

		int lFileNum=0;
		while(true)
		{
			tString sNum = lFileNum<=9 ? "0"+cString::ToString(lFileNum) : cString::ToString(lFileNum);
			tString sFile = sName + sNum+"."+sExt;

			if (m_pResources->GetFileSearcher()->GetFilePath(sFile)=="") break;

			cResourceImage *pImage = m_pResources->GetImageManager()->CreateImage(sFile);
			vImages.push_back(pImage);
			++lFileNum;
		}

		if (vImages.empty())
		{
			Error("Could not load any images with '%s' at base!\n", a_sFile.c_str());
			return NULL;
		}

		cGuiGfxElement *pGfxElem = efeNew(cGuiGfxElement, (this));

		pGfxElem->SetColor(a_Color);
		pGfxElem->SetMaterial(GetMaterial(a_Material));

		for (int i=0; i<(int)vImages.size(); ++i)
		{
			if (i==0) pGfxElem->AddImage(vImages[i]);
			pGfxElem->AddImageToBuffer(vImages[i]);
		}

		if (a_bAddToList) m_lstGfxElements.push_back(pGfxElem);

		if (a_bCreateAnimation)
		{
			cGuiGfxAnimation *pAnim = pGfxElem->CreateAnimation("Default");
			for (size_t i=0; i<(int)vImages.size(); ++i)
				pAnim->AddFrame((int)i);
		}

		return pGfxElem;
	}

	//--------------------------------------------------------------

	void cGui::DestroyGfx(cGuiGfxElement *a_pGfx)
	{
		STLFindAndDelete(m_lstGfxElements, a_pGfx);
	}

	//--------------------------------------------------------------

	bool cGui::SendMousePos(const cVector2f &a_vPos, const cVector2f &a_vRel)
	{
		if (m_pSetInFocus==NULL) return false;

		cGuiMessageData data = cGuiMessageData(a_vPos, a_vRel);
		return m_pSetInFocus->SendMessage(eGuiMessage_MouseMove, data);
	}

	bool cGui::SendMouseClickDown(eGuiMouseButton a_Button)
	{
		if (m_pSetInFocus==NULL) return false;

		cGuiMessageData data = cGuiMessageData(a_Button);
		return m_pSetInFocus->SendMessage(eGuiMessage_MouseDown, data);
	}

	bool cGui::SendMouseClickUp(eGuiMouseButton a_Button)
	{
		if (m_pSetInFocus==NULL) return false;

		cGuiMessageData data = cGuiMessageData(a_Button);
		return m_pSetInFocus->SendMessage(eGuiMessage_MouseUp, data);
	}

	bool cGui::SendMouseDoubleClick(eGuiMouseButton a_Button)
	{
		if (m_pSetInFocus==NULL) return false;

		cGuiMessageData data = cGuiMessageData(a_Button);
		return m_pSetInFocus->SendMessage(eGuiMessage_MouseDoubleClick, data);
	}

	bool cGui::SendKeyPress(const cKeyPress &keyPress)
	{
		if (m_pSetInFocus==NULL) return false;

		cGuiMessageData data = cGuiMessageData(keyPress);
		return m_pSetInFocus->SendMessage(eGuiMessage_KeyPress, data);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	#define AddFont(name) m_mapSkinFontStrings.insert(tGuiSkinFontMap::value_type(#name,eGuiSkinFont_##name))
	#define AddAttribute(name) m_mapSkinAttributeStrings.insert(tGuiSkinAttributeMap::value_type(#name,eGuiSkinAttribute_##name))
	#define AddGfx(name) m_mapSkinGfxStrings.insert(tGuiSkinGfxMap::value_type(#name,eGuiSkinGfx_##name))
	void cGui::GenerateSkinTypeStrings()
	{
		//Fonts
		AddFont(Default);
		AddFont(Disabled);
		AddFont(WindowLabel);

		//Attributes
		AddAttribute(WindowLabelTextOffset);
		AddAttribute(ButtonPressedContentOffset);
		AddAttribute(SliderButtonSize);
		AddAttribute(ListBoxSliderWidth);
		AddAttribute(ComboBoxButtonWidth);
		AddAttribute(ComboBoxSliderWidth);

		//Pointer Graphics
		AddGfx(PointerNormal);
		AddGfx(PointerText);

		//Widnow Graphics
		AddGfx(WindowBorderRight);
		AddGfx(WindowBorderLeft);
		AddGfx(WindowBorderUp);
		AddGfx(WindowBorderDown);

		AddGfx(WindowCornerLU);
		AddGfx(WindowCornerRU);
		AddGfx(WindowCornerRD);
		AddGfx(WindowCornerLD);

		AddGfx(WindowLabel);
		AddGfx(WindowBackground);

		//Frame Graphics

		AddGfx(FrameBorderRight);
		AddGfx(FrameBorderLeft);
		AddGfx(FrameBorderUp);
		AddGfx(FrameBorderDown);

		AddGfx(FrameCornerLU);
		AddGfx(FrameCornerRU);
		AddGfx(FrameCornerRD);
		AddGfx(FrameCornerLD);

		AddGfx(FrameBackground);

		//Check box Graphics
		AddGfx(CheckBoxEnabledUnchecked);
		AddGfx(CheckBoxEnabledChecked);
		AddGfx(CheckBoxDisabledUnchecked);
		AddGfx(CheckBoxDisabledChecked);

		//Text box Graphics
		AddGfx(TextBoxBackground);
		AddGfx(TextBoxSelectedTextBack);
		AddGfx(TextBoxMarker);

		//List box Graphics
		AddGfx(ListBoxBackground);

		//List box Graphics
		AddGfx(ComboBoxButtonIcon);

		AddGfx(ComboBoxBorderRight);
		AddGfx(ComboBoxBorderLeft);
		AddGfx(ComboBoxBorderUp);
		AddGfx(ComboBoxBorderDown);

		AddGfx(ComboBoxCornerLU);
		AddGfx(ComboBoxCornerRU);
		AddGfx(ComboBoxCornerRD);
		AddGfx(ComboBoxCornerLD);

		AddGfx(ComboBoxBackground);

		//Slider Graphics
		AddGfx(SliderVertArrowUp);
		AddGfx(SliderVertArrowDown);
		AddGfx(SliderVertBackground);

		AddGfx(SliderHoriArrowUp);
		AddGfx(SliderHoriArrowDown);
		AddGfx(SliderHoriBackground);

		//Button Graphics
		AddGfx(ButtonUpBorderRight);
		AddGfx(ButtonUpBorderLeft);
		AddGfx(ButtonUpBorderUp);
		AddGfx(ButtonUpBorderDown);

		AddGfx(ButtonUpCornerLU);
		AddGfx(ButtonUpCornerRU);
		AddGfx(ButtonUpCornerRD);
		AddGfx(ButtonUpCornerLD);

		AddGfx(ButtonUpBackground);

		AddGfx(ButtonDownBorderRight);
		AddGfx(ButtonDownBorderLeft);
		AddGfx(ButtonDownBorderUp);
		AddGfx(ButtonDownBorderDown);

		AddGfx(ButtonDownCornerLU);
		AddGfx(ButtonDownCornerRU);
		AddGfx(ButtonDownCornerRD);
		AddGfx(ButtonDownCornerLD);

		AddGfx(ButtonDownBackground);
	}
}