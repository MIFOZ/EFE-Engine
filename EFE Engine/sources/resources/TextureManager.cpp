#include "resources/TextureManager.h"
#include "system/String.h"
#include "graphics/Graphics.h"
#include "resources/Resources.h"
#include "graphics/Texture.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/LowLevelResources.h"
#include "system/LowLevelSystem.h"
#include "resources/FileSearcher.h"
#include "graphics/Bitmap2D.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cTextureManager::cTextureManager(cGraphics *a_pGraphics, cResources *a_pResources)
		: iResourceManager(a_pResources->GetFileSearcher(), a_pResources->GetLowLevel(),
							a_pResources->GetLowLevelSystem())
	{
		m_pGraphics = a_pGraphics;
		m_pResources = a_pResources;

		m_pLowLevelResources->GetSupportedImageFormats(m_lstFileFormats);

		m_vCubeSideSuffixes.push_back("_pos_x");
		m_vCubeSideSuffixes.push_back("_neg_x");
		m_vCubeSideSuffixes.push_back("_pos_y");
		m_vCubeSideSuffixes.push_back("_neg_y");
		m_vCubeSideSuffixes.push_back("_pos_z");
		m_vCubeSideSuffixes.push_back("_neg_z");
	}

	cTextureManager::~cTextureManager()
	{
		STLMapDeleteAll(m_mapAttenuationTextures);
		DestroyAll();
		Log(" Destroyed all textures\n");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iTexture *cTextureManager::Create1D(const tString &a_sName, bool a_bUseMipMaps, bool a_bCompress, eTextureType a_Type,
							unsigned int a_lTextureSizeLevel)
	{
		return CreateFlatTexture(a_sName, a_bUseMipMaps, a_bCompress, a_Type, eTextureTarget_1D, a_lTextureSizeLevel);
							
	}

	//--------------------------------------------------------------

	iTexture *cTextureManager::Create2D(const tString &a_sName, bool a_bUseMipMaps, bool a_bCompress, eTextureType a_Type,
							unsigned int a_lTextureSizeLevel, eTextureTarget a_Target)
	{
		return CreateFlatTexture(a_sName, a_bUseMipMaps, a_bCompress, a_Type, a_Target, a_lTextureSizeLevel);
	}

	iTexture *cTextureManager::CreateAnim2D(const tString &a_sName, bool a_bUseMipMaps, bool a_bCompress, eTextureType a_Type,
							unsigned int a_lTextureSizeLevel)
	{
		BeginLoad(a_sName);

		iTexture *pTexture = static_cast<iTexture*>(GetByName(a_sName));

		if (pTexture==NULL)
		{
			tString sFileExt = cString::GetFileExt(a_sName);
			tString sFileName = cString::SetFileExt(cString::GetFileName(a_sName),"");

			tStringVec m_vFileNames;

			tString sTest = sFileName + "01."+sFileExt;
			int lNum = 2;
			tStringVec vPaths;

			while(true)
			{
				tString sPath = m_pFileSearcher->GetFilePath(sTest);

				if (sPath == "")
					break;
				else
				{
					vPaths.push_back(sPath);
					if (lNum<10)
						sTest = sFileName + "0"+cString::ToString(lNum)+"."+sFileExt;
					else
						sTest = sFileName + cString::ToString(lNum)+"."+sFileExt;

					++lNum;
				}
			}

			if (vPaths.empty())
			{
				Error("No textures found for animation %s\n", a_sName.c_str());
				Error("Couldn't texture '%s'\n", a_sName.c_str());
				EndLoad();
				return NULL;
			}

			tBitmap2DVec vBitmaps;
			for (size_t i=0;i<vPaths.size();++i)
			{
				iBitmap2D *pBmp = m_pResources->GetLowLevel()->LoadBitmap2D(vPaths[i]);
				if (pBmp==NULL)
				{
					Error("Couldn't load bitmap '%s'!\n", vPaths[i].c_str());

					for (int j=0;j<(int)vBitmaps.size();j++) efeDelete(vBitmaps[i]);

					EndLoad();
					return NULL;
				}

				vBitmaps.push_back(pBmp);
			}

			pTexture = m_pGraphics->GetLowLevel()->CreateTexture(a_sName, a_bUseMipMaps, a_Type,
																eTextureTarget_2D);

			//pTexture->SetSizeLevel(a_lTextureSizeLevel);

			

		}

		EndLoad();
		return pTexture;
	}

	iTexture *cTextureManager::CreateCubeMap(const tString &a_sPathName, bool a_bUseMipMaps, eTextureType a_Type,
												unsigned int a_lArraySlices, bool m_bSRGB)
	{
		tString sName = cString::SetFileExt(a_sPathName,"");

		iTexture *pTexture = static_cast<iTexture*>(GetByName(sName));

		BeginLoad(a_sPathName);

		if (pTexture==NULL)
		{
			tStringVec vPaths;
			tString sPath = "";
			for (int i = 0; i < 6; i++)
			{
				for (tStringListIt it = m_lstFileFormats.begin(); it!=m_lstFileFormats.end();++it)
				{
					tString sNewName = sName + m_vCubeSideSuffixes[i] + "." + *it;
					sPath = m_pFileSearcher->GetFilePath(sNewName);

					if (sPath!="")
						break;
				}

				if (sPath=="")
				{
					tString sNewName = sName + m_vCubeSideSuffixes[i];
					Error("Couldn't find %d-face '%s', for cubemap '%s'\n", i, sNewName.c_str(), sName.c_str());
					return NULL;
				}

				vPaths.push_back(sPath);
			}

			tBitmap2DVec vBitmaps;
			for (int i=0; i<6; i++)
			{
				iBitmap2D *pBmp = m_pResources->GetLowLevel()->LoadBitmap2D(vPaths[i]);
				if (pBmp == NULL)
				{
					Error("Couldn't load bitmap '%s'!\n", vPaths[i].c_str());
					for (int j=0;j<(int)vBitmaps.size();j++) efeDelete(vBitmaps[j]);
					EndLoad();
					return NULL;
				}

				vBitmaps.push_back(pBmp);
			}

			pTexture = m_pGraphics->GetLowLevel()->CreateTexture(sName, a_bUseMipMaps, a_Type,
													eTextureTarget_CubeMap);

			if (pTexture->CreateCubeMapFromBitmapVec(&vBitmaps)==false)
			{
				Error("Couldn't create cubemap  '%s'\n", sName.c_str());
				efeDelete(pTexture);
				for (int j=0;j<(int)vBitmaps.size();j++) efeDelete(vBitmaps[j]);
				EndLoad();
				return NULL;
			}

			//Bitmaps no longer nedeed
			for (int j=0;j<(int)vBitmaps.size();j++) efeDelete(vBitmaps[j]);

			AddResource(pTexture);
		}

		if (pTexture)pTexture->IncUserCount();
		else ("Couldn't create texture '%s'\n", sName.c_str());

		EndLoad();
		return pTexture;
	}

	iResourceBase *cTextureManager::Create(const tString &a_sName)
	{
		return Create2D(a_sName, true);
	}

	void cTextureManager::Unload(iResourceBase *a_pResource)
	{
		
	}

	void cTextureManager::Destroy(iResourceBase *a_pResource)
	{
		a_pResource->DecUserCount();

		if (a_pResource->HasUsers()==false)
		{
			RemoveResource(a_pResource);
			efeDelete(a_pResource);
		}
	}

	void cTextureManager::Update(float a_fTimeStep)
	{
		tResourceHandleMapIt it = m_mapHandleResources.begin();
		for (; it != m_mapHandleResources.end(); ++it)
		{
			iResourceBase *pBase = it->second;
			iTexture *pTexture = static_cast<iTexture*> (pBase);

			//pTexture-
		}
	}

	iTexture *cTextureManager::CreateFlatTexture(const tString &a_sName, bool a_bUseMipMaps,
									bool a_bCompress, eTextureType a_Type, eTextureTarget a_Target,
									unsigned int a_lTextureSizeLevel)
	{
		tString sPath;
		iTexture *pTexture;

		BeginLoad(a_sName);

		pTexture = FindTexture2D(a_sName, sPath);

		if (pTexture==NULL && sPath!="")
		{
			iBitmap2D *pBmp;
			pBmp = m_pLowLevelResources->LoadBitmap2D(sPath);
			if (pBmp==NULL)
			{
				Error("Texturemanager Couldn't load bitmap '%s'\n", sPath.c_str());
				EndLoad();
				return NULL;
			}

			pTexture = m_pGraphics->GetLowLevel()->CreateTexture(a_sName, a_bUseMipMaps, a_Type,
																a_Target);
			pTexture->SetSizeLevel(a_lTextureSizeLevel);
			if (pTexture->CreateFromBitmap(pBmp)==false)
			{
				efeDelete(pTexture);
				efeDelete(pBmp);
				EndLoad();
				return NULL;
			}
			efeDelete(pBmp);

			AddResource(pTexture);
		}

		if (pTexture) pTexture->IncUserCount();
		else Error("Couldn't texture '%s'\n", a_sName.c_str());

		EndLoad();
		return pTexture;
	}

	iTexture *cTextureManager::FindTexture2D(const tString &a_sName, tString &a_sFilePath)
	{
		iTexture *pTexture = NULL;

		if (cString::GetFileExt(a_sName) == "")
		{
			for (tStringListIt it = m_lstFileFormats.begin(); it != m_lstFileFormats.end(); ++it)
			{
				tString sNewName = cString::SetFileExt(a_sName, *it);
				pTexture = static_cast<iTexture*> (FindLoadedResource(sNewName, a_sFilePath));

				if ((pTexture==NULL && a_sFilePath != "") || pTexture!=NULL) break;
			}
		}
		else
			pTexture = static_cast<iTexture*> (FindLoadedResource(a_sName, a_sFilePath));

		return pTexture;
	}
}