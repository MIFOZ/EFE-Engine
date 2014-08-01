#include "gui/GuiSkin.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "impl/tinyXML/tinyxml.h"

#include "gui/Gui.h"
#include "gui/GuiGfxElement.h"

#include "resources/Resources.h"
#include "resources/FontManager.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// FONT
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGuiSkinFont::cGuiSkinFont(cGui *a_pGui)
	{
		m_pGui = a_pGui;

		m_pMaterial = NULL;
	}

	cGuiSkinFont::~cGuiSkinFont()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGuiSkin::cGuiSkin(const tString &a_sName, cGui *a_pGui)
	{
		m_pGui = a_pGui;
		m_sName = a_sName;

		m_vGfxElements.resize(eGuiSkinGfx_LastEnum, NULL);
		m_vFonts.resize(eGuiSkinFont_LastEnum, NULL);
		m_vAttributes.resize(eGuiSkinAttribute_LastEnum, NULL);
	}

	cGuiSkin::~cGuiSkin()
	{
		for (size_t i=0; i<m_vGfxElements.size(); i++)
			m_pGui->DestroyGfx(m_vGfxElements[i]);

		STLDeleteAll(m_vFonts);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	static eGuiGfxAnimationType ToAnimType(const char *a_pString)
	{
		if (a_pString==NULL) return eGuiGfxAnimationType_Loop;

		tString sLow = cString::ToLowerCase(a_pString);

		if (sLow=="loop") return eGuiGfxAnimationType_Loop;
		if (sLow=="oscillate") return eGuiGfxAnimationType_Oscillate;
		if (sLow=="random") return eGuiGfxAnimationType_Random;
		if (sLow=="stop_at_end") return eGuiGfxAnimationType_StopAtEnd;
		
		Warning("Animation type '%s' does not exist!\n", sLow.c_str());

		return eGuiGfxAnimationType_Loop;
	}

	static eGuiMaterial ToMaterial(const char *a_pString)
	{
		if (a_pString==NULL) return eGuiMaterial_Alpha;

		tString sLow = cString::ToLowerCase(a_pString);

		if (sLow=="alpha") return eGuiMaterial_Alpha;
		if (sLow=="diffuse") return eGuiMaterial_Diffuse;
		if (sLow=="font_normal") return eGuiMaterial_FontNormal;
		if (sLow=="additive") return eGuiMaterial_Additive;
		if (sLow=="modulative") return eGuiMaterial_Modulative;
		if (sLow=="inverse") return eGuiMaterial_Inverse;

		Warning("Material type '%s' does not exist!\n", sLow.c_str());

		return eGuiMaterial_Alpha;
	}

	bool cGuiSkin::LoadFromFile(const tString &a_sFile)
	{
		TiXmlDocument *pXmlDoc = efeNew(TiXmlDocument, (a_sFile.c_str()));
		if (pXmlDoc->LoadFile()==false)
		{
			Error("Couldn't load skin file '%s'!\n", a_sFile.c_str());
			efeDelete(pXmlDoc);
			return false;
		}

		// ATTRIBUTES
		TiXmlElement *pRootElem = pXmlDoc->RootElement();

		TiXmlElement *pAttributesElement = pRootElem->FirstChildElement("Attributes");

		TiXmlElement *pAttributeElem = pAttributesElement->FirstChildElement();
		for (; pAttributeElem != NULL; pAttributeElem = pAttributeElem->NextSiblingElement())
		{
			tString sType = cString::ToString(pAttributeElem->Attribute("type"),"");

			eGuiSkinAttribute type = m_pGui->GetSkinAttributeFromString(sType);
			if (type == eGuiSkinAttribute_LastEnum) continue;

			tString sValue = cString::ToString(pAttributeElem->Attribute("value"),"");

			cVector3f vVal(0);
			tFloatVec vValues;
			tString sSepp = " ";
			cString::GetFloatVec(sValue, vValues, &sSepp);

			if (vValues.size()>0) vVal.x = vValues[0];
			if (vValues.size()>1) vVal.y = vValues[1];
			if (vValues.size()>2) vVal.z = vValues[2];

			m_vAttributes[type] = vVal;
		}

		// FONTS
		TiXmlElement *pFontsElement = pRootElem->FirstChildElement("Fonts");

		TiXmlElement *pFontElem = pFontsElement->FirstChildElement();
		for (; pFontElem != NULL; pFontElem = pFontElem->NextSiblingElement())
		{
			tString sType = cString::ToString(pFontElem->Attribute("type"),"");

			eGuiSkinFont type = m_pGui->GetSkinFontFromString(sType);
			if (type == eGuiSkinFont_LastEnum) continue;

			tString sFontFile = cString::ToString(pFontElem->Attribute("file"),"");
			cVector2f vSize = cString::ToVector2f(pFontElem->Attribute("size"),1);
			cColor color = cString::ToColor(pFontElem->Attribute("color"),cColor(1,1));
			
			cGuiSkinFont *pFont = efeNew(cGuiSkinFont, (m_pGui));

			pFont->m_pFont = m_pGui->GetResources()->GetFontManager()->CreateFontData(sFontFile);
			pFont->m_vSize = vSize;
			pFont->m_Color = color;

			m_vFonts[type] = pFont;
		}

		// GFX ELEMENTS
		TiXmlElement *pGfxElementsElement = pRootElem->FirstChildElement("GfxElements");

		TiXmlElement *pGfxElem = pGfxElementsElement->FirstChildElement();
		for (; pGfxElem != NULL; pGfxElem = pGfxElem->NextSiblingElement())
		{
			tString sType = cString::ToString(pGfxElem->Attribute("type"),"");

			eGuiSkinGfx type = m_pGui->GetSkinGfxFromString(sType);
			if (type == eGuiSkinGfx_LastEnum) continue;

			tString sFile = cString::ToString(pGfxElem->Attribute("file"),"");
			cVector2f vOffset = cString::ToVector2f(pGfxElem->Attribute("offset"),0);
			cVector2f vSize = cString::ToVector2f(pGfxElem->Attribute("size"),-1);
			cColor color = cString::ToColor(pGfxElem->Attribute("color"),cColor(1,1));
			eGuiMaterial material = ToMaterial(pGfxElem->Attribute("material"));

			bool bAnimated = cString::ToBool(pGfxElem->Attribute("animated"),false);
			float fAnimFrameTime = cString::ToFloat(pGfxElem->Attribute("anim_frame_time"),1);
			eGuiGfxAnimationType animType = ToAnimType(pGfxElem->Attribute("anim_mode"));

			cGuiGfxElement *pGfx = NULL;
			if (sFile != "")
			{
				if (bAnimated)
					pGfx = m_pGui->CreateGfxImageBuffer(sFile, material, true, color);
				else
					pGfx = m_pGui->CreateGfxImage(sFile, material, color);
			}
			else
				pGfx = m_pGui->CreateGfxFilledRect(color, material);

			if (pGfx)
			{
				if (bAnimated)
				{
					cGuiGfxAnimation *pAnim = pGfx->GetAnimation(0);
					pAnim->SetFrameLength(fAnimFrameTime);
					pAnim->SetType(animType);
				}

				if (vSize.x >= 0)
					pGfx->SetActiveSize(vSize);
				pGfx->SetOffset(cVector3f(vOffset.x, vOffset.y, 0));

				m_vGfxElements[type] = pGfx;
			}
		}
		efeDelete(pXmlDoc);

		return true;
	}

	cGuiGfxElement *cGuiSkin::GetGfx(eGuiSkinGfx a_Type)
	{
		return m_vGfxElements[a_Type];
	}

	cGuiSkinFont *cGuiSkin::GetFont(eGuiSkinFont a_Type)
	{
		return m_vFonts[a_Type];
	}

	const cVector3f &cGuiSkin::GetAttribute(eGuiSkinAttribute a_Type)
	{
		return m_vAttributes[a_Type];
	}
}