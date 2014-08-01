#include "scene/Tile.h"
#include "scene/TileMap.h"
#include "scene/TileData.h"
#include "scene/TileSet.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-----------------------------------------------------------

	cTile::cTile(iTileData *a_pTileData, eTileRotation a_Angle,
					const cVector3f &a_vPos, const cVector2f &a_vSize, cCollisionMesh2D *a_pCollMesh)
	{
		m_vPosition = a_vPos + a_vSize/2;

		m_pTileData = a_pTileData;
		m_Angle = a_Angle;

		m_pCollMesh = a_pCollMesh;

		m_lFlags = 0;
	}

	//-----------------------------------------------------------

	cTile::~cTile()
	{
		if (m_pCollMesh) efeDelete(m_pCollMesh);
	}
}