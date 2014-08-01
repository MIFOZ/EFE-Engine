#ifndef EFE_MESH_LOADER_X_H
#define EFE_MESH_LOADER_X_H

#include "math/MathTypes.h"
#include "resources/MeshLoader.h"
#include "graphics/VertexBuffer.h"

namespace efe
{
	class iLowLevelGraphics;

	class cMeshLoaderX : public iMeshLoader
	{
	public:
		cMeshLoaderX(iLowLevelGraphics *a_pLowLevelGraphics);
		~cMeshLoaderX();

		cMesh *LoadMesh(const tString &a_sName, tMeshLoadFlag a_Flags);
	private:
		/*LPD3DXMESH m_Mesh;
		LPDIRECT3DVERTEXBUFFER9 m_VtxBuffer;
		D3DMATERIAL9 *m_pMaterial;
		//LPDIRECT3DTEXTURE9 *m_pTexture;
		unsigned long m_lMaterialsNum;
		//iD3D10Device *m_pDevice;
		iLowLevelGraphics *m_pLowLevelGraphics;*/
	};
};
#endif