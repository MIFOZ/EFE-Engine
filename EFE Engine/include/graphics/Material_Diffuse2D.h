#ifndef EFE_MATERIAL_DIFFUSE2D_H
#define EFE_MATERIAL_DIFFUSE2D_H

#include "graphics/Material.h"

namespace efe
{
	class cMaterial_Diffuse2D : public iMaterial
	{
	public:
		cMaterial_Diffuse2D(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics,
			cImageManager *a_pImageManager, cTextureManager *a_pTextureManger,
			cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
			eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D);
		~cMaterial_Diffuse2D();

		void Compile();
		bool StartRendering(eMaterialRenderType a_Type, iCamera *a_pCam, iLight *a_pLight);

		eMaterialType GetType(eMaterialRenderType a_Type);
	};

	class cMaterialType_Diffuse2D : public iMaterialType
	{
	public:
		bool IsCorrect(tString a_sName)
		{
			return cString::ToLowerCase(a_sName) == "diff2d";
		}

		iMaterial *Create(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics,
			cImageManager *a_pImageManager, cTextureManager *a_pTextureManager,
			cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
			eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D)
		{
			return efeNew(cMaterial_Diffuse2D, (a_sName, a_pLowLevelGraphics,
									a_pImageManager, a_pTextureManager, a_pRenderer, 
									a_pProgramManager, a_Picture, a_pRenderer3D));
		}
	};
};

#endif