#ifndef EFE_MESHCREATOR_H
#define EFE_MESHCREATOR_H

#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"

namespace efe
{
	class cMesh2D;
	class iVertexBuffer;
	class iLowLevelGraphics;
	class cResources;
	class cMesh;
	class iBitmap2D;

	class cMeshCreator
	{
	public:
		cMeshCreator(iLowLevelGraphics *a_pLowLevelGraphics, cResources *a_pResources);
		~cMeshCreator();

		cMesh2D *Create2D(tString a_sName, cVector2f m_vSize);

		cMesh *CreateBox(const tString &a_sName, cVector3f a_vSize, const tString &a_sMaterial);

		cMesh *CreateSphere(const tString &a_sName, int a_lSubdivLevel);

		cMesh *CreateTerrain(iBitmap2D *a_pHeightmap);

		iVertexBuffer *CreateSkyBoxVertexBuffer(float a_fSize);
		iVertexBuffer *CreateBoxVertexBuffer(cVector3f a_vSize);
		iVertexBuffer *CreateSphereVertexBuffer(int a_lSubdivLevel);
		iVertexBuffer *CreateTerrainVertexBuffer(iBitmap2D *a_pHeightmap);

	private:
		cVector3f GetBoxTex(int i, int x, int y, int z, cVector3f *vAdd);
		int GetBoxIdx(int i, int x, int y, int z);

		iLowLevelGraphics *m_pLowLevelGraphics;
		cResources *m_pResources;
	};
};

#endif