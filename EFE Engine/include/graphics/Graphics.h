#ifndef EFE_GRAPHICS_H
#define EFE_GRAPHICS_H

#include "system/SystemTypes.h"

namespace efe
{
	class cResources;
	class cRenderer2D;
	class cRenderer3D;
	class cRendererPostEffects;
	class cRenderList;
	class cGraphicsDrawer;
	class iLowLevelResources;
	class iLowLevelGraphics;
	class cMeshCreator;
	class cMaterialHandler;

	class cGraphics
	{
	public:
		cGraphics(iLowLevelGraphics *a_pLowLevelGraphics, iLowLevelResources *a_pLowLevelResources);
		~cGraphics();

		bool Init(int a_iWidth, int a_iHeight, int a_iBpp, bool a_bFullscreen, int a_lMultiSampling,
			const tString &a_sWindowCaption, cResources *a_pResources);

		iLowLevelGraphics *GetLowLevel();

		cGraphicsDrawer *GetDrawer();

		cRenderer2D *GetRenderer2D();
		cRenderer3D *GetRenderer3D(){return m_pRenderer3D;}
		cMeshCreator *GetMeshCreator(){return m_pMeshCreator;}
		cMaterialHandler *GetMaterialHandler(){return m_pMaterialHandler;}
	private:
		iLowLevelGraphics *m_pLowLevelGraphics;
		iLowLevelResources *m_pLowLevelResources;
		cGraphicsDrawer *m_pDrawer;
		cMeshCreator *m_pMeshCreator;
		cMaterialHandler *m_pMaterialHandler;
		cRenderer2D *m_pRenderer2D;
		cRenderer3D *m_pRenderer3D;
		cRendererPostEffects *m_pRendererPostEffects;
		cRenderList *m_pRenderList;
	};
};

#endif