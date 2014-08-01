#include "graphics/MeshCreator.h"
#include "graphics/Mesh2d.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Bitmap2D.h"
#include "resources/Resources.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "resources/MaterialManager.h"
#include "resources/AnimationManager.h"

#include "math/Math.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMeshCreator::cMeshCreator(iLowLevelGraphics *a_pLowLevelGraphics, cResources *a_pResources)
	{
		m_pLowLevelGraphics = a_pLowLevelGraphics;
		m_pResources = a_pResources;
	}

	cMeshCreator::~cMeshCreator()
	{
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMesh2D *cMeshCreator::Create2D(tString a_sName, cVector2f m_vSize)
	{
		int i;
		tString sMeshName = cString::ToLowerCase(a_sName);
		cMesh2D *pMesh;

		if (sMeshName == "square")
		{
			cVector2f vPos[4] = {cVector2f(m_vSize.x/2, -m_vSize.y/2),cVector2f(m_vSize.x/2, m_vSize.y/2),
				cVector2f(-m_vSize.x/2, m_vSize.y/2),cVector2f(-m_vSize.x/2, -m_vSize.y/2)};

			pMesh = efeNew(cMesh2D, ());
			for (i=0;i<4;i++)
			{
				pMesh->AddVertex(vPos[i],0,1);
				pMesh->AddEdgeIndex(i);
			}
			for (i=1;i<4;i++) pMesh->AddIndex(i);
			for (i=0;i<3;i++) pMesh->AddIndex(i<2?i:3);

			return pMesh;
		}
		else if (sMeshName == "tri_1_to_1")
		{
			cVector2f vPos[3] = {cVector2f(m_vSize.x/2, -m_vSize.y/2),cVector2f(m_vSize.x/2, m_vSize.y/2),
				cVector2f(-m_vSize.x/2, m_vSize.y/2)};

			pMesh = efeNew(cMesh2D, ());
			for (i=0;i<3;i++)
			{
				pMesh->AddVertex(vPos[i],0,1);
				pMesh->AddEdgeIndex(i);
				pMesh->AddIndex(i);
			}

			return pMesh;
		}

		return NULL;
	}

	//--------------------------------------------------------------

	cMesh *cMeshCreator::CreateBox(const tString &a_sName, cVector3f a_vSize, const tString &a_sMaterial)
	{
		cMesh *pMesh = efeNew(cMesh, (a_sName, m_pResources->GetMaterialManager(), 
			m_pResources->GetAnimationManager()));

		cSubMesh *pSubMesh = pMesh->CreateSubMesh("Main");

		iMaterial *pMat = m_pResources->GetMaterialManager()->CreateMaterial(a_sMaterial);
		pSubMesh->SetMaterial(pMat);
		iVertexBuffer *pVtxBuffer = CreateBoxVertexBuffer(a_vSize);
		pSubMesh->SetVertexBuffer(pVtxBuffer);

		return pMesh;
	}

	cMesh *cMeshCreator::CreateSphere(const tString &a_sName, int a_lSubdivLevel)
	{
		cMesh *pMesh = efeNew(cMesh, (a_sName, m_pResources->GetMaterialManager(), 
			m_pResources->GetAnimationManager()));

		cSubMesh *pSubMesh = pMesh->CreateSubMesh("Main");

		iVertexBuffer *pVtxBuffer = CreateSphereVertexBuffer(a_lSubdivLevel);
		pSubMesh->SetVertexBuffer(pVtxBuffer);

		return pMesh;
	}

	cMesh *cMeshCreator::CreateTerrain(iBitmap2D *a_pHeightmap)
	{
		cMesh *pMesh = efeNew(cMesh, ("Terrain", m_pResources->GetMaterialManager(), 
			m_pResources->GetAnimationManager()));

		cSubMesh *pSubMesh = pMesh->CreateSubMesh("Main");

		iVertexBuffer *pVtxBuffer = CreateTerrainVertexBuffer(a_pHeightmap);
		pSubMesh->SetVertexBuffer(pVtxBuffer);

		return pMesh;
	}

	iVertexBuffer *cMeshCreator::CreateSkyBoxVertexBuffer(float a_fSize)
	{
		iVertexBuffer *pSkyBox = m_pLowLevelGraphics->CreateVertexBuffer(
			/*eVertexFlag_Color0 | */eVertexFlag_Position /*| eVertexFlag_Texture0 |
			eVertexFlag_Texture1 | eVertexFlag_Normal*/, eVertexBufferDrawType_TriangleList,
			eVertexBufferUsageType_Static, 8, 36);

		float fSize = a_fSize;

		/*for (int x=-1; x<1; x++)
			for (int y=-1; y<1; y++)
				for (int z=-1; z<1; z++)
				{
					if (x==0 && y==0 && z==0)continue;
					if (std::abs(x) + std::abs(y) + std::abs(z) > 1)continue;

					cVector3f vDir;
					cVector3f vSize;

					cVector3f vAdd[4];
					if (std::abs(x))
					{
						vDir.x = (float)x;

						vAdd[0].y = 1;vAdd[0].z = 1;
						vAdd[1].y = -1;vAdd[1].z = 1;
						vAdd[2].y = -1;vAdd[2].z = -1;
						vAdd[3].y = 1;vAdd[3].z = -1;
					}
					else if (std::abs(y))
					{
						vDir.y = (float)y;

						vAdd[0].z = 1;vAdd[0].z = 1;
						vAdd[1].z = -1;vAdd[1].z = 1;
						vAdd[2].z = -1;vAdd[2].z = -1;
						vAdd[3].z = 1;vAdd[3].z = -1;
					}
					else if (std::abs(z))
					{
						vAdd[0].y = 1;vAdd[0].x = 1;
						vAdd[1].y = 1;vAdd[1].x = -1;
						vAdd[2].y = -1;vAdd[2].x = -1;
						vAdd[3].y = -1;vAdd[3].x = 1;

						vDir.z = (float)z;
					}

					for (int i=0;i<4;i++)
					{
						int idx = i;
						if (x + y + z < 0) idx = 3 - i;

						pSkyBox->AddColor(eVertexFlag_Color0, cColor(1,1,1,1));
						pSkyBox->AddVertex(eVertexFlag_Position, (vDir+vAdd[idx])*fSize);
						pSkyBox->AddVertex(eVertexFlag_Texture0, vDir+vAdd[idx]);

						vSize = vDir+vAdd[idx];
						}
						}*/
		/*pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(-1,-1,-1));
		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(-1,1,-1));
		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(1,1,-1));
		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(1,-1,-1));
		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(-1,-1,1));
		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(-1,1,1));
		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(1,1,1));
		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(1,-1,1));

		pSkyBox->AddVertex(eVertexFlag_Texture0, cVector3f(0,1,1));
		pSkyBox->AddVertex(eVertexFlag_Texture0, cVector3f(0,0,1));
		pSkyBox->AddVertex(eVertexFlag_Texture0, cVector3f(1,0,1));
		pSkyBox->AddVertex(eVertexFlag_Texture0, cVector3f(1,1,1));
		pSkyBox->AddVertex(eVertexFlag_Texture0, cVector3f(0,1,1));
		pSkyBox->AddVertex(eVertexFlag_Texture0, cVector3f(0,0,1));
		pSkyBox->AddVertex(eVertexFlag_Texture0, cVector3f(1,0,1));
		pSkyBox->AddVertex(eVertexFlag_Texture0, cVector3f(1,1,1));*/

		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(-1,1,1));
		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(1,1,1));
		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(-1,1,-1));
		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(1,1,-1));
		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(-1,-1,-1));
		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(1,-1,-1));
		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(-1,-1,1));
		pSkyBox->AddVertex(eVertexFlag_Position, cVector3f(1,-1,1));

		pSkyBox->AddTriangle(0,1,2);
		pSkyBox->AddTriangle(2,1,3);
		pSkyBox->AddTriangle(2,3,4);
		pSkyBox->AddTriangle(4,5,3);
		pSkyBox->AddTriangle(4,5,6);
		pSkyBox->AddTriangle(6,7,5);
		pSkyBox->AddTriangle(0,1,6);
		pSkyBox->AddTriangle(6,6,1);
		pSkyBox->AddTriangle(0,2,4);
		pSkyBox->AddTriangle(4,6,0);
		pSkyBox->AddTriangle(1,3,5);
		pSkyBox->AddTriangle(5,7,1);

		if (!pSkyBox->Compile(0))
		{
			efeDelete(pSkyBox);
			return NULL;
		}

		return pSkyBox;
	}

	iVertexBuffer *cMeshCreator::CreateBoxVertexBuffer(cVector3f a_vSize)
	{
		iVertexBuffer *pBox = m_pLowLevelGraphics->CreateVertexBuffer(
			eVertexFlag_Color0 | eVertexFlag_Position | eVertexFlag_Texture0 | 
			eVertexFlag_Tangent | eVertexFlag_Normal,
			eVertexBufferDrawType_TriangleList, eVertexBufferUsageType_Static);

		a_vSize = a_vSize * 0.5;

		int lVtxIdx = 0;

		for (int x=-1; x<=1; x++)
		{
			for (int y=-1; y<=1; y++)
			{
				for (int z=-1; z<=1; z++)
				{
					if (x==0 && y==0 && z==0)continue;
					if (std::abs(x) + std::abs(y) + std::abs(z) > 1)continue;

					cVector3f vDir;
					cVector3f vSize;

					cVector3f vAdd[4];
					if (std::abs(x))
					{
						vDir.x = (float)x;

						vAdd[0].y = 1;vAdd[0].z = 1;
						vAdd[1].y = -1;vAdd[1].z = 1;
						vAdd[2].y = -1;vAdd[2].z = -1;
						vAdd[3].y = 1;vAdd[3].z = -1;
					}
					else if (std::abs(y))
					{
						vDir.y = (float)y;

						vAdd[0].z = 1;vAdd[0].x = 1;
						vAdd[1].z = -1;vAdd[1].x = 1;
						vAdd[2].z = -1;vAdd[2].x = -1;
						vAdd[3].z = 1;vAdd[3].x = -1;
					}
					else if (std::abs(z))
					{
						vAdd[0].y = 1;vAdd[0].x = 1;
						vAdd[1].y = 1;vAdd[1].x = -1;
						vAdd[2].y = -1;vAdd[2].x = -1;
						vAdd[3].y = -1;vAdd[3].x = 1;

						vDir.z = (float)z;
					}

					for (int i=0;i<4;i++)
					{
						int idx = GetBoxIdx(i,x,y,z);
						cVector3f vTex = GetBoxTex(i,x,y,z,vAdd);

						pBox->AddColor(eVertexFlag_Color0, cColor(1,1,1,1));
						pBox->AddVertex(eVertexFlag_Position, (vDir+vAdd[idx])*a_vSize);
						pBox->AddVertex(eVertexFlag_Normal, vDir);

						cVector3f vCoord = cVector3f((vTex.x+1)*0.5f,(vTex.y+1)*0.5,0);
						pBox->AddVertex(eVertexFlag_Texture0, vCoord);

						vSize = vDir+vAdd[idx];
					}

					pBox->AddTriangle(lVtxIdx+0,lVtxIdx+1,lVtxIdx+2);
					pBox->AddTriangle(lVtxIdx+2,lVtxIdx+3,lVtxIdx+0);

					lVtxIdx += 4;
				}
			}
		}

		if (!pBox->Compile(eVertexCompileFlag_CreateTangents))
		{
			efeDelete(pBox);
			return NULL;
		}
		return pBox;
	}

	void Subdivide(tVector3fVec &a_vDest, cVector3f &a_vV1, cVector3f &a_vV2, cVector3f &a_vV3, int a_lSubLevel)
	{
		if (a_lSubLevel != 0)
		{
			a_lSubLevel--;

			cVector3f vV4 = cMath::Vector3Normalize(a_vV1 + a_vV2);
			cVector3f vV5 = cMath::Vector3Normalize(a_vV2 + a_vV3);
			cVector3f vV6 = cMath::Vector3Normalize(a_vV3 + a_vV1);

			Subdivide(a_vDest, a_vV1,	vV4,	vV6, a_lSubLevel);
			Subdivide(a_vDest, vV4,		vV5,	vV6, a_lSubLevel);
			Subdivide(a_vDest, vV4,		a_vV2,	vV5, a_lSubLevel);
			Subdivide(a_vDest, vV6,		vV5,	a_vV3, a_lSubLevel);
		}
		else
		{
			a_vDest.push_back(a_vV1);
			a_vDest.push_back(a_vV2);
			a_vDest.push_back(a_vV3);
			/**a_vDest++ = a_vV1;
			*a_vDest++ = a_vV2;
			*a_vDest++ = a_vV3;*/
		}
	}

	iVertexBuffer *cMeshCreator::CreateSphereVertexBuffer(int a_lSubdivLevel)
	{
		iVertexBuffer *pSphere = m_pLowLevelGraphics->CreateVertexBuffer(
			eVertexFlag_Position, eVertexBufferDrawType_TriangleList, eVertexBufferUsageType_Static);

		cVector3f vX0(-1,0,0);
		cVector3f vX1(1,0,0);
		cVector3f vY0(0,-1,0);
		cVector3f vY1(0,1,0);
		cVector3f vZ0(0,0,-1);
		cVector3f vZ1(0,0,1);

		int lVerticesNum = 8 * 3 * (1 << (2 * a_lSubdivLevel));

		cVector3f *pOrigVertices = efeNewArray(cVector3f,lVerticesNum);
		tVector3fVec vVertices;
		cVector3f *pVertices = pOrigVertices;
		Subdivide(vVertices, vY0, vX0, vZ0, a_lSubdivLevel);
		Subdivide(vVertices, vY0, vZ0, vX1, a_lSubdivLevel);
		Subdivide(vVertices, vY0, vX1, vZ1, a_lSubdivLevel);
		Subdivide(vVertices, vY0, vZ1, vX0, a_lSubdivLevel);
		Subdivide(vVertices, vY1, vZ0, vX0, a_lSubdivLevel);
		Subdivide(vVertices, vY1, vX0, vZ1, a_lSubdivLevel);
		Subdivide(vVertices, vY1, vZ1, vX1, a_lSubdivLevel);
		Subdivide(vVertices, vY1, vX1, vZ0, a_lSubdivLevel);

		for (int i = 0; i < lVerticesNum; i++)
		{
			cVector3f w = pVertices[i];
			pSphere->AddVertex(eVertexFlag_Position, vVertices[i]);
			//pVertices++;
			pSphere->AddIndex(i);
		}

		efeDeleteArray(pOrigVertices);

		if (!pSphere->Compile(0))
		{
			efeDelete(pSphere);
			return NULL;
		}

		return pSphere;
	}

	//--------------------------------------------------------------

	iVertexBuffer *cMeshCreator::CreateTerrainVertexBuffer(iBitmap2D *a_pHeightmap)
	{
		float *pHeightData=0;

		int lHMWidth = a_pHeightmap->GetWidth();
		int lHMHeight = a_pHeightmap->GetHeight();

		unsigned int lVertexNum = lHMWidth * lHMHeight;
		pHeightData = new float[lVertexNum];

		unsigned char *pPixels = (unsigned char *)a_pHeightmap->GetPixels();
		int k=0;
		for (int r = 0; r < lHMHeight; r++)
		{
			for (int c = 0; c < lHMWidth; c++)
			{
				float fDivisor = 100 / 6;
				pHeightData[r * lHMWidth + c] = (float)pPixels[k] / fDivisor - 3;
				k += 4;
			}
		}

		cVertex *pMeshVertices = new cVertex[lVertexNum];
		float vTexCoord[2] = {0,1}, vCoordStep[2] = {1.0f / lHMWidth, 1.0f / lHMHeight};

		float z = -32, x = -32;
		int cnt = 0, w = 0, h = 0;

		while (h < lHMHeight)
		{
			while (w < lHMWidth)
			{
				pMeshVertices[cnt].tex = cVector3f(vTexCoord[0], vTexCoord[1], 0);
				pMeshVertices[cnt++].pos = cVector3f(x, pHeightData[cnt], z);
				vTexCoord[0] += vCoordStep[0];
				w++;
				x += 64.0f/(lHMWidth-1);
			}

			x = -32;
			z += 64.0f/(lHMWidth-1);
			vTexCoord[0] = 0;
			vTexCoord[1] -= vCoordStep[1];
			w = 0;
			h++;
		}

		for (unsigned int i=0; i < lVertexNum - lHMWidth; i++)
		{
			cVector3f v1, v2;

			v1 = pMeshVertices[i+lHMWidth].pos - pMeshVertices[i].pos;
			v2 = pMeshVertices[i+1].pos - pMeshVertices[i].pos;

			cVector3f vNormal = cMath::Vector3Cross(v1, v2);

			pMeshVertices[i].norm += vNormal;
			pMeshVertices[i+1].norm += vNormal;
			pMeshVertices[i+lHMWidth].norm += vNormal;

			v1 = pMeshVertices[i+lHMWidth].pos - pMeshVertices[i+1].pos;
			v2 = pMeshVertices[i+lHMWidth+1].pos - pMeshVertices[i+1].pos;

			vNormal = cMath::Vector3Cross(v1, v2);

			pMeshVertices[i+1].norm += vNormal;
			pMeshVertices[i+lHMWidth].norm += vNormal;
			pMeshVertices[i+lHMWidth+1].norm += vNormal;

			if (i % lHMWidth == lHMWidth - 2) i++;
		}

		for (unsigned int i=0; i < lVertexNum - lHMWidth; i++)
			pMeshVertices[i].norm.Normalize();

		//create indices
		unsigned int lIndexNum = (lHMHeight-1) * (lHMWidth-1)*6;
		unsigned int *pMeshIndices = new unsigned int[lIndexNum];
		cnt = 0;

		for (unsigned int i = 0; i < lVertexNum - lHMWidth; i++)
		{
			pMeshIndices[cnt++] = i;
			pMeshIndices[cnt++] = i+lHMWidth;
			pMeshIndices[cnt++] = i+1;
			pMeshIndices[cnt++] = i+1;
			pMeshIndices[cnt++] = i+lHMWidth;
			pMeshIndices[cnt++] = i+lHMWidth+1;

			if (i % lHMWidth == lHMWidth - 2) i++;
		}

		iVertexBuffer *pTerrain = m_pLowLevelGraphics->CreateVertexBuffer(
			eVertexFlag_Color0 | eVertexFlag_Position | eVertexFlag_Texture0 | 
			eVertexFlag_Tangent | eVertexFlag_Normal,
			eVertexBufferDrawType_TriangleList, eVertexBufferUsageType_Static, lVertexNum, lIndexNum);

		for (unsigned int v = 0; v < lVertexNum; v++)
		{
			pTerrain->AddColor(eVertexFlag_Color0, pMeshVertices[v].col);
			pTerrain->AddVertex(eVertexFlag_Position, pMeshVertices[v].pos);
			pTerrain->AddVertex(eVertexFlag_Texture0, pMeshVertices[v].tex);
			pTerrain->AddVertex(eVertexFlag_Tangent, pMeshVertices[v].tan);
			pTerrain->AddVertex(eVertexFlag_Normal, pMeshVertices[v].norm);
		}
		for (unsigned int i = 0; i < lIndexNum; i++)
		{
			pTerrain->AddIndex(pMeshIndices[i]);
		}

		pTerrain->Compile(eVertexCompileFlag_CreateTangents);

		return pTerrain;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	cVector3f cMeshCreator::GetBoxTex(int i, int x, int y, int z, cVector3f *vAdd)
	{
		cVector3f vTex;

		if (std::abs(x))
		{
			vTex.x = vAdd[i].z;
			vTex.y = vAdd[i].y;
		}
		else if (std::abs(y))
		{
			vTex.x = vAdd[i].x;
			vTex.y = vAdd[i].z;
		}
		else if (std::abs(z))
		{
			vTex.x = vAdd[i].x;
			vTex.y = vAdd[i].y;
		}

		if (x+y+z<0)
		{
			vTex.x = -vTex.x;
			vTex.y = -vTex.y;
		}

		return vTex;
	}

	//--------------------------------------------------------------

	int cMeshCreator::GetBoxIdx(int i, int x, int y, int z)
	{
		int idx = i;
		if (x + y + z > 0) idx = 3 - i;

		return idx;
	}

	//--------------------------------------------------------------
}