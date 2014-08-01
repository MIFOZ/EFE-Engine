#ifndef EFE_TILEMAP_RECT_IT_H
#define EFE_TILEMAP_RECT_IT_H

#include "scene/TileMapIt.h"
#include "scene/TileMap.h"
#include "math/MathTypes.h"

namespace efe
{
	class cTileMapRectIt : public iTileMapIt
	{
	public:
		cTileMapRectIt(cVector2l a_vPos, cVector2l a_vSize, cTileMap *a_pTileMap, int a_lLayer);
		~cTileMapRectIt();

		bool HasNext();
		cTile *Next();
		cTile *PeekNext();
		int GetNum();
		int GetCurrentLayer();

	private:
		cVector2l m_vPos;
		cVector2l m_vSize;
		int m_lLayer;
		int m_lLayerCount;
		int m_lTileNum;
		int m_lTileColAdd;
		int m_lTileRowCount;
		int m_lTileColCount;

		int m_lCurrentLayer;

		bool m_bUpdated;

		cTileMap *m_pTileMap;
		cTile *m_pTile;

		void GetTile();
	};
};
#endif