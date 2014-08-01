#ifndef EFE_FRAMEBITMAP_H
#define EFE_FRAMEBITMAP_H

#include "resources/FrameBase.h"
#include "system/BinTree.h"
#include "math/MathTypes.h"

namespace efe
{
	class cFrameTexture;
	class cResourceImage;
	class iBitmap2D;

	class cFBitmapRect
	{
	public:
		cFBitmapRect(){m_lHandle =-1;}
		cFBitmapRect(int x, int y ,int w, int h, int a_lHandle)
		{
			m_Rect = cRectl(x,y,w,h);
			m_lHandle = a_lHandle;
		}

		cRectl m_Rect;
		int m_lHandle;
	};

	typedef BinTree<cFBitmapRect> tRectTree;
	typedef BinTreeNode<cFBitmapRect> tRectTreeNode;
	typedef std::list<tRectTreeNode*> tRectTreeNodeList;
	typedef tRectTreeNodeList::iterator tRectTreeNodeListIt;

	class cFrameBitmap : public iFrameBase
	{
	public:
		cFrameBitmap(iBitmap2D *a_pBitmap, cFrameTexture *a_pFrmTex, int a_lHandle);
		~cFrameBitmap();

		cResourceImage *AddBitmap(iBitmap2D *a_pSrc);
		bool MinimumFit(cRectl a_Src, cRectl a_Dest);
		bool IsFull();
		bool IsUpdated();

		bool IsLocked()const{return m_bIsLocked;}
		void SetLocked(bool a_bX){m_bIsLocked = a_bX;}

		bool FlushToTexture();

		cFrameTexture *GetFrameTexture(){return m_pFrameTexture;}

		int GetHanlde()const{return m_lHandle;}
	private:
		iBitmap2D *m_pBitmap;
		cFrameTexture *m_pFrameTexture;
		tRectTree m_Rects;
		int m_lMinHole;
		int m_lHandle;
		bool m_bIsFull;
		bool m_bIsUpdated;
		bool m_bIsLocked;
	};
};
#endif