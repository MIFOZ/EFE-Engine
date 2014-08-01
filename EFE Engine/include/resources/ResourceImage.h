#ifndef EFE_RESOURCE_IMAGE_H
#define EFE_RESOURCE_IMAGE_H

#include <vector>

#include "resources/ResourceBase.h"

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"

namespace efe
{
	class cFrameTexture;
	class cFrameBitmap;
	class iTexture;

	class cResourceImage : public iResourceBase
	{
		friend class cImageManager;
	public:
		cResourceImage(tString a_sName, cFrameTexture *a_pFrameTex,
						cFrameBitmap *a_pFrameBmp,
						cRectl a_Rect,
						cVector2l a_vSrcSize, int a_lHandle);

		bool Reload();
		void Unload();
		void Destroy();

		int GetHeight()const{return m_Rect.h;}
		int GetWidth()const{return m_Rect.w;}
		cVector2l GetSize()const
		{
			return cVector2l(m_Rect.w,m_Rect.h);
		}
		cVector2l GetPosition()const
		{
			return cVector2l(m_Rect.x,m_Rect.y);
		}

		int GetSourceWidth()const{return m_vSourceSize.x;}
		int GetSourceHeight()const{return m_vSourceSize.y;}

		iTexture *GetTexture()const;

		cFrameTexture *GetFrameTexture()const{return m_pFrameTexture;}
		cFrameBitmap *GetFrameBitmap()const{return m_pFrameBitmap;}

		tVertexVec GetVertexVecCopy(const cVector2f &a_vPos, const cVector2f &a_vSize);
		const tVertexVec &GetVertexVec(){return m_vVtx;}
	private:
		~cResourceImage();

		cFrameTexture *m_pFrameTexture;
		cFrameBitmap *m_pFrameBitmap;
		
		cVector2l m_vSourceSize;
		cRectl m_Rect;
		tVertexVec m_vVtx;

		int m_lHandle;
	};

	typedef std::vector<cResourceImage*> tResourceImageVec;
	typedef tResourceImageVec::iterator tResourceImageVecIt;
};
#endif