#ifndef EFE_MATERIAL_FONTNORMAL_H
#define EFE_MATERIAL_FONTNORMAL_H

#include "graphics\Material.h"

namespace efe
{
	class cMaterial_FontNormal : public iMaterial
	{
	public:
		cMaterial_FontNormal(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics,
			cImageManager *a_pImageManager, cTextureManager *a_pTextureManger,
			cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
			eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D);
		~cMaterial_FontNormal();

		void Compile();
		bool StartRendering(eMaterialRenderType a_Type, iCamera *a_pCam, iLight *a_pLight);

		eMaterialType GetType(eMaterialRenderType a_Type);

	private:
		iBlendState *m_pBS;
	};

	class cMaterialType_FontNormal : public iMaterialType
	{
	public:
		bool IsCorrect(tString a_sName)
		{
			return cString::ToLowerCase(a_sName)=="fontnormal";
		}

		iMaterial *Create(const tString &a_sName,  iLowLevelGraphics *a_pLowLevelGraphics,
			cImageManager *a_pImageManager, cTextureManager *a_pTextureManger,
			cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
			eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D)
		{
			return efeNew(cMaterial_FontNormal, (a_sName,a_pLowLevelGraphics,
				a_pImageManager,a_pTextureManger,a_pRenderer,
				a_pProgramManager,a_Picture,a_pRenderer3D));
		}

	};
};

#endif