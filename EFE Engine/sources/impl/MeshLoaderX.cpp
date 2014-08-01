#include "impl/MeshLoaderX.h"

#include "graphics/LowLevelGraphics.h"

namespace efe
{
	cMeshLoaderX::cMeshLoaderX(iLowLevelGraphics *a_pLowLevelGraphics)
		:iMeshLoader(a_pLowLevelGraphics)
	{
		m_pDevice = a_pLowLevelGraphics->GetDevice();
	}

	cMeshLoaderX::~cMeshLoaderX()
	{
	}

	cMesh *cMeshLoaderX::LoadMesh(const tString &a_sName, tMeshLoadFlag a_Flags)
	{
		LPD3DXBUFFER MaterialBuffer;
		D3DXLoadMeshFromX(a_sName.c_str(),D3DXMESH_SYSTEMMEM, m_Device, NULL, &MaterialBuffer, NULL, &m_lMaterialsNum, &m_Mesh);

		D3DXMATERIAL *Materials = (D3DXMATERIAL*) MaterialBuffer->GetBufferPointer();
		m_pMaterial = new D3DMATERIAL9[m_lMaterialsNum];
		m_pTexture = new LPDIRECT3DTEXTURE9[m_lMaterialsNum];

		for (int i=0; i < m_lMaterialsNum; i++)
		{
			m_pMaterial[i] = Materials[i].MatD3D;
			m_pMaterial[i].Ambient = m_pMaterial[i].Diffuse;
			m_pTexture = NULL;
			D3DXCreateTextureFromFile(m_Device, Materials[i].pTextureFilename, &m_pTexture[i]);
		}
		MaterialBuffer->Release();
		return NULL;
	}

}