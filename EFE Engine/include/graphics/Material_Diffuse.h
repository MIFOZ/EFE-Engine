#ifndef EFE_MATERIAL_DIFFUSE_H
#define EFE_MATERIAL_DIFFUSE_H

#include "graphics/Material_BaseLight.h"

namespace efe
{
	class cMaterial_Diffuse : public iMaterial_BaseLight
	{
	public:
		cMaterial_Diffuse(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics,
			cImageManager *a_pImageManager, cTextureManager *a_pTextureManager,
			cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
			eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D);
		~cMaterial_Diffuse();
	};

	class cMaterialType_Diffuse : public iMaterialType
	{
	public:
		cMaterialType_Diffuse();

		bool IsCorrect(tString a_sName)
		{
			return cString::ToLowerCase(a_sName)=="diffuse";
		}

		iMaterial *Create(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics,
			cImageManager *a_pImageManager, cTextureManager *a_pTextureManager,
			cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
			eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D);

	private:
		int m_lTechLevel;
	};
};
#endif