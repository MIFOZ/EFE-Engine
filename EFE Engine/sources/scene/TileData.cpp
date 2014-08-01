#include "scene/TileData.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	cTileDataNormal::cTileDataNormal(cImageManager *a_pImageManager, cVector2f a_vTileSize)
	{
		m_pImageManager = a_pImageManager;

		m_vImage.resize(eMaterialTexture_LastEnum);
		m_vImage.assign(m_vImage.size(), NULL);

		m_vTileSize = a_vTileSize;
		m_CollisionType = eCollisionType_Normal;

		m_pMaterial = NULL;
		m_pMesh = NULL;
		m_pCollideMesh = NULL;
	}

	//--------------------------------------------------------------

	cTileDataNormal::~cTileDataNormal()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cTileDataNormal::Destroy()
	{
		if (m_pMaterial) efeDelete(m_pMaterial);
		if (m_pMesh) efeDelete(m_pMesh);
		if (m_pCollideMesh) efeDelete(m_pCollideMesh);
	}

	//--------------------------------------------------------------

	tVertexVec *cTileDataNormal::GetVertexVec(eTileRotation a_Rotation)
	{
		return m_vVtx[a_Rotation];
	}

	//--------------------------------------------------------------

	tUIntVec *cTileDataNormal::GetIndexVec(eTileRotation a_Rotation)
	{
		return m_vIdx;
	}

	//--------------------------------------------------------------

	tVertexVec *cTileDataNormal::GetCollideVertexVec(eTileRotation a_Rotation)
	{
		return m_vCollideVtx[a_Rotation];
	}

	//--------------------------------------------------------------

	tUIntVec *cTileDataNormal::GetCollideIndexVec(eTileRotation a_Rotation)
	{
		return m_vCollideIdx;
	}

	//--------------------------------------------------------------

	void cTileDataNormal::SetData(cMesh2D *a_pMesh, iMaterial *a_pMaterial)
	{
		SetMaterial(a_pMaterial);
		SetMesh(a_pMesh);
	}

	//--------------------------------------------------------------

	cMesh2D *cTileDataNormal::GetCollideMesh()
	{
		return m_pCollideMesh;
	}

	//--------------------------------------------------------------

	void cTileDataNormal::SetCollideMesh(cMesh2D *a_pCollideMesh)
	{
		m_pCollideMesh = a_pCollideMesh;

		for (int i=0;i<eTileRotation_LastEnum;i++)
			m_vCollideVtx[i] = m_pCollideMesh->GetVertexVec(cRectf(0,0,1,1), m_vTileSize, (eTileRotation)i);

		m_vCollideIdx = m_pCollideMesh->GetIndexVec();
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cTileDataNormal::SetMesh(cMesh2D *a_pMesh)
	{
		m_pMesh = a_pMesh;

		m_pMesh->CreateTileVertexVec();

		if (m_pMaterial)
		{
			cRectf ImageRect = m_pMaterial->GetTextureOffset(eMaterialTexture_Diffuse);

			for (int i=0;i<eTileRotation_LastEnum;i++)
				m_vVtx[i] = m_pMesh->GetVertexVec(ImageRect, m_vTileSize, (eTileRotation)i);

			m_vIdx = m_pMesh->GetIndexVec();
		}
	}

	//--------------------------------------------------------------

	void cTileDataNormal::SetMaterial(iMaterial *a_pMaterial)
	{
		m_pMaterial = a_pMaterial;
	}
}