#include "resources/ResourceImage.h"
#include "resources/FrameTexture.h"
#include "graphics/Texture.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	#define kContrastSize (0.001f)

	cResourceImage::cResourceImage(tString a_sName, cFrameTexture *a_pFrameTex,
			cFrameBitmap *a_pFrameBmp, cRectl a_Rect,
			cVector2l a_vSrcSize, int a_lHandle) : iResourceBase(a_sName,0)
	{
		m_pFrameTexture = a_pFrameTex;
		m_pFrameBitmap = a_pFrameBmp;
		m_Rect = a_Rect;
		m_vSourceSize = a_vSrcSize;
		m_lHandle = a_lHandle;

		cVector2f vTexSize = cVector2f((float)m_Rect.w,(float)m_Rect.h)/
								cVector2f((float)m_vSourceSize.x,(float)m_vSourceSize.y);
		cVector2f vTexPos = cVector2f((float)m_Rect.x,(float)m_Rect.y)/
								cVector2f((float)m_vSourceSize.x,(float)m_vSourceSize.y);

		m_vVtx.push_back(cVertex(cVector3f(0,0,0),
							cVector3f(vTexPos.x+kContrastSize, vTexPos.y+vTexSize.y-kContrastSize,0),
							cColor(1)));

		m_vVtx.push_back(cVertex(cVector3f((float)m_Rect.w,0,0),
							cVector3f(vTexPos.x+kContrastSize,vTexPos.y+kContrastSize,0),
							cColor(1)));

		m_vVtx.push_back(cVertex(cVector3f((float)m_Rect.w,(float)m_Rect.h,0),
							cVector3f(vTexPos.x+vTexSize.x-kContrastSize,vTexPos.y+kContrastSize,0),
							cColor(1)));

		m_vVtx.push_back(cVertex(cVector3f(0,(float)m_Rect.h,0),
							cVector3f(vTexPos.x+vTexSize.x-kContrastSize,vTexPos.y+vTexSize.y-kContrastSize,0),
							cColor(1)));
	}

	cResourceImage::~cResourceImage()
	{
		m_vVtx.clear();
		m_pFrameTexture = NULL;
		m_lHandle = -1;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iTexture *cResourceImage::GetTexture()const{return m_pFrameTexture->GetTexture();}

	//--------------------------------------------------------------

	tVertexVec cResourceImage::GetVertexVecCopy(const cVector2f &a_vPos, const cVector2f &a_vSize)
	{
		tVertexVec vTmpVtx = m_vVtx;

		if (a_vSize == cVector2f(-1,-1))
		{
			vTmpVtx[1].pos.x = m_vVtx[0].pos.x + m_Rect.w;
			vTmpVtx[2].pos.x = m_vVtx[0].pos.x + m_Rect.w;
			vTmpVtx[2].pos.y = m_vVtx[0].pos.y + m_Rect.h;
			vTmpVtx[3].pos.y = m_vVtx[0].pos.y + m_Rect.h;
		}
		else
		{
			vTmpVtx[1].pos.x = m_vVtx[0].pos.x + a_vSize.x;
			vTmpVtx[2].pos.x = m_vVtx[0].pos.x + a_vSize.x;
			vTmpVtx[2].pos.y = m_vVtx[0].pos.y + a_vSize.y;
			vTmpVtx[3].pos.y = m_vVtx[0].pos.y + a_vSize.y;
		}

		for (int i=0;i<4;i++)
			vTmpVtx[i].pos+=a_vPos;

		return vTmpVtx;
	}

	//--------------------------------------------------------------

	bool cResourceImage::Reload()
	{
		return false;
	}

	//--------------------------------------------------------------

	void cResourceImage::Unload()
	{
	}

	//--------------------------------------------------------------

	void cResourceImage::Destroy()
	{
	}

	//--------------------------------------------------------------
}