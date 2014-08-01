#ifndef EFE_MATERIALHANLDER_H
#define EFE_MATERIALHANDLER_H

#include "graphics/Material.h"

namespace efe
{
	class cGraphics;
	class cResources;

	class cMaterialHandler
	{
	public:
		cMaterialHandler(cGraphics *a_pGraphics, cResources *a_pResources);
		~cMaterialHandler();

		void Add(iMaterialType *a_pTypedata);

		iMaterial *Create(tString a_sMatName, eMaterialPicture m_PicType);

		iMaterial *Create(const tString &a_sName, tString a_sMatName, eMaterialPicture m_PicType);
	private:
		tMaterialTypeList m_lstMatTypes;
		cResources *m_pResources;
		cGraphics *m_pGraphics;
	};
};

#endif