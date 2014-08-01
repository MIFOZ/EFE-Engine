#ifndef EFE_TILEMAP_H
#define EFE_TILEMAP_H

#include <vector>
#include "graphics/Graphics.h"
#include "math/MathTypes.h"
#include "scene/Camera2D.h"
#include "scene/TileSet.h"
#include "scene/TileLayer.h"
#include "scene/TileMapIt.h"

namespace efe
{
	typedef std::vector<cTileSet*> tTileSetVec;
	typedef tTileSetVec::iterator tTileSetVecIt;

	typedef std::vector<cTileLayer*> tTileLayerVec;
	typedef tTileLayerVec::iterator tTileLayerVecIt;

	class cTileMap
	{
		friend class cTileMapRectIt;
	public:
		cTileMap(cVector2l a_vSize, float a_fTileSize, cGraphics *a_pGraphics, cResources *a_pResources);
		~cTileMap();

		iTileMapIt *GetRectIterator(const cRectf &a_Rect, int a_lLayer);

		void Render(cCamera2D *a_pCam);

	private:
		cGraphics *m_pGraphics;
		cResources *m_pResources;

		tTileSetVec m_vTileSet;
		tTileLayerVec m_vTileLayer;
		int m_lShadowLayer;

		cVector2l m_vSize;
		float m_fTileSize;
		int m_lCurrentLayer;

		inline void RenderTileData(cTile *a_pTile, int a_lLayer);
	};
};
#endif