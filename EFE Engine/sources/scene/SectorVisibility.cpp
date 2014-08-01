#include "scene/SectorVisibility.h"

#include "math/Math.h"
#include "scene/PortalContainer.h"

#include "system/LowLevelSystem.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// PORTAL VISIBILITY SET
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPortalVisibility::cPortalVisibility()
	{
	}

	cPortalVisibility::~cPortalVisibility()
	{
	}

	

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PORTAL VISIBILITY SET
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPortalVisibilitySet::cPortalVisibilitySet(cSectorVisibilityContainer *a_pContainer, cPortalVisibilitySet *a_pParent)
	{
		m_pParent = a_pParent;
		m_pContainer = a_pContainer;
	}

	cPortalVisibilitySet::~cPortalVisibilitySet()
	{
		STLDeleteAll(m_vVisibility);
	}

	//--------------------------------------------------------------

	int cPortalVisibilitySet::AddPortalVisibilty(cPortal *a_pPortal)
	{
		m_vVisibility.push_back(efeNew(cPortalVisibility, ()));

		size_t lIdx = m_vVisibility.size()-1;

		m_vVisibility[lIdx]->m_pPortal = a_pPortal;

		cShadowVolumeBV *pShadow = a_pPortal->GetBV()->GetShadowVolume(m_pContainer->GetOrigin(),9999.0f,true);
		if (pShadow)
		{
			m_vVisibility[lIdx]->m_Shadow = *pShadow;
			m_vVisibility[lIdx]->m_bNullShadow = false;
		}
		else
			m_vVisibility[lIdx]->m_bNullShadow = true;

		return (int)lIdx;
	}

	bool cPortalVisibilitySet::PortalExists(cPortal *a_pPortal)
	{
		for (size_t i=0;i<m_vVisibility.size();i++)
			if (m_vVisibility[i]->m_pPortal == a_pPortal) return true;
		return false;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// SECTOR VISIBILITY
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSectorVisibility::cSectorVisibility(cSectorVisibilityContainer *a_pContainer)
	{
		m_pSector = NULL;
		bStart = false;

		m_pContainer = a_pContainer;
	}

	cSectorVisibility::~cSectorVisibility()
	{
		
	}

	//--------------------------------------------------------------

	cPortalVisibilitySet *cSectorVisibility::GetSetConnectingFromSector(cSector *a_pSector)
	{
		for (size_t i=0;i < m_vVisibilitySets.size();++i)
		{
			if (m_vVisibilitySets[i]->GetVisibility(0)->m_pPortal->GetSector() == a_pSector)
				return m_vVisibilitySets[i];
		}

		return NULL;
	}


	//--------------------------------------------------------------

	void cSectorVisibility::AddVisibilitySet(cPortalVisibilitySet *a_pSet)
	{
		m_vVisibilitySets.push_back(a_pSet);
	}

	//--------------------------------------------------------------

	bool cSectorVisibility::PortalExists(cPortal *a_pPortal)
	{
		for (size_t i=0;i<m_vVisibilitySets.size();i++)
			if (m_vVisibilitySets[i]->PortalExists(a_pPortal)) return true;

		return false;
	}

	bool cSectorVisibility::IntersectionBV(cBoundingVolume *a_pBV)
	{
		if (m_vVisibilitySets.empty())
			return m_pContainer->IntersectionBV(a_pBV, NULL);
		else
		{
			for (size_t i=0;i<m_vVisibilitySets.size();++i)
			{
				if (m_pContainer->IntersectionBV(a_pBV, m_vVisibilitySets[i]))
					return true;
			}

			return false;
		}
	}

	cSectorVisibility *cSectorVisibilityContainer::GetSectorVisibility(cSector *a_pSector)
	{
		tSectorVisibilityMapIt it = m_mapSectors.find(a_pSector);

		if (it==m_mapSectors.end())
		{
			if (m_bLog) Log("%sCreating visibility sector for '%s'!\n", GetTabs().c_str(),a_pSector->GetId().c_str());

			cSectorVisibility *pVisSector = efeNew(cSectorVisibility, (this));
			pVisSector->m_pSector = a_pSector;

			m_mapSectors.insert(tSectorVisibilityMapIt::value_type(a_pSector, pVisSector));

			return pVisSector;
		}
		else
		{
			if (m_bLog) Log("%sVisibility sector for '%s' already exists!\n", GetTabs().c_str(), a_pSector->GetId().c_str());
			return it->second;
		}
	}

	tSectorVisibilityIterator cSectorVisibilityContainer::GetSectorIterator()
	{
		return tSectorVisibilityIterator(&m_mapSectors);
	}

	cPortalVisibilitySet *cSectorVisibilityContainer::CreatePortalVisibilitySet(cPortalVisibilitySet *a_pParent)
	{
		cPortalVisibilitySet *pSet = efeNew(cPortalVisibilitySet, (this, a_pParent));

		m_lstPortalVisibility.push_back(pSet);

		return pSet;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// SECTOR VISIBILITY CONTAINER
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSectorVisibilityContainer::cSectorVisibilityContainer(eSectorVisibilityType a_Type)
	{
		m_Type = a_Type;

		m_bLog = true;
		m_lTabs = 0;
	}

	cSectorVisibilityContainer::~cSectorVisibilityContainer()
	{
		STLMapDeleteAll(m_mapSectors);
		STLDeleteAll(m_lstPortalVisibility);
	}

	void cSectorVisibilityContainer::Compute(cPortalContainer *a_pContainer)
	{
		tSectorMap *pSectorMap = a_pContainer->GetSectorMap();

		if (m_bLog) Log("Checking for start sectors\n");
		
		m_setStartSectors.clear();

		if (m_Type == eSectorVisibilityType_BV)				m_vOrigin = m_BoundingVolume.GetPosition();
		else if (m_Type== eSectorVisibilityType_Frustum)	m_vOrigin = m_Frustum.GetOrigin();

		tSectorMapIt it = pSectorMap->begin();
		for (;it != pSectorMap->end(); ++it)
		{
			cSector *pSector = it->second;

			if (cMath::PointBVCollision(m_vOrigin, *pSector->GetBV()))
			{
				if (m_bLog) Log("Sector '%s' is a start!\n", pSector->GetId().c_str());

				m_setStartSectors.insert(pSector);
			}
		}

		tSectorSetIt startIt = m_setStartSectors.begin();
		for (; startIt != m_setStartSectors.end(); ++startIt)
		{
			cSector *pSector = *startIt;
			SearchSector(pSector , NULL, 0);
		}

		if (m_bLog) Log("Done checking start sectors!\n");
	}

	//--------------------------------------------------------------

	void cSectorVisibilityContainer::SearchSector(cSector *a_pSector, cPortalVisibilitySet *a_pParentSet, int a_lPortalIndex)
	{
		if (m_bLog)
		{
			Log("%s--- Searching sector %s\n%s----------------------------------\n",
			GetTabs().c_str(), a_pSector->GetId().c_str(), GetTabs().c_str());
			m_lTabs++;
		}

		cSectorVisibility *pVisSector = GetSectorVisibility(a_pSector);

		tPortalList lstNewPortals;

		tPortalList *pPortalList = NULL;

		if (a_pParentSet)
		{
			if (m_bLog) Log("%sSearching portals from parent portal %d with index %d\n", GetTabs().c_str(), a_pParentSet->GetVisibility(a_lPortalIndex)->m_pPortal->GetId(), a_lPortalIndex);
			pPortalList = a_pParentSet->GetVisibility(a_lPortalIndex)->m_pPortal->GetPortalList();
		}
		else
		{
			if (m_bLog) Log("%sNo parent set, searching all portals.\n", GetTabs().c_str());
			pPortalList = a_pSector->GetPortalList();
		}

		//Iterate all portals
		tPortalListIt it = pPortalList->begin();
		for (;it != pPortalList->end();++it)
		{
			cPortal *pPortal = *it;
			cSector *pTargetSector = pPortal->GetTargetSector();

			//Check if it is a start sector
			if (m_setStartSectors.find(pTargetSector) != m_setStartSectors.end())
				continue;

			if (pVisSector->PortalExists(pPortal)==false &&
				IntersectionBV(pPortal->GetBV(), a_pParentSet) &&
				pPortal->GetActive())
			{
				if (m_bLog) Log("%sFound new valid portal %d\n", GetTabs().c_str(), pPortal->GetId());

				if (cMath::PlaneToPointDist(pPortal->GetPlane(),m_vOrigin) < 0.0)
					continue;

				cSectorVisibility *pTargetVisSector = GetSectorVisibility(pTargetSector);

				cPortalVisibilitySet *pSet = pTargetVisSector->GetSetConnectingFromSector(pTargetSector);

				if (pSet==NULL)
				{
					pSet = CreatePortalVisibilitySet(a_pParentSet);

					pTargetVisSector->AddVisibilitySet(pSet);

					if (m_bLog) Log("%sNo portal connecting from %s to %s, creating new visibility set\n",
																				GetTabs().c_str(),
																				a_pSector->GetId().c_str(),
																				pTargetSector->GetId().c_str());
				}

				int lIdx = pSet->AddPortalVisibilty(pPortal);

				SearchSector(pTargetSector,pSet,lIdx);
			}
			else if (m_bLog) Log("%sSkipped unvalid portal %d\n", GetTabs().c_str(), pPortal->GetId());
		}

		if (m_bLog)
		{
			m_lTabs--;
			Log("%s----------------------------------\n%s--- Done searching sector %s!\n",
																	GetTabs().c_str(),
																	GetTabs().c_str(),
																	a_pSector->GetId().c_str());
		}
	}

	bool cSectorVisibilityContainer::IntersectionBV(cBoundingVolume *a_pBV, cPortalVisibilitySet *pVisibilitySet)
	{
		if (pVisibilitySet)
		{
			cPortalVisibilitySet *pSet = pVisibilitySet;

			while (pVisibilitySet)
			{
				bool bIntersection = false;

				size_t lVisibilityNum = pSet->GetVisibilittyNum();
				for (size_t i=0;i<lVisibilityNum;++i)
				{
					if (pVisibilitySet->GetVisibility(i)->m_bNullShadow ||
						pVisibilitySet->GetVisibility(i)->m_Shadow.CollideBoundingVolume(a_pBV))
					{
						bIntersection = true;
						break;
					}
				}

				if (bIntersection == false) return false;

				pVisibilitySet = pVisibilitySet->GetParent();
			}
		}

		if (m_Type==eSectorVisibilityType_BV)
			return cMath::CheckCollisionBV(m_BoundingVolume, *a_pBV);
		else
			return m_Frustum.CollideBoundingVolume(a_pBV)!=eFrustumCollision_Outside;
	}

	tString cSectorVisibilityContainer::GetTabs()
	{
		tString sTabs = "";
		for (int i=0;i<m_lTabs;++i) sTabs += "	";

		return sTabs;
	}
}