#ifndef EFE_RENDEROBJECT2D_H
#define EFE_RENDEROBJECT2D_H

#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"
#include "Graphics/Material.h"

namespace efe
{
	class iRenderObject2DRenderer
	{
	public:
		virtual void RenderToBatch(eMaterialRenderType a_RenderType,
									unsigned int &a_lIdxAdd)=0;
	};

	class cRenderObject2D
	{
	public:
		cRenderObject2D(iMaterial *a_pMaterial, tVertexVec *m_pVtxVec,tUIntVec *m_pIdxVec,
			ePrimitiveType a_Type, float a_fZ, cRectf &a_Rect, cMatrixf *a_pMtx=NULL, cVector3f *a_pTransform=NULL);

		cRenderObject2D(iMaterial *a_pMaterial, iRenderObject2DRenderer *a_pRednerer,
			ePrimitiveType a_Type, float a_fZ);

		~cRenderObject2D();

		iMaterial *GetMaterial()const{return m_pMaterial;}
		tVertexVec *GetVertexVec()const{return m_pVtxVec;}
		tUIntVec *GetIndexVec()const{return m_pIdxVec;}
		ePrimitiveType GetType()const{return m_Type;}
		cVector3f *GetTransform()const{return m_pTransform;}
		float GetZ()const{return m_fZ;}
		const cRectf &GetRect()const{return m_Rect;}
		iRenderObject2DRenderer *GetCustomRenderer()const{return m_pCustomRenderer;}
	private:
		iMaterial *m_pMaterial;
		tVertexVec *m_pVtxVec;
		tUIntVec *m_pIdxVec;
		ePrimitiveType m_Type;
		cMatrixf *m_pMtx;
		cVector3f *m_pTransform;
		float m_fZ;
		cRectf m_Rect;
		iRenderObject2DRenderer *m_pCustomRenderer;
	};
};
#endif