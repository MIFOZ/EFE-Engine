#include "scene/TileMap.h"
#include "math/Math.h"
#include "resources/ResourceImage.h"
#include "scene/TileMapRectIt.h"

#include "resources/Resources.h"
#include "system/LowLevelSystem.h"
#include "graphics/Renderer2D.h"
#include "resources/TileSetManager.h"
#include "graphics/GraphicsTypes.h"
#include "graphics/Renderer2D.h"
#include "graphics/RenderObject2D.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	cTileMap::cTileMap(cVector2l a_vSize, float a_fTileSize, cGraphics *a_pGraphics, cResources *a_pResources)
	{
		m_fTileSize = a_fTileSize;
		m_vSize = a_vSize;
		m_pGraphics = a_pGraphics;
		m_pResources = a_pResources;
		m_lShadowLayer = 0;
	}

	//--------------------------------------------------------------

	cTileMap::~cTileMap()
	{
		Log(" Deleting tilemap.\n");
		for (tTileLayerVecIt it = m_vTileLayer.begin();it!=m_vTileLayer.end();it++)
			efeDelete(*it);
		for (tTileSetVecIt it = m_vTileSet.begin();it!=m_vTileSet.end();it++)
			m_pResources->GetTileSetManager()->Destroy(*it);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iTileMapIt *cTileMap::GetRectIterator(const cRectf &a_Rect, int a_lLayer)
	{
		cVector2l vPos = cVector2l((int)floor(a_Rect.x/m_fTileSize),
									(int)floor(a_Rect.y/m_fTileSize));

		cVector2l vSize = cVector2l((int)(a_Rect.w/m_fTileSize)+1,
									(int)(a_Rect.h/m_fTileSize)+1);

		if (a_Rect.x+a_Rect.w>=(vPos.x+vSize.x)*m_fTileSize) vSize.x++;
		if (a_Rect.y+a_Rect.h>=(vPos.y+vSize.y)*m_fTileSize) vSize.y++;

		return efeNew(cTileMapRectIt, (vPos, vSize, this, a_lLayer));
	}

	//--------------------------------------------------------------

	void cTileMap::Render(cCamera2D *a_pCam)
	{
		cRectf Rect;
		a_pCam->GetClipRect(Rect);

		iTileMapIt *TileIt = GetRectIterator(Rect, -1);

		while (TileIt->HasNext())
			RenderTileData(TileIt->Next(), TileIt->GetCurrentLayer());

		efeDelete(TileIt);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cTileMap::RenderTileData(cTile *a_pTile, int a_lLayer)
	{
		static int count = 0;

		cTileDataNormal *pData = static_cast<cTileDataNormal*>(a_pTile->GetTileData());
		if (pData==NULL) return;
		cRectf _obj = cRectf(a_pTile->GetPosition().x-m_fTileSize/2,
			a_pTile->GetPosition().y-m_fTileSize/2,m_fTileSize, m_fTileSize);
		cRenderObject2D _obj2 = cRenderObject2D(pData->GetMaterial(),
								pData->GetVertexVec(a_pTile->GetAngle()),
								pData->GetIndexVec(a_pTile->GetAngle()),
								ePrimitiveType_Quad, a_pTile->GetPosition().z,
								_obj,
								NULL, a_pTile->GetPositionPtr());
		m_pGraphics->GetRenderer2D()->AddObject(_obj2);

		count++;
	}
}