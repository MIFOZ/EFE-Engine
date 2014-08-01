#ifndef EFE_TILELAYER_H
#define EFE_TILELAYER_H

#include <vector>
#include "scene/Tile.h"
#include "math/MathTypes.h"
#include "system/SystemTypes.h"

namespace efe
{
	enum eTileLayerType
	{
		eTileLayerType_Normal,
		eTileLayerType_LastEnum
	};

	typedef std::vector<cTile*> tTileVec;
	typedef tTileVec::iterator tTileVecIt;

	class cTileLayer
	{
		friend class cTileMapRectIt;
		friend class cTileMapLineIt;
	public:
		cTileLayer(unsigned int a_lW, unsigned int a_lH, bool a_bCollision, bool a_bLit, eTileLayerType a_Type, float a_fZ = 0);
		~cTileLayer();

		bool SetTile(unsigned int a_lX, unsigned int a_lY, cTile *a_Val);
		cTile *GetAt(int a_lX, int a_lY);
		cTile *GetAt(int a_lNum);

		void SetZ(float a_fZ){m_fZ = a_fZ;}
		float GetZ(){return m_fZ;}

		bool HasCollsion(){return m_bCollision;}
	private:
		tTileVec m_vTile;

		cVector2l m_vSize;
		bool m_bCollision;
		bool m_bLit;
		float m_fZ;
		eTileLayerType m_Type;
	};
};
#endif