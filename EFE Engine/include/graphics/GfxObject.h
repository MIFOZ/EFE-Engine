#ifndef EFE_GFX_OBJECT_H
#define EFE_GFX_OBJECT_H

#include <vector>
#include "graphics/GraphicsTypes.h"

namespace efe
{
	class iMaterial;

	class cGfxObject
	{
	public:
		cGfxObject(iMaterial *a_pMat, const tString &a_sFile, bool a_bIsImage);
		~cGfxObject();

		iMaterial *GetMaterial()const{return m_pMat;}
		cVertex *GetVtxPtr(int a_lNum){return &m_vVtx[a_lNum];}
		tVertexVec *GetVertexVec(){return &m_vVtx;}

		const tString &GetResourceFile(){return m_sSourceFile;}

		bool IsImage(){return m_bIsImage;}
	private:
		tVertexVec m_vVtx;
		iMaterial *m_pMat;
		float m_fZ;
		bool m_bIsImage;

		tString m_sSourceFile;
	};

	typedef std::vector<cGfxObject> tGfxObjectVec;
	typedef tGfxObjectVec::iterator tGfxObjectVecIt;
};
#endif