#include "scene/PortalContainer.h"

#include "graphics/Renderable.h"
#include "graphics/RenderList.h"
#include "math/Frustum.h"
#include "system/LowLevelSystem.h"
#include "math/Math.h"
#include "scene/Light3D.h"

#include "scene/SectorVisibility.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// ENTITY ITERATOR
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPortalContainerEntityIterator::cPortalContainerEntityIterator(cPortalContainer *a_pContainer,
																	cBoundingVolume *a_pBV)
	{
		m_pContainer = a_pContainer;
		m_pBV = a_pBV;
		m_bGlobal = true;

		m_pSectorMap = &m_pContainer->m_mapSectors;

		m_EntityIt = m_pContainer->m_setGlobalEntities.begin();

		if (m_EntityIt == m_pContainer->m_setGlobalEntities.begin())
			m_bGlobal = true;

		m_SectorIt = m_pContainer->m_mapSectors.begin();
		if (m_SectorIt != m_pContainer->m_mapSectors.end() &&
			((m_SectorIt->second)->m_setEntities.empty() ||
			!cMath::CheckCollisionBV(*m_pBV, (m_SectorIt->second)->m_BV)))
		{
			for (;m_SectorIt != m_pContainer->m_mapSectors.end(); ++m_SectorIt)
			{
				cSector *pSector = m_SectorIt->second;
				if ((m_SectorIt->second)->m_setEntities.empty() == false &&
					cMath::CheckCollisionBV(*m_pBV, pSector->m_BV))
					break;
			}
		}

		if (m_bGlobal == false && m_SectorIt != a_pContainer->m_mapSectors.end())
		{
			m_pEntity3DSet = &(m_SectorIt->second)->m_setEntities;
			m_EntityIt = m_pEntity3DSet->begin();
		}

		++m_pContainer->m_lSectorVisitCount;
		m_lIteratorCount = m_pContainer->m_lSectorVisitCount;
	}

	//--------------------------------------------------------------

	bool cPortalContainerEntityIterator::HasNext()
	{
		if (m_bGlobal == false && m_SectorIt == m_pContainer->m_mapSectors.end()) return false;

		return true;
	}

	//--------------------------------------------------------------

	iEntity3D *cPortalContainerEntityIterator::Next()
	{
		iEntity3D *pEntity = *m_EntityIt;
		pEntity->SetIteratorCount(m_lIteratorCount);

		++m_EntityIt;

		bool bNextEntity = false;
		do
		{
			//Search Global
			if (m_bGlobal)
			{
				if (m_EntityIt == m_pContainer->m_setGlobalEntities.end())
				{
					m_bGlobal = false;
					if (m_SectorIt == m_pContainer->m_mapSectors.end()) return pEntity;

					m_pEntity3DSet = &(m_SectorIt->second)->m_setEntities;
					m_EntityIt = m_pEntity3DSet->begin();
				}
			}
			//Search Sectors
			else
			{
				if (m_EntityIt == m_pEntity3DSet->end())
				{
					++m_EntityIt;
					if (m_SectorIt != m_pContainer->m_mapSectors.end() &&
						((m_SectorIt->second)->m_setEntities.empty() ||
						!cMath::CheckCollisionBV(*m_pBV,(m_SectorIt->second)->m_BV)))
					{
						for (; m_SectorIt != m_pContainer->m_mapSectors.end(); ++m_SectorIt)
						{
							cSector *pSector = m_SectorIt->second;
							if (pSector->m_setEntities.empty() == false &&
								cMath::CheckCollisionBV(*m_pBV,pSector->m_BV))
							{
								break;
							}
						}
					}

					if (m_SectorIt != m_pContainer->m_mapSectors.end())
					{
						m_pEntity3DSet = &(m_SectorIt->second)->m_setEntities;
						m_EntityIt = m_pEntity3DSet->begin();
					}
				}
			}

			bNextEntity = true;
			if (m_bGlobal == false && m_SectorIt == m_pContainer->m_mapSectors.end()) bNextEntity = false;
			else if ((*m_EntityIt)->GetIteratorCount() != m_lIteratorCount) bNextEntity = false;

			if (bNextEntity) ++m_EntityIt;
		}
		while (bNextEntity);

		return pEntity;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PORTAL
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPortal::cPortal(int a_lId, cPortalContainer *a_pContainer)
	{
		m_lId = a_lId;
		m_pContainer = a_pContainer;

		m_pTargetSector = NULL;

		m_bPortalsNeedUpdate = true;

		m_bActive = true;
	}

	cPortal::~cPortal()
	{
		
	}

	cSector *cPortal::GetTargetSector()
	{
		if (m_pTargetSector==NULL)
		{
			m_pTargetSector = m_pContainer->GetSector(m_sTargetSectorId);

			if (m_pTargetSector == NULL)
				Error("Portal %d in sector %s target sector %s is NOT valid!\n", m_lId, m_sSectorId.c_str(),
																				m_sTargetSectorId.c_str());
		}

		return m_pTargetSector;
	}

	cSector *cPortal::GetSector()
	{
		return m_pSector;
	}

	tPortalList *cPortal::GetPortalList()
	{
		if (m_bPortalsNeedUpdate)
		{
			m_bPortalsNeedUpdate = false;

			for (size_t i=0;i<m_vPortalIds.size();i++)
			{
				cPortal *pPortal = GetTargetSector()->GetPortal(m_vPortalIds[i]);

				if (pPortal) m_lstPortals.push_back(pPortal);
			}
		}
		return &m_lstPortals;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// SECTOR
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSector::cSector(tString a_sId, cPortalContainer *a_pContainer)
	{
		m_sId = a_sId;

		m_pContainer = a_pContainer;

		m_BV.SetPosition(0);
		m_BV.SetLocalMinMax(cVector3f(100000,100000,100000),cVector3f(-100000,-100000,-10000));

		m_lVisitCount = -1;
		m_Ambient = cColor(1,1);
	}

	//--------------------------------------------------------------

	cSector::~cSector()
	{
		STLDeleteAll(m_lstPortals);
	}

	//--------------------------------------------------------------

	cPortal *cSector::GetPortal(int a_lId)
	{
		tPortalListIt it = m_lstPortals.begin();
		for (;it != m_lstPortals.end();++it)
		{
			cPortal *pPortal = *it;
			if (pPortal->m_lId = a_lId) return pPortal;
		}
		return NULL;
	}

	bool cSector::TryToAdd(iRenderable *a_pObject, bool a_bStatic)
	{
		if (a_pObject->CollidesWithBV(&m_BV))
		{
			if (a_bStatic)
			{
				a_pObject->GetRenderContainerDataList()->push_back(this);
				m_setStaticObjects.insert(a_pObject);
				a_pObject->GetRenderContainerDataList()->push_back(this);
			}
			else
			{
				a_pObject->GetRenderContainerDataList()->push_back(this);
				m_setDynamicObjects.insert(a_pObject);
			}
			return true;
		}
		return false;
	}

	bool cSector::TryToAddEntity(iEntity3D *a_pEntity)
	{
		if (cMath::CheckCollisionBV(*a_pEntity->GetBoundingVolume(), m_BV))
		{
			a_pEntity->GetRenderContainerDataList()->push_back(this);

			m_setEntities.insert(a_pEntity);
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------

	void cSector::RemoveDynamic(iRenderable *a_pObject)
	{
		m_setDynamicObjects.erase(a_pObject);
	}

	//--------------------------------------------------------------

	void cSector::RemoveEntity(iEntity3D *a_pEntity)
	{
		m_setEntities.erase(a_pEntity);
	}

	//--------------------------------------------------------------

	bool g_bCallbackActive = true;

	//////////////////////////////////////////////////////////////
	// PORTAL CONTAINER ENTITY CALLBACK
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPortalContainerEntityCallback::cPortalContainerEntityCallback(cPortalContainer *a_pContainer)
	{
		m_pContainer = a_pContainer;
	}

	//--------------------------------------------------------------

	void cPortalContainerEntityCallback::OnTransformUpdate(iEntity3D *a_pEntity)
	{
		if (!g_bCallbackActive) return;

		tRenderContainerDataList *pDataList = a_pEntity->GetRenderContainerDataList();

		if (pDataList->empty())
			m_pContainer->m_setGlobalEntities.erase(a_pEntity);
		else
		{
			tRenderContainerDataListIt it = pDataList->begin();
			for (;it != pDataList->end(); ++it)
			{
				cSector *pSector = static_cast<cSector*>(*it);
				pSector->RemoveEntity(a_pEntity);
			}

			pDataList->clear();
		}

		bool bAdded = false;

		tSectorMapIt it = m_pContainer->m_mapSectors.begin();
		for (; it != m_pContainer->m_mapSectors.end(); ++it)
		{
			cSector *pSector = it->second;
			if (pSector->TryToAddEntity(a_pEntity))
				bAdded = true;
		}

		if (!bAdded)
			m_pContainer->m_setGlobalEntities.insert(a_pEntity);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PORTAL CONTAINER CALLBACK
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPortalContainerCallback::cPortalContainerCallback(cPortalContainer *a_pContainer)
	{
		m_pContainer = a_pContainer;
	}

	void cPortalContainerCallback::OnTransformUpdate(iEntity3D *a_pEntity)
	{
		if (!g_bCallbackActive) return;


	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPortalContainer::cPortalContainer()
	{
		m_pEntityCallback = efeNew(cPortalContainerCallback, (this));
		m_pNormalEntityCallback = efeNew(cPortalContainerEntityCallback, (this));

		m_lSectorVisitCount = 0;

		m_lEntityIterateCount = 0;
	}

	//--------------------------------------------------------------

	cPortalContainer::~cPortalContainer()
	{
		efeDelete(m_pEntityCallback);
		efeDelete(m_pNormalEntityCallback);

		STLMapDeleteAll(m_mapSectors);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cPortalContainer::Add(iRenderable *a_pRenderable, bool a_bStatic)
	{
		if (a_pRenderable==NULL)
		{
			Warning("Trayan' to add NULL object to portal container!");
			return false;
		}

		bool bLog = false;

		if (!bLog) Log("-------------\n");
		if (!bLog) Log("Adding in portal container: %s\n", a_pRenderable->GetName().c_str());

		bool bAdded = false;
		if (a_bStatic)
		{
			a_pRenderable->SetCurrentSector(NULL);
			cVector3f vEntityWorldPos = a_pRenderable->GetBoundingVolume()->GetWorldCenter();
			bool bFoundCenter = false;

			tSectorMapIt it = m_mapSectors.begin();
			for (;it != m_mapSectors.end(); ++it)
			{
				cSector *pSector = it->second;

				if (pSector->TryToAdd(a_pRenderable, true))
					bAdded = true;
				
				if (!bFoundCenter)
				{
					if (cMath::PointBVCollision(vEntityWorldPos, *pSector->GetBV()))
					{
						a_pRenderable->SetCurrentSector(pSector);
						bFoundCenter = true;
					}
					else if (cMath::CheckCollisionBV(*a_pRenderable->GetBoundingVolume(), *pSector->GetBV()))
						a_pRenderable->SetCurrentSector(pSector);
				}
			}

			if (!bAdded)
				m_lstGlobalStaticObjects.push_back(a_pRenderable);
		}
		else
		{
			a_pRenderable->SetCurrentSector(NULL);
			cVector3f vEntityWorldPos = a_pRenderable->GetBoundingVolume()->GetWorldCenter();
			bool bFoundCenter = false;

			if (!m_mapSectors.empty())
				a_pRenderable->AddCallback(m_pEntityCallback);

			tSectorMapIt it = m_mapSectors.begin();
			for (;it != m_mapSectors.end(); ++it)
			{
				cSector *pSector = it->second;

				if (pSector->TryToAdd(a_pRenderable, true))
					bAdded = true;
				
				if (!bFoundCenter)
				{
					if (cMath::PointBVCollision(vEntityWorldPos, *pSector->GetBV()))
					{
						a_pRenderable->SetCurrentSector(pSector);
						bFoundCenter = true;
					}
					else if (cMath::CheckCollisionBV(*a_pRenderable->GetBoundingVolume(), *pSector->GetBV()))
						a_pRenderable->SetCurrentSector(pSector);
				}
			}

			if (!bAdded)
				m_setGlobalDynamicObjects.insert(a_pRenderable);
		}
		return true;
	}

	//--------------------------------------------------------------

	bool cPortalContainer::Remove(iRenderable *a_pRenderable)
	{
		tRenderContainerDataList *pDataList = a_pRenderable->GetRenderContainerDataList();

		if (pDataList->empty())
			m_setGlobalDynamicObjects.erase(a_pRenderable);
		else
		{
			tRenderContainerDataListIt it = pDataList->begin();
			for (;it != pDataList->end(); ++it)
			{
				cSector *pSector = static_cast<cSector*>(*it);
				pSector->RemoveDynamic(a_pRenderable);
			}
			pDataList->clear();
		}
		return true;
	}

	//--------------------------------------------------------------

	bool cPortalContainer::AddEntity(iEntity3D *a_pEntity)
	{
		if (a_pEntity == NULL)
		{
			Warning("Trying to add NULL onbject to portal container!");
			return false;
		}

		bool bLog = false;

		bool bAdded = false;

		if (m_mapSectors.empty() == false)
			a_pEntity->AddCallback(m_pNormalEntityCallback);

		tSectorMapIt it = m_mapSectors.begin();
		for (;it != m_mapSectors.end(); it++)
		{
			cSector *pSector = it->second;

			if (pSector->TryToAddEntity(a_pEntity))
				bAdded = true;
		}

		if (bAdded == false)
			m_setGlobalEntities.insert(a_pEntity);
		return true;
	}

	bool cPortalContainer::RemoveEntity(iEntity3D *a_pEntity)
	{
		tRenderContainerDataList *pDataList = a_pEntity->GetRenderContainerDataList();

		if (pDataList->empty())
			m_setGlobalEntities.erase(a_pEntity);
		else
		{
			tRenderContainerDataListIt it = pDataList->begin();
			for (;it != pDataList->end(); ++it)
			{
				cSector *pSector = static_cast<cSector*>(*it);
				pSector->RemoveEntity(a_pEntity);
			}
			pDataList->clear();
		}
		return true;
	}

	void cPortalContainer::AddToRenderList(iRenderable *a_pObject, cFrustum *a_pFrustum,
											cRenderList *a_pRenderList)
	{
		if (a_pRenderList->Add(a_pObject))
		{
			if (a_pObject->GetRenderType() == eRenderableType_Light)
			{

			}
		}
	}

	//--------------------------------------------------------------

	void cPortalContainer::GetVisible(cFrustum *a_pFrustum, cRenderList *a_pRenderList)
	{
		g_bCallbackActive = false;

		m_lstVisibleSectors.clear();

		cSectorVisibilityContainer *pVisSectorCont = CreateVisibilityFromFrustum(a_pFrustum);

		tSectorVisibilityIterator SectorIt = pVisSectorCont->GetSectorIterator();
		while (SectorIt.HasNext())
		{
			cSectorVisibility *pVisSector = SectorIt.Next();
			cSector *pSector = pVisSector->GetSector();

			m_lstVisibleSectors.push_back(pSector->GetId());

			tRenderableSetIt it = pSector->m_setStaticObjects.begin();
			for (; it != pSector->m_setStaticObjects.end(); ++it)
			{
				iRenderable *pObject = *it;

				if (pVisSector->IntersectionBV(pObject->GetBoundingVolume()))
					AddToRenderList(pObject, a_pFrustum, a_pRenderList);
			}

			it = pSector->m_setDynamicObjects.begin();
			{
				iRenderable *pObject = *it;

				if (pVisSector->IntersectionBV(pObject->GetBoundingVolume()))
					AddToRenderList(pObject, a_pFrustum, a_pRenderList);
			}

		}

		{
			tRenderableSetIt it = m_setGlobalDynamicObjects.begin();
			for (; it!=m_setGlobalDynamicObjects.end(); ++it)
			{
				iRenderable *pObject = *it;

				if (pObject->CollidesWithFrustum(a_pFrustum))
					AddToRenderList(pObject, a_pFrustum, a_pRenderList);
			}
		}

		{
			tRenderableListIt it = m_lstGlobalStaticObjects.begin();
			for (; it!=m_lstGlobalStaticObjects.end(); ++it)
			{
				iRenderable *pObject = *it;

				if (pObject->CollidesWithFrustum(a_pFrustum))
					AddToRenderList(pObject, a_pFrustum, a_pRenderList);
			}
		}

		efeDelete(pVisSectorCont);

		g_bCallbackActive = true;
	}

	//--------------------------------------------------------------

	void cPortalContainer::Compile()
	{
		tEntity3DSet setEntities;

		//Sectors
		tSectorMapIt SecIt = m_mapSectors.begin();
		for (; SecIt != m_mapSectors.end(); SecIt++)
		{
			cSector *pSector = SecIt->second;

			tEntity3DSetIt EntIt = pSector->m_setEntities.begin();
			for (; EntIt != pSector->m_setEntities.end(); ++EntIt)
			{
				iEntity3D *pEntity = *EntIt;
				setEntities.insert(pEntity);
			}
		}

		//Global
		tEntity3DSetIt EntIt = m_setGlobalEntities.begin();
		for (; EntIt != m_setGlobalEntities.end(); ++EntIt)
		{
			iEntity3D *pEntity = *EntIt;
			setEntities.insert(pEntity);
		}

		EntIt = setEntities.begin();
		for (; EntIt != setEntities.end(); ++EntIt)
		{
			iEntity3D *pEntity = *EntIt;
			m_pNormalEntityCallback->OnTransformUpdate(pEntity);
		}
	}

	//--------------------------------------------------------------

	cSector *cPortalContainer::GetSector(tString a_sId)
	{
		tSectorMapIt it = m_mapSectors.find(a_sId);
		if (it==m_mapSectors.end()) return NULL;

		return it->second;
	}

	//--------------------------------------------------------------

	cPortalContainerEntityIterator cPortalContainer::GetEntityIterator(cBoundingVolume *a_pBV)
	{
		return cPortalContainerEntityIterator(this, a_pBV);
	}

	//--------------------------------------------------------------

	cSectorVisibilityContainer *cPortalContainer::CreateVisibilityFromBV(cBoundingVolume *a_pBV)
	{
		cSectorVisibilityContainer *pContainer = efeNew(cSectorVisibilityContainer, (eSectorVisibilityType_BV));
		pContainer->SetBV(*a_pBV);

		pContainer->Compute(this);

		return pContainer;
	}

	//--------------------------------------------------------------

	cSectorVisibilityContainer *cPortalContainer::CreateVisibilityFromFrustum(cFrustum *a_pFrustum)
	{
		cSectorVisibilityContainer *pContainer = efeNew(cSectorVisibilityContainer, (eSectorVisibilityType_Frustum));
		pContainer->SetFrustum(*a_pFrustum);

		pContainer->Compute(this);

		return pContainer;
	}
}