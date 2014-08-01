#include "scene/TileLayer.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-----------------------------------------------------------

	cTileLayer::cTileLayer(unsigned int a_lW, unsigned int a_lH, bool a_bCollision, bool a_bLit,
							eTileLayerType a_Type, float a_fZ)
	{
		m_vSize = cVector2l(a_lW, a_lH);
		m_bCollision = a_bCollision;
		m_bLit = a_bLit;
		m_Type = a_Type;
		m_fZ = a_fZ;

		m_vTile.resize(a_lW*a_lH);
		m_vTile.assign(m_vTile.size(),NULL);
	}

	//--------------------------------------------------------------

	cTileLayer::~cTileLayer()
	{
		Log(" Deleting tilelayer.\n");

		for (int i=0;i<(int)m_vTile.size();i++)
		{
			if (m_vTile[i])
				efeDelete(m_vTile[i]);
		}

		m_vTile.clear();
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cTileLayer::SetTile(unsigned int a_lX, unsigned int a_lY, cTile *a_Val)
	{
		int lNum = a_lY*m_vSize.x+a_lX;
		if (lNum>=(int)m_vTile.size())
			return false;

		if (a_Val == NULL)
		{
			if (m_vTile[lNum]) efeDelete(m_vTile[lNum]);
			m_vTile[lNum] = NULL;
		}
		else
		{
			if (m_vTile[lNum]==NULL)
				m_vTile[lNum] = a_Val;
			else
				*m_vTile[lNum] = *a_Val;
		}

		return true;
	}

	//--------------------------------------------------------------

	cTile *cTileLayer::GetAt(int a_lX, int a_lY)
	{
		if (a_lX<0 || a_lX>=m_vSize.x || a_lY<0 || a_lY>=m_vSize.y) return NULL;

		int lNum = a_lY*m_vSize.x+a_lX;
		if (lNum>=(int)m_vTile.size()) return NULL;

		return m_vTile[lNum];
	}

	//--------------------------------------------------------------

	cTile *cTileLayer::GetAt(int a_lNum)
	{
		return m_vTile[a_lNum];
	}
}