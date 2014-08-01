#include "resources/FrameBitmap.h"
#include "math/Math.h"
#include "system/LowLevelSystem.h"
#include "resources/FrameTexture.h"
#include "resources/ResourceImage.h"
#include "graphics/Bitmap2D.h"
#include "graphics/Texture.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cFrameBitmap::cFrameBitmap(iBitmap2D *a_pBitmap, cFrameTexture *a_pFrmTex, int a_lHandle)
	{
		m_pBitmap = a_pBitmap;
		m_pFrameTexture = a_pFrmTex;
		m_pBitmap->FillRect(cRectl(0,0,0,0),cColor(1,1));
		m_lMinHole = 6;
		m_lHandle = a_lHandle;
		m_bIsFull = false;
		m_bIsLocked = false;

		m_Rects.Insert(cFBitmapRect(0,0,m_pBitmap->GetWidth(),m_pBitmap->GetHeight(),-1));
	}

	cFrameBitmap::~cFrameBitmap()
	{
		efeDelete(m_pBitmap);
		m_pBitmap = NULL;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	#define DEBUG_BTREE (false)

	cResourceImage *cFrameBitmap::AddBitmap(iBitmap2D *a_pSrc)
	{
		cResourceImage *pImage=NULL;

		int lSW = a_pSrc->GetWidth()+2;
		int lSH = a_pSrc->GetHeight()+2;

		int lDW = m_pBitmap->GetWidth();
		int lDH = m_pBitmap->GetHeight();

		cVector2l vPos;

		bool bFoundEmptyNode = false;
		bool bFoundNode = false;
		int node=0;

		if (DEBUG_BTREE)Log("**** Image %d ****\n", m_lPicCount);

		tRectTreeNodeList lstNodes = m_Rects.GetLeafList();
		tRectTreeNodeListIt it;
		for (it=lstNodes.begin();it!=lstNodes.end();++it)
		{
			if (DEBUG_BTREE)Log("Checking node %d:\n", node++);
			tRectTreeNode *TopNode = *it;
			cFBitmapRect *pData = TopNode->GetData();

			if (pData->m_lHandle<0)
			{
				if (DEBUG_BTREE)Log("Found tree node\n");
				bFoundEmptyNode = true;

				cRectl NewRect = cRectl(pData->m_Rect.x,pData->m_Rect.y,lSW,lSH);
				if (DEBUG_BTREE)Log("Fit: [%d:%d:%d:%d] in [%d:%d:%d:%d]\n",
					NewRect.x, NewRect.y, NewRect.w, NewRect.h,
					pData->m_Rect.x, pData->m_Rect.y, pData->m_Rect.w, pData->m_Rect.h); 

				if (cMath::BoxFit(NewRect,pData->m_Rect))
				{
					if (DEBUG_BTREE)Log("The node fits!\n");
					bFoundNode = true;

					if (MinimumFit(NewRect,pData->m_Rect))
					{
						if (DEBUG_BTREE)Log("Minimum fit!\n");
						pData->m_Rect = NewRect;
						pData->m_lHandle = 1;
					}
					else
					{
						if (DEBUG_BTREE)Log("Normal fit!\n");
						tRectTreeNode *UpperNode;
						UpperNode = m_Rects.InsertAt(cFBitmapRect(NewRect.x,NewRect.y,
													pData->m_Rect.w,NewRect.h,-2),TopNode,
													eBinTreeNode_Left);

						m_Rects.InsertAt(cFBitmapRect(NewRect.x,NewRect.y+NewRect.h,
										pData->m_Rect.w,pData->m_Rect.h-NewRect.h,-3),TopNode,
										eBinTreeNode_Right);

						pData = UpperNode->GetData();

						m_Rects.InsertAt(cFBitmapRect(NewRect.x,NewRect.y,
										NewRect.w,NewRect.h,2),UpperNode,
										eBinTreeNode_Left);

						m_Rects.InsertAt(cFBitmapRect(NewRect.x+NewRect.w,NewRect.y,
										pData->m_Rect.w-NewRect.w,NewRect.h,-4),UpperNode,
										eBinTreeNode_Right);
					}

					vPos = cVector2l(NewRect.x+1,NewRect.y+1);

					for (int i=0;i<2;i++) for (int j=0;j<2;j++)
					{
						a_pSrc->DrawToBitmap(m_pBitmap, cVector2l(NewRect.x+i*2, NewRect.y+j*2));
					}

					for (int i=0;i<2;i++)
					{
						for (int j=0;j<2;j++)
							if ((i==0 || j==0) && (i!=j))
								a_pSrc->DrawToBitmap(m_pBitmap,cVector2l(NewRect.x+1+i, NewRect.y+1+j));
					}
					a_pSrc->DrawToBitmap(m_pBitmap,cVector2l(NewRect.x+1,NewRect.y+1));

					m_lPicCount++;
					m_pFrameTexture->SetPicCount(m_lPicCount);
					break;
				}
			}
		}

		if (bFoundNode)
		{
			pImage = efeNew(cResourceImage, (a_pSrc->GetFileName(),m_pFrameTexture,this,
				cRectl(vPos,cVector2l(lSW-2,lSH-2)),
				cVector2l(m_pBitmap->GetWidth(),m_pBitmap->GetHeight()),
				m_lHandle));

			if (!bFoundEmptyNode)
				m_bIsFull = true;

			m_bIsUpdated = true;
		}

		if (DEBUG_BTREE)
		{
			Log("Current Tree begin:\n");
			tRectTreeNodeList lstNodes = m_Rects.GetNodeList();
			tRectTreeNodeListIt it;
			int node=0;
			for (it=lstNodes.begin();it!=lstNodes.end();++it)
			{
				cRectl Rect = (*it)->GetData()->m_Rect;
				int h = (*it)->GetData()->m_lHandle;
				Log(" %d: [%d:%d:%d:%d]:%d\n",node,Rect.x,Rect.y,Rect.w,Rect.h,h);
				node++;
			}
			Log("Current Tree end:\n");
			Log("-----------------\n");

			Log("Current Leaves begin:\n");
			lstNodes = m_Rects.GetLeafList();
			node=0;
			for(it=lstNodes.begin();it!=lstNodes.end();++it);
			{
				cRectl Rect = (*it)->GetData()->m_Rect;
				int h = (*it)->GetData()->m_lHandle;
				Log(" %d: [%d:%d:%d:%d]:%d\n",node,Rect.x,Rect.y,Rect.w,Rect.h,h);
				node++;
			}
			Log("Current Tree end:\n");
			Log("-----------------\n");
		}

		return pImage;
	}

	//--------------------------------------------------------------

	bool cFrameBitmap::MinimumFit(cRectl a_Src, cRectl a_Dest)
	{
		if (a_Dest.w-a_Src.w<m_lMinHole && a_Dest.h-a_Src.h<m_lMinHole) return true;

		return false;
	}

	//--------------------------------------------------------------

	bool cFrameBitmap::IsUpdated()
	{
		return m_bIsUpdated;
	}

	//--------------------------------------------------------------

	bool cFrameBitmap::IsFull()
	{
		return m_bIsFull;
	}

	//--------------------------------------------------------------

	bool cFrameBitmap::FlushToTexture()
	{
		if (m_bIsUpdated)
		{
			m_pFrameTexture->GetTexture()->CreateFromBitmap(m_pBitmap);
			//m_pFrameTexture->GetTexture()->SetWrapS(eTextureWrap_ClampToEdge);
			//m_pFrameTexture->GetTexture()->SetWrapT(eTextureWrap_ClampToEdge);

			m_bIsUpdated = false;
			return true;
		}
		else
			return false;
	}
}