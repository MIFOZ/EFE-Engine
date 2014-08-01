#include "resources/ImageManager.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/ResourceImage.h"
#include "resources/FrameBitmap.h"
#include "resources/FrameTexture.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/LowLevelResources.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cImageManager::cImageManager(cFileSearcher *a_pFileSearcher, iLowLevelGraphics *a_pLowLevelGraphics,
			iLowLevelResources *a_pLowLevelResources, iLowLevelSystem *a_pLowLevelSystem)
			: iResourceManager(a_pFileSearcher, a_pLowLevelResources, a_pLowLevelSystem)
	{
		m_pLowLevelGraphics = a_pLowLevelGraphics;

		m_pLowLevelResources->GetSupportedImageFormats(m_lstFileFormats);

		m_vFrameSize = cVector2l(512);
		m_lFrameHandle = 0;
	}

	cImageManager::~cImageManager()
	{
		DestroyAll();

		Log(" Done with images\n");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iResourceBase *cImageManager::Create(const tString &a_sName)
	{
		return CreateInFrame(a_sName, -1);
	}

	//--------------------------------------------------------------

	iResourceBase *cImageManager::CreateInFrame(const tString &a_sName, int a_lFrameHandle)
	{
		cResourceImage *pImage = NULL;
		tString sPath;

		BeginLoad(a_sName);

		pImage = FindImage(a_sName, sPath);
		if (!pImage)
		{
			if (sPath != "")
			{
				iBitmap2D *pBmp;
				pBmp = m_pLowLevelResources->LoadBitmap2D(sPath);
				if (pBmp==NULL)
				{
					Error("Imagemanager couldn't load bitmap '%s'\n", sPath.c_str());
					EndLoad();
					return NULL;
				}

				pImage = AddToFrame(pBmp, a_lFrameHandle);

				efeDelete(pBmp);

				if (pImage==NULL)
					Error("Imagemanager couldn't create image '%s'\n", a_sName.c_str());

				if (pImage) AddResource(pImage);
			}
		}
		//else
		//{}

		if (pImage) pImage->IncUserCount();
		else Error("Couldn't load image '%s'\n", a_sName.c_str());

		EndLoad();
		return pImage;
	}

	//--------------------------------------------------------------

	cResourceImage *cImageManager::CreateImage(const tString &a_sName, int a_lFrameHandle)
	{
		iResourceBase *pRes = CreateInFrame(a_sName, a_lFrameHandle);
		cResourceImage *pImage = static_cast<cResourceImage*> (pRes);

		return pImage;
	}

	//--------------------------------------------------------------

	cResourceImage *cImageManager::CreateFromBitmap(const tString &a_sName, iBitmap2D *a_pBmp, int a_lFrameHandle)
	{
		if (a_pBmp==NULL) return NULL;

		cResourceImage *pImage = AddToFrame(a_pBmp, a_lFrameHandle);

		if (pImage)
		{
			AddResource(pImage, false);
			pImage->IncUserCount();
		}
		return pImage;
	}

	//--------------------------------------------------------------

	void cImageManager::Unload(iResourceBase *a_pResource)
	{
	}

	//--------------------------------------------------------------

	void cImageManager::Destroy(iResourceBase *a_pResource)
	{
		cResourceImage *pImage = static_cast<cResourceImage*> (a_pResource);
		cFrameTexture *pFrame = pImage->GetFrameTexture();
		cFrameBitmap *pBmpFrame = pImage->GetFrameBitmap();

		pImage->DecUserCount();

		if (pImage->HasUsers()==false)
		{
			pFrame->DecPicCount();
			pBmpFrame->DecPicCount();
			RemoveResource(a_pResource);
			efeDelete(a_pResource);
		}

		if (pFrame->IsEmpty())
		{
			for (tFrameBitmapListIt it=m_lstBitmapFrames.begin(); it!=m_lstBitmapFrames.end();++it)
			{
				cFrameBitmap *pBmpFrame = *it;
				if (pBmpFrame->GetFrameTexture() == pFrame)
				{
					efeDelete(pBmpFrame);
					m_lstBitmapFrames.erase(it);
					break;
				}
			}

			m_mapTextureFrames.erase(pFrame->GetHandle());
			efeDelete(pFrame);
		}
	}

	//--------------------------------------------------------------

	void cImageManager::DeleteAllBitmapFrames()
	{
		FlushAll();
		for (tFrameBitmapListIt it=m_lstBitmapFrames.begin();it!=m_lstBitmapFrames.end();++it)
		{
			efeDelete(*it);
			it = m_lstBitmapFrames.erase(it);
		}
	}

	//--------------------------------------------------------------

	int cImageManager::FlushAll()
	{
		int lNum=0;
		for (tFrameBitmapListIt it=m_lstBitmapFrames.begin();it!=m_lstBitmapFrames.end();++it)
		{
			if ((*it)->FlushToTexture()) lNum++;

			if ((*it)->IsFull())
				it++;

			else
				it++;
		}
		return lNum;
	}

	//--------------------------------------------------------------

	int cImageManager::CreateFrame(cVector2l a_vSize)
	{
		cFrameBitmap *pBFrame = CreateBitmapFrame(a_vSize);

		if (pBFrame==NULL) return -1;

		return pBFrame->GetHanlde();
	}

	//--------------------------------------------------------------

	void cImageManager::SetFrameLocked(int a_lHandle, bool a_bLocked)
	{
		tFrameBitmapListIt it=m_lstBitmapFrames.begin();
		while (it!=m_lstBitmapFrames.end())
		{
			if ((*it)->GetHanlde()==a_lHandle)
			{
				(*it)->SetLocked(a_bLocked);
				break;
			}
			it++;
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cResourceImage *cImageManager::FindImage(const tString &a_sName, tString &a_sFilePath)
	{
		cResourceImage *pImage = NULL;

		if (cString::GetFileExt(a_sName)=="")
		{
			for (tStringListIt it = m_lstFileFormats.begin();it!=m_lstFileFormats.end();++it)
			{
				tString sNewName = cString::SetFileExt(a_sName, *it);
				pImage = static_cast<cResourceImage*> (FindLoadedResource(sNewName,a_sFilePath));

				if ((pImage==NULL && a_sFilePath!="") || pImage!=NULL) break;
			}
		}
		else
			pImage = static_cast<cResourceImage*> (FindLoadedResource(a_sName,a_sFilePath));

		return pImage;
	}

	//--------------------------------------------------------------

	cResourceImage *cImageManager::AddToFrame(iBitmap2D *a_pBmp, int a_lFrameHandle)
	{
		bool bFound = false;
		cResourceImage *pImage = NULL;

		if (m_lstBitmapFrames.size()==0)
			CreateBitmapFrame(m_vFrameSize);

		if (a_lFrameHandle<0)
		{
			for (tFrameBitmapListIt it=m_lstBitmapFrames.begin();it!=m_lstBitmapFrames.end();++it)
			{
				if (!(*it)->IsFull() && !(*it)->IsLocked())
				{
					pImage = (*it)->AddBitmap(a_pBmp);
					if (pImage!=NULL)
					{
						bFound = true;
						break;
					}
				}
			}

			if (!bFound)
			{
				cFrameBitmap *pFrame = CreateBitmapFrame(m_vFrameSize);
				if (pFrame)
				{
					pImage = pFrame->AddBitmap(a_pBmp);
					if (pImage==NULL)
						Log("No fit in new frame!\n");
				}
			}
		}
		else
		{
			tFrameBitmapListIt it = m_lstBitmapFrames.begin();
			while (it!=m_lstBitmapFrames.end())
			{
				if ((*it)->GetHanlde() == a_lFrameHandle)
				{
					pImage = (*it)->AddBitmap(a_pBmp);
					break;
				}
				it++;
			}
			if (pImage==NULL)
				Error("Image didn't fit frame %d!\n", a_lFrameHandle);
		}
		return pImage;
	}

	//--------------------------------------------------------------

	cFrameBitmap *cImageManager::CreateBitmapFrame(cVector2l a_vSize)
	{
		iTexture *pTex = m_pLowLevelGraphics->CreateTexture(false, eTextureType_Normal, eTextureTarget_2D);
		cFrameTexture *pTFrame = efeNew(cFrameTexture, (pTex, m_lFrameHandle));
		iBitmap2D *pBmp = m_pLowLevelGraphics->CreateBitmap2D(a_vSize, eFormat_RGBA8);
		cFrameBitmap *pBFrame = efeNew(cFrameBitmap, (pBmp, pTFrame, m_lFrameHandle));

		m_lstBitmapFrames.push_back(pBFrame);

		std::pair<tFrameTextureMap::iterator, bool> ret=m_mapTextureFrames.insert(tFrameTextureMap::value_type(m_lFrameHandle, pTFrame));
		if (ret.second == false)
			Error("Could not add texture frame %d with handle %d! Handle already exists!\n", pTFrame, m_lFrameHandle);
		else
			Log("Added texture frame: %d\n", pTFrame);

		m_lFrameHandle++;
		return pBFrame;
	}
}