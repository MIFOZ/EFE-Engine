#ifndef EFE_TILE_H
#define EFE_TILE_H

#include <list>
#include "system/SystemTypes.h"
#include "scene/TileData.h"

namespace efe
{
	#define eTileFlag_Breakable (eFlagBit_0)

	class cTile
	{
	public:
		cTile(iTileData *a_pTileData, eTileRotation a_Angle,
				const cVector3f &a_vPos, const cVector2f &a_vSize, cCollisionMesh2D *a_pCollMesh);
		~cTile();

		iTileData *GetTileData(){return m_pTileData;}

		eTileRotation GetAngle(){return m_Angle;}

		const cVector3f &GetPosition()const{return m_vPosition;}
		cVector3f *GetPositionPtr(){return &m_vPosition;}

	private:
		cCollisionMesh2D *m_pCollMesh;

		tFlag m_lFlags;
		iTileData *m_pTileData;
		cVector3f m_vPosition;
		eTileRotation m_Angle;
	};

	typedef std::list<cTile*> tTileList;
	typedef tTileList::iterator tTileListIt;
};

#endif