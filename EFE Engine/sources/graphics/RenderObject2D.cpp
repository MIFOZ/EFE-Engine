#include "graphics/RenderObject2D.h"

#include "graphics/Material.h"

namespace efe
{
	cRenderObject2D::cRenderObject2D(iMaterial *a_pMaterial, tVertexVec *m_pVtxVec,tUIntVec *m_pIdxVec,
			ePrimitiveType a_Type, float a_fZ, cRectf &a_Rect, cMatrixf *a_pMtx, cVector3f *a_pTransform)
	{
		m_pMaterial = a_pMaterial;
		m_pVtxVec = m_pVtxVec;
		m_pIdxVec = m_pIdxVec;
		m_Type = a_Type;
		m_pMtx = a_pMtx;
		m_pTransform = a_pTransform;
		m_fZ = a_fZ;
		m_Rect = a_Rect;
		m_pCustomRenderer = NULL;
	}

	cRenderObject2D::cRenderObject2D(iMaterial *a_pMaterial, iRenderObject2DRenderer *a_pRednerer,
			ePrimitiveType a_Type, float a_fZ)
	{
		m_pMaterial = a_pMaterial;
		m_pVtxVec = NULL;
		m_pIdxVec = NULL;
		m_Type = a_Type;
		m_pMtx = NULL;
		m_pTransform = NULL;
		m_fZ = a_fZ;
		m_pCustomRenderer = a_pRednerer;
	}

	cRenderObject2D::~cRenderObject2D()
	{
	
	}
}