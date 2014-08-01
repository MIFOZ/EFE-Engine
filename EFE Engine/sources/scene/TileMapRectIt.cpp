#include "scene/TileMapRectIt.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	cTileMapRectIt::cTileMapRectIt(cVector2l a_vPos, cVector2l a_vSize, cTileMap *a_pTileMap, int a_lLayer)
	{
		m_pTileMap = a_pTileMap;
		m_pTile = NULL;

		m_vPos = a_vPos;
		m_vSize = a_vSize;

		m_lLayer = a_lLayer;
		m_lLayerCount = 0;
		m_lCurrentLayer = 0;

		if (m_vPos.x<0){m_vSize.x+=m_vPos.x; m_vPos.x=0;}
		if (m_vPos.y<0){m_vSize.y+=m_vPos.y; m_vPos.y=0;}

		if (m_vPos.x + m_vSize.x > m_pTileMap->m_vSize.x)
			m_vSize.x -= (m_vPos.x + m_vSize.x) - (m_pTileMap->m_vSize.x);

		if (m_vPos.y + m_vSize.y > m_pTileMap->m_vSize.y)
			m_vSize.y -= (m_vPos.y + m_vSize.y) - (m_pTileMap->m_vSize.y);

		m_lTileNum = m_vPos.x + m_vPos.y*m_pTileMap->m_vSize.x;
		m_lTileColAdd = m_pTileMap->m_vSize.x - m_vSize.x;
		m_lTileRowCount = m_vSize.y;
		m_lTileColCount = m_vSize.x;

		if (m_vSize.x<=0 || m_vSize.y<=0 || m_vPos.x>=m_pTileMap->m_vSize.x || m_vPos.y>=m_pTileMap->m_vSize.y)
		{
			m_lTileColCount = 0;
			m_lTileRowCount = 0;
			m_lLayerCount = (int)m_pTileMap->m_vTileLayer.size();
		}

		m_bUpdated = false;
	}

	//--------------------------------------------------------------

	cTileMapRectIt::~cTileMapRectIt()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cTileMapRectIt::HasNext()
	{
		GetTile();

		return m_pTile!=NULL;
	}

	//--------------------------------------------------------------

	cTile *cTileMapRectIt::Next()
	{
		GetTile();

		m_bUpdated = false;
		return m_pTile;
	}

	//--------------------------------------------------------------

	cTile *cTileMapRectIt::PeekNext()
	{
		GetTile();

		return m_pTile;
	}

	//--------------------------------------------------------------

	int cTileMapRectIt::GetNum()
	{
		return m_lTileNum;
	}

	//--------------------------------------------------------------

	int cTileMapRectIt::GetCurrentLayer()
	{
		return m_lCurrentLayer;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cTileMapRectIt::GetTile()
	{
		if (m_bUpdated) return;
		m_bUpdated = true;

		while (true)
		{
			if ((m_lLayer>=0 && m_lLayerCount>0) || (m_lLayer==-1 && m_lLayerCount>=(int)m_pTileMap->m_vTileLayer.size()))
			{
				m_lLayerCount = 0;

				m_lTileNum++;
				m_lTileColCount--;
				if (m_lTileColCount<=0)
				{
					m_lTileColCount = m_vSize.x;
					m_lTileRowCount--;
					if (m_lTileRowCount<=0)
					{
						m_pTile = NULL;
						break;
					}
					m_lTileNum += m_lTileColAdd;
				}
			}
			else
			{
				if (m_lLayer<0)
				{
					m_pTile = m_pTileMap->m_vTileLayer[m_lLayerCount]->m_vTile[m_lTileNum];
					m_lCurrentLayer = m_lLayerCount;
				}
				else
				{
					m_pTile = m_pTileMap->m_vTileLayer[m_lLayer]->m_vTile[m_lTileNum];
					m_lCurrentLayer = m_lLayer;
				}

				m_lLayerCount++;

				if (m_pTile!=NULL)
				{
					iTileData *pData = m_pTile->GetTileData();
					if (pData && pData->IsSolid())
						m_lLayerCount = (int)m_pTileMap->m_vTileLayer.size();

					break;
				}
			}
		}
	}
}