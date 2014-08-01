#include "scene/GridMap2D.h"
#include "math/Math.h"

#include "scene/Entity2D.h"
#include "graphics/LowLevelGraphics.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGridMap2D::cGridMap2D(cVector2l a_vSize, cVector2l a_vGridSize, cVector2l a_vMaxGridSpan)
	{
		m_vSize = a_vSize;
		m_vGridSize = a_vGridSize;
		m_vMaxGridSpan = a_vMaxGridSpan;
		m_vGridNum = a_vSize/a_vGridSize+1;
		m_lGlobalCount = 0;

		m_vGrids.resize(m_vGridNum.x*m_vGridNum.y);

		for (int i=0;i<(int)m_vGrids.size();i++)
			m_vGrids[i] = cGrid2D();
	}

	cGridMap2D::~cGridMap2D()
	{
		for (tGrid2DObjectMapIt it = m_mapAllObjects.begin();it != m_mapAllObjects.end();++it)
		{
			it->second->Destoy();
			efeDelete(it->second);
		}

		m_mapAllObjects.clear();
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// GRIDMAP PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iGridMap2DIt *cGridMap2D::GetRectIterator(const cRectf &a_Rect)
	{
		cVector2l vPos = cVector2l((int)floor(a_Rect.x/(float)m_vGridSize.x),
									(int)floor(a_Rect.y/(float)m_vGridSize.y));

		cVector2l vSize = cVector2l((int)(a_Rect.w/(float)m_vGridSize.x)+1,
									(int)(a_Rect.h/(float)m_vGridSize.y)+1);

		if (a_Rect.x+a_Rect.w>=(vPos.x+vSize.x)*m_vGridSize.x)vSize.x++;
		if (a_Rect.y+a_Rect.h>=(vPos.y+vSize.y)*m_vGridSize.y)vSize.y++;

		return efeNew(cGridMap2DRectIt, (this, vPos, vSize));
	}

	//--------------------------------------------------------------

	bool cGridMap2D::AddEntity(iEntity2D *a_pEntity)
	{
		cGrid2DObject *pObject = efeNew(cGrid2DObject, (a_pEntity, this, m_lHandleCount));
		a_pEntity->SetGrid2DObject(pObject);

		m_mapAllObjects.insert(tGrid2DObjectMap::value_type(m_lHandleCount, pObject));

		m_lHandleCount++;
		return true;
	}

	//--------------------------------------------------------------

	bool cGridMap2D::RemoveEntity(iEntity2D *a_pEntity)
	{
		cGrid2DObject *pObject = a_pEntity->GetGrid2DObject();
		if (pObject==NULL)return false;

		pObject->Destoy();
		m_mapAllObjects.erase(pObject->GetHandle());

		efeDelete(pObject);
		a_pEntity->SetGrid2DObject(NULL);

		return true;
	}

	//--------------------------------------------------------------

	cGrid2D *cGridMap2D::GetAt(int a_lX, int a_lY)
	{
		if (a_lX>=m_vGridNum.x || a_lX<0 || a_lY>=m_vGridNum.y || a_lY<0) return NULL;

		return &m_vGrids[a_lY + a_lY*m_vGridNum.x];
	}

	//--------------------------------------------------------------

	void cGridMap2D::DrawGrid(iLowLevelGraphics *a_pLowLevel, float a_fZ,cColor a_Col)
	{
		for (int x=0;x<m_vGridNum.x;x++)
			for (int y=0;y<m_vGridNum.y;y++)
			{
				//a_pLowLevel-
			}
	}

	//--------------------------------------------------------------

	void cGridMap2D::DrawEntityGrids(iLowLevelGraphics *a_pLowLevel,cVector2f a_vWorldPos, float a_vZ,cColor a_Col)
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// GRIDMAP PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGridMap2DRectIt::cGridMap2DRectIt(cGridMap2D *a_pGridMap, cVector2l a_vPos, cVector2l a_vSize)
	{
		m_pGridMap = a_pGridMap;
		m_vPos = a_vPos;
		m_vSize = a_vSize;
		m_lType = 0;
		m_pObject = NULL;
		m_lGridNum = 0;
		m_lGridAdd = 0;
		m_lGridRowCount = 0;
		m_bUpdated = false;

		m_pGridMap->m_lGlobalCount++;

		m_It = m_pGridMap->m_mapGlobalObjects.begin();
	}

	//--------------------------------------------------------------

	bool cGridMap2DRectIt::HasNext()
	{
		if (!m_bUpdated)
		{
			GetGridObject();
			m_bUpdated = true;
		}

		if(m_pObject) return true;
		else return false;
	}

	//--------------------------------------------------------------

	iEntity2D *cGridMap2DRectIt::Next()
	{
		iEntity2D *pEntity = NULL;
		if (!m_bUpdated)
		{
			GetGridObject();
			m_bUpdated = true;
		}

		if(m_pObject)
		{
			pEntity = m_pObject->GetEntity();
			m_It++;
			m_bUpdated = true;
		}
		return pEntity;
	}

	//--------------------------------------------------------------
	//////////////////////// PRIVATE ////////////////////////////////
	//--------------------------------------------------------------

	void cGridMap2DRectIt::GetGridObject()
	{
		bool bEnd = false;

		switch(m_lType)
		{
		case 0:
			if (m_It == m_pGridMap->m_mapGlobalObjects.end())
			{
				m_lType = 1;

				if (m_vPos.x<0 || m_vPos.y<0 || 
					m_vPos.x+m_vSize.x>m_pGridMap->m_vGridNum.x ||
					m_vPos.y+m_vSize.y>m_pGridMap->m_vGridNum.y)
					m_It = m_pGridMap->m_mapOuterObjects.begin();
				else
					m_It = m_pGridMap->m_mapAllObjects.end();
			}
			else
			{
				m_pObject = m_It->second;
				break;
			}
		case 1:
			{
				if (m_It == m_pGridMap->m_mapOuterObjects.end())
				{
					m_lType = 2;

					if (m_vPos.x>=m_pGridMap->m_vGridNum.x || m_vPos.y>=m_pGridMap->m_vGridNum.y ||
						m_vPos.x+m_vSize.x - 1 < 0 || m_vPos.y+m_vSize.y - 1 < 0)
					{
						m_pObject = NULL;
						m_lType = -1;
						break;
					}

					if (m_vPos.x<0){m_vSize.x+=m_vPos.x; m_vPos.x=0;}
					if (m_vPos.y<0){m_vSize.y+=m_vPos.y; m_vPos.y=0;}

					if (m_vPos.x + m_vSize.x > m_pGridMap->m_vGridNum.x)
						m_vSize.x -= (m_vPos.x + m_vSize.x) - (m_pGridMap->m_vGridNum.x);

					if (m_vPos.y + m_vSize.y > m_pGridMap->m_vGridNum.y)
						m_vSize.y -= (m_vPos.y + m_vSize.y) - (m_pGridMap->m_vGridNum.y);

					m_lGridNum = m_vPos.x + m_vPos.y*m_pGridMap->m_vGridNum.x;
					m_lGridAdd = m_pGridMap->m_vGridNum.x-m_vSize.x;
					m_lGridRowCount = m_vSize.y;
					m_lGridColCount = m_vSize.x;

					if (m_vSize.x<=0 || m_vSize.y<=0)
					{
						m_lGridNum = 0;
						m_It = m_pGridMap->m_vGrids[m_lGridNum].m_mapObjects.end();
						m_lGridColCount = 0;
						m_lGridRowCount = 0;
					}
					else
						m_It = m_pGridMap->m_vGrids[m_lGridNum].m_mapObjects.begin();
				}
				else
				{
					m_pObject = m_It->second;

					m_pObject->FirstTime(m_pGridMap->m_lGlobalCount);
					break;
				}
			}
		case 2:
			{
				while (true)
				{
					if (m_It == m_pGridMap->m_vGrids[m_lGridNum].m_mapObjects.end())
					{
						m_lGridNum++;
						m_lGridColCount--;
						if (m_lGridColCount<=0)
						{
							m_lGridColCount = m_vSize.x;
							m_lGridRowCount--;
							if (m_lGridRowCount<=0)
							{
								m_pObject = NULL;
								break;
							}
							m_lGridNum+=m_lGridAdd;
						}
						m_It = m_pGridMap->m_vGrids[m_lGridNum].m_mapObjects.begin();
					}
					else
					{
						m_pObject = m_It->second;

						if (m_pObject->FirstTime(m_pGridMap->m_lGlobalCount))
							break;
						else
							m_It++;
					}
				}
			}
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// GRIDOBJECT PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGrid2DObject::cGrid2DObject(iEntity2D *a_pEntity, cGridMap2D *a_pGridMap, unsigned int a_lHandle)
	{
		m_pEntity = a_pEntity;
		m_pGridMap = a_pGridMap;

		m_vGridParents.resize(m_pGridMap->GetMaxArraySize());
		for (int i=0;i<(int)m_vGridParents.size();i++)
			m_vGridParents[i] = NULL;

		m_vPosition = cVector2l(-1000);
		m_vGridSpan = cVector2l(-1,-1);
		m_lHandle = a_lHandle;

		m_bIsInOuter = false;
		m_bIsInGlobal = false;
		m_lCount = 0;

		m_pEntity->UpdateBoundingBox();
		Update(m_pEntity->GetBoundingBox());
	}

	//--------------------------------------------------------------

	void cGrid2DObject::Destoy()
	{
		for (int x=0;x<m_vGridSpan.x;x++)
			for (int y=0;y<m_vGridSpan.y;y++)
			{
				int lNum = x + y*m_pGridMap->GetMaxGridSpan().x;
				if (m_vGridParents[lNum])
				{
					m_vGridParents[lNum]->Erase(m_lHandle);
					m_vGridParents[lNum] = NULL;
				}
			}

			if (m_bIsInGlobal)
			{
				m_pGridMap->m_mapGlobalObjects.erase(m_lHandle);
				m_bIsInGlobal = false;
			}
			else if (m_bIsInOuter)
			{
				m_pGridMap->m_mapOuterObjects.erase(m_lHandle);
				m_bIsInOuter = false;
			}
	}

	//--------------------------------------------------------------

	void cGrid2DObject::Update(const cRectf &a_Rect)
	{
		cVector2f vGSize((float)m_pGridMap->GetGridSize().x, (float)m_pGridMap->GetGridSize().y);
		cVector2l vPos((int)floor(a_Rect.x/vGSize.x), (int)floor(a_Rect.y/vGSize.y));

		cVector2l vGridSpan((int)(a_Rect.w/vGSize.x), (int)(a_Rect.h/vGSize.y));
		vGridSpan += 1;

		if (a_Rect.h<0 || a_Rect.w<0 || vGridSpan.x>=m_pGridMap->GetMaxGridSpan().x ||
			vGridSpan.y>=m_pGridMap->GetMaxGridSpan().y)
		{
			if (m_bIsInGlobal==false)
			{
				m_bIsInGlobal = true;

				for (int x=0;x<m_vGridSpan.x;x++)
					for (int y=0;y<m_vGridSpan.y;y++)
					{
						int lNum = x + y*m_pGridMap->GetMaxGridSpan().x;
						if (m_vGridParents[lNum])
						{
							m_vGridParents[lNum]->Erase(m_lHandle);
							m_vGridParents[lNum] = NULL;
						}
					}
					m_pGridMap->m_mapGlobalObjects.insert(tGrid2DObjectMap::value_type(m_lHandle, this));
			}
			return;
		}

		if (a_Rect.x+a_Rect.w>=(vPos.x+vGridSpan.x)*vGSize.x) vGridSpan.x++;
		if (a_Rect.y+a_Rect.h>=(vPos.y+vGridSpan.y)*vGSize.y) vGridSpan.y++;

		if (vPos.x!=m_vPosition.x || vPos.y!=m_vPosition.y ||
			vGridSpan.x!=m_vGridSpan.x || vGridSpan.y!=m_vGridSpan.y)
		{
			for (int x=0;x<m_vGridSpan.x;x++)
				for (int y=0;y<m_vGridSpan.y;y++)
				{
					int lNum = x + y*m_pGridMap->GetMaxGridSpan().x;
						if (m_vGridParents[lNum])
						{
							m_vGridParents[lNum]->Erase(m_lHandle);
							m_vGridParents[lNum] = NULL;
						}
				}

				if (m_bIsInGlobal)
				{
					m_pGridMap->m_mapGlobalObjects.erase(m_lHandle);
					m_bIsInGlobal = false;
				}
				else if (m_bIsInOuter)
				{
					m_pGridMap->m_mapOuterObjects.erase(m_lHandle);
					m_bIsInOuter = false;
				}

				m_vGridSpan = vGridSpan;

				cGrid2D *pGrid = NULL;
				for (int x=0;x<m_vGridSpan.x;x++)
					for (int y=0;y<m_vGridSpan.y;y++)
					{
						pGrid = m_pGridMap->GetAt(vPos.x+x,vPos.y+y);
						if (pGrid==NULL)
						{
							if (!m_bIsInOuter)
							{
								m_pGridMap->m_mapOuterObjects.insert(tGrid2DObjectMap::value_type(m_lHandle,this));

								m_bIsInOuter = true;
							}
						}
						else
						{
							pGrid->Add(this);
							m_vGridParents[x + y*m_pGridMap->GetMaxGridSpan().x] = pGrid;
						}
					}
		}
	}
}