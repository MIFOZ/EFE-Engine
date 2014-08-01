#ifndef EFE_IMAGEMANAGER_H
#define EFE_IMAGEMANAGER_H

#include "resources/ResourceManager.h"
#include "math/MathTypes.h"

namespace efe
{
	class cResourceImage;
	class cFrameTexture;
	class cFrameBitmap;
	class iLowLevelGraphics;
	class iBitmap2D;

	typedef std::list<cFrameBitmap*> tFrameBitmapList;
	typedef tFrameBitmapList::iterator tFrameBitmapListIt;

	typedef std::map<int,cFrameTexture*> tFrameTextureMap;
	typedef tFrameTextureMap::iterator tFrameTextureMapIt;

	class cImageManager : public iResourceManager
	{
	public:
		cImageManager(cFileSearcher *a_pFileSearcher, iLowLevelGraphics *a_pLowLevelGraphics,
			iLowLevelResources *a_pLowLevelResources, iLowLevelSystem *a_pLowLevelSystem);
		~cImageManager();

		iResourceBase *Create(const tString &a_sName);

		void Destroy(iResourceBase *a_pResource);

		void Unload(iResourceBase *a_pResource);

		iResourceBase *CreateInFrame(const tString &a_sName, int a_lFrameHandle);
		cResourceImage *CreateImage(const tString &a_sName, int a_lFrameHandle = -1);

		int FlushAll();
		void DeleteAllBitmapFrames();

		cResourceImage *CreateFromBitmap(const tString &a_sName, iBitmap2D *a_pBmp, int a_lFrameHandle = -1);

		int CreateFrame(cVector2l a_vSize);
		void SetFrameLocked(int a_lHandle, bool a_bLocked);
	private:
		iLowLevelGraphics *m_pLowLevelGraphics;

		tFrameBitmapList m_lstBitmapFrames;
		tFrameTextureMap m_mapTextureFrames;

		tStringList m_lstFileFormats;
		cVector2l m_vFrameSize;
		int m_lFrameHandle;

		cResourceImage *FindImage(const tString &a_sName, tString &a_sFilePath);
		cResourceImage *AddToFrame(iBitmap2D *a_pBmp, int a_lFrameHandle);
		cFrameBitmap *CreateBitmapFrame(cVector2l a_vSize);
	};
};
#endif