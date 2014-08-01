#include "resources/MaterialManager.h"
#include "system/String.h"
#include "system/System.h"
#include "graphics/Graphics.h"
#include "resources/TextureManager.h"
#include "resources/Resources.h"
#include "graphics/Material.h"
#include "system/LowLevelSystem.h"
#include "graphics/MaterialHandler.h"
#include "impl/tinyXML/tinyxml.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMaterialManager::cMaterialManager(cGraphics *a_pGraphics, cResources *a_pResources)
		: iResourceManager(a_pResources->GetFileSearcher(), a_pResources->GetLowLevel(),
							a_pResources->GetLowLevelSystem())
	{
		m_pGraphics = a_pGraphics;
		m_pResources = a_pResources;

		m_lTextureSizeLevel = 0;

		m_fTextureAnisotropy = 1.0f;

		m_lIdCounter = 0;
	}

	cMaterialManager::~cMaterialManager()
	{
		DestroyAll();

		Log("Done with materials\n");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iMaterial *cMaterialManager::CreateMaterial(const tString &a_sName)
	{
		tString sPath;
		iMaterial *pMaterial;
		tString a_sNewName;

		BeginLoad(a_sName);

		a_sNewName = cString::SetFileExt(a_sName, "mat");

		pMaterial = static_cast<iMaterial*> (this->FindLoadedResource(a_sNewName, sPath));

		if(pMaterial == NULL && sPath != "")
		{
			pMaterial = LoadFromFile(a_sNewName, sPath);

			if (pMaterial == NULL)
			{
				Error("Couldn't load material '%s'\n", a_sNewName.c_str());
				EndLoad();
				return NULL;
			}
			AddResource(pMaterial);
		}
		if (pMaterial) pMaterial->IncUserCount();
		else Error("Couldn't create material '%s'\n", a_sNewName.c_str());

		EndLoad();
		return pMaterial;
	}

	//--------------------------------------------------------------

	iResourceBase *cMaterialManager::Create(const tString &a_sName)
	{
		return CreateMaterial(a_sName);
	}

	//--------------------------------------------------------------

	void cMaterialManager::Update(float a_fTimeStep)
	{
		tResourceHandleMapIt it = m_mapHandleResources.begin();
		for (;it != m_mapHandleResources.end(); ++it)
		{
			iResourceBase *pBase = it->second;
			iMaterial *pMap = static_cast<iMaterial*>(pBase);

			pMap->Update(a_fTimeStep);
		}
	}

	//--------------------------------------------------------------

	void cMaterialManager::Unload(iResourceBase *a_pResource)
	{
		
	}

	//--------------------------------------------------------------

	void cMaterialManager::Destroy(iResourceBase *a_pResource)
	{
		a_pResource->DecUserCount();

		if (a_pResource->HasUsers()==false)
		{
			RemoveResource(a_pResource);
			efeDelete(a_pResource);
		}
	}

	//--------------------------------------------------------------

	tString cMaterialManager::GetPhysicsMaterialName(const tString &a_sName)
	{
		tString sPath;
		iMaterial *pMaterial;
		tString sNewName;

		sNewName = cString::SetFileExt(a_sName, "mat");

		pMaterial = static_cast<iMaterial*>(FindLoadedResource(sNewName, sPath));

		if (pMaterial == NULL && sPath != "")
		{
			TiXmlDocument *pDoc = efeNew(TiXmlDocument, (sPath.c_str()));
			if (!pDoc->LoadFile())
				return "";

			TiXmlElement *pRoot = pDoc->RootElement();

			TiXmlElement *pMain = pRoot->FirstChildElement("Main");
			if (pMain == NULL)
			{
				efeDelete(pDoc);
				Error("Main child not found in '%s'\n", sPath.c_str());
				return "";
			}

			tString sPhysicsName = cString::ToString(pMain->Attribute("PhysicsMaterial"), "Default");

			efeDelete(pDoc);

			return sPhysicsName;
		}

		if (pMaterial)
			return pMaterial->GetPhysicsMaterial();
		else
			return "";
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iMaterial *cMaterialManager::LoadFromFile(const tString &a_sName, const tString &a_sPath)
	{
		TiXmlDocument *pDoc = efeNew(TiXmlDocument, (a_sPath.c_str()));
		if (!pDoc->LoadFile())
		{
			efeDelete(pDoc);
			return NULL;
		}

		TiXmlElement *pRoot = pDoc->RootElement();

		TiXmlElement *pMain = pRoot->FirstChildElement("Main");
		if (pMain==NULL)
		{
			efeDelete(pDoc);
			Error("Main child not found\n");
			return NULL;
		}

		const char *sType = pMain->Attribute("Type");
		if (sType==NULL)
		{
			efeDelete(pDoc);
			Error("Type not found\n");
			return NULL;
		}

		bool bUseAlpha = cString::ToBool(pMain->Attribute("UseAlpha"), false);
		bool bDepthTest = cString::ToBool(pMain->Attribute("DepthTest"), true);
		float fValue = cString::ToFloat(pMain->Attribute("Value"), 1);
		tString sPhysicsMatName = cString::ToString(pMain->Attribute("PhysicsMaterial"), "Default");

		iMaterial *pMat = m_pGraphics->GetMaterialHandler()->Create(a_sName, sType, eMaterialPicture_Texture);
		if (pMat==NULL)
		{
			efeDelete(pDoc);
			Error("Invalid material type '%s'\n", sType);
			return NULL;
		}

		pMat->SetHasAlpha(bUseAlpha);
		pMat->SetDepthTest(bDepthTest);
		pMat->SetValue(fValue);
		pMat->SetPhysicsMaterial(sPhysicsMatName);

		// Textures
		TiXmlElement *pTexRoot = pRoot->FirstChildElement("TextureUnits");
		if (pTexRoot==NULL)
		{
			efeDelete(pDoc);
			Error("TextureUnits child not found.\n");
			return NULL;
		}

		tTextureTypeList lstTexTypes = pMat->GetTextureTypes();
		tTextureTypeListIt it = lstTexTypes.begin();
		for (; it != lstTexTypes.end(); it++)
		{
			iTexture *pTex;

			TiXmlElement *pTexChild = pTexRoot->FirstChildElement(GetTextureString(it->m_Type).c_str());
			if (pTexChild==NULL)
				continue;

			eTextureTarget target = GetTarget(cString::ToString(pTexChild->Attribute("Type"),""));
			tString sFile = cString::ToString(pTexChild->Attribute("File"),"");
			bool bMipMaps = cString::ToBool(pTexChild->Attribute("MipMaps"),true);
			bool bCompress = cString::ToBool(pTexChild->Attribute("Compress"),false);
			eTextureAddressMode wrap = GetAddressMode(cString::ToString(pTexChild->Attribute("AddressMode"),""));

			eTextureAnimMode animMode = GetAnimMode(cString::ToString(pTexChild->Attribute("AnimMode"),"None"));
			float fFrameTime = cString::ToFloat(pTexChild->Attribute("AnimFrameTime"),1.0f);

			if (sFile=="")
				continue;

			if (animMode != eTextureAnimMode_None)
				pTex = m_pResources->GetTextureManager()->CreateAnim2D(sFile, bMipMaps, bCompress,
																		eTextureType_Normal,
																		m_lTextureSizeLevel);
			else
			{
				if (target == eTextureTarget_2D)
					pTex = m_pResources->GetTextureManager()->Create2D(sFile, bMipMaps, bCompress,
																		eTextureType_Normal,
																		m_lTextureSizeLevel);
			}
			
			pTex->SetFrameTime(fFrameTime);
			pTex->SetAnimMode(animMode);

			if (pTex == NULL)
			{
				efeDelete(pDoc);
				efeDelete(pTex);
				return NULL;
			}

			pMat->SetTexture(pTex, it->m_Type);
		}

		pMat->LoadData(pRoot);

		efeDelete(pDoc);

		pMat->Compile();

		return pMat;
	}

	//--------------------------------------------------------------

	eTextureTarget cMaterialManager::GetTarget(const tString &a_sType)
	{
		if (cString::ToLowerCase(a_sType) == "cube") return eTextureTarget_CubeMap;
		else if (cString::ToLowerCase(a_sType) == "1D") return eTextureTarget_1D;
		else if (cString::ToLowerCase(a_sType) == "2D") return eTextureTarget_2D;
		else if (cString::ToLowerCase(a_sType) == "3D") return eTextureTarget_3D;

		return eTextureTarget_2D;
	}

	//--------------------------------------------------------------

	tString cMaterialManager::GetTextureString(eMaterialTexture a_Type)
	{
		switch(a_Type)
		{
		case eMaterialTexture_Diffuse : return "Diffuse";
		case eMaterialTexture_Alpha : return "Alpha";
		case eMaterialTexture_NMap : return "NMap";
		case eMaterialTexture_Illumination : return "Illumination";
		case eMaterialTexture_Specular : return "Specular";
		case eMaterialTexture_CubeMap : return "CubeMap";
		case eMaterialTexture_Refraction : return "Refraction";
		}

		return "";
	}

	//--------------------------------------------------------------

	eTextureAddressMode cMaterialManager::GetAddressMode(const tString &a_sType)
	{
		if (cString::ToLowerCase(a_sType) == "wrap") return eTextureAddressMode_Wrap;
		else if (cString::ToLowerCase(a_sType) == "clamp") return eTextureAddressMode_Clamp;
		else if (cString::ToLowerCase(a_sType) == "border") return eTextureAddressMode_Border;

		return eTextureAddressMode_Wrap;
	}

	eTextureAnimMode cMaterialManager::GetAnimMode(const tString &a_sType)
	{
		if (cString::ToLowerCase(a_sType) == "none") return eTextureAnimMode_None;
		else if (cString::ToLowerCase(a_sType) == "loop") return eTextureAnimMode_Loop;
		else if (cString::ToLowerCase(a_sType) == "oscillate") return eTextureAnimMode_Oscillate;

		return eTextureAnimMode_None;
	}
}