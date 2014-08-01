#include "graphics/GfxObject.h"

#include "graphics/Material.h"

namespace efe
{
	cGfxObject::cGfxObject(iMaterial *a_pMat, const tString &a_sFile, bool a_bIsImage)
	{
		m_pMat = a_pMat;

		m_sSourceFile = a_sFile;

		m_bIsImage = a_bIsImage;

		if (m_bIsImage)
			m_vVtx = a_pMat->GetImage(eMaterialTexture_Diffuse)->GetVertexVecCopy(0,-1);
		else
		{
			m_vVtx.push_back(cVertex(cVector3f(0,0,0), cVector3f(0,0,0), cColor(1,1)));
			m_vVtx.push_back(cVertex(cVector3f(1,0,0), cVector3f(1,0,0), cColor(1,1)));
			m_vVtx.push_back(cVertex(cVector3f(1,1,0), cVector3f(1,1,0), cColor(1,1)));
			m_vVtx.push_back(cVertex(cVector3f(0,1,0), cVector3f(0,1,0), cColor(1,1)));
		}
	}

	cGfxObject::~cGfxObject()
	{
		efeDelete(m_pMat);
	}
}