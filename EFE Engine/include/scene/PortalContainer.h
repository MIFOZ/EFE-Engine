#ifndef EFE_PORTAL_CONTAINER_H
#define EFE_PORTAL_CONTAINER_H

#include <list>
#include <set>
#include <map>
#include "scene/RenderableContainer.h"
#include "math/BoundingVolume.h"
#include "graphics/Renderable.h"

namespace efe
{
	class iLight3D;
	class cSectorVisibilityContainer;

	typedef std::list<iRenderable*> tRenderableList;
	typedef tRenderableList::iterator tRenderableListIt;

	typedef std::set<iRenderable*> tRenderableSet;
	typedef tRenderableSet::iterator tRenderableSetIt;

	typedef std::set<iEntity3D*> tEntity3DSet;
	typedef tEntity3DSet::iterator tEntity3DSetIt;

	//--------------------------------------------------------------

	class cPortalContainer;
	class cSector;

	//--------------------------------------------------------------

	typedef std::map<tString, cSector*> tSectorMap;
	typedef tSectorMap::iterator tSectorMapIt;

	//--------------------------------------------------------------

	class cPortalContainerEntityIterator
	{
	public:
		cPortalContainerEntityIterator(cPortalContainer *a_pContainer,
										cBoundingVolume *a_pBV);

		bool HasNext();

		iEntity3D *Next();
	private:
		cPortalContainer *m_pContainer;
		cBoundingVolume *m_pBV;

		tSectorMap *m_pSectorMap;
		tEntity3DSet *m_pEntity3DSet;

		tEntity3DSetIt m_EntityIt;
		tSectorMapIt m_SectorIt;

		tEntity3DSet m_IteratedSet;

		int m_lIteratorCount;

		bool m_bGlobal;
	};

	//--------------------------------------------------------------

	class cPortalContainerCallback : public iEntityCallback
	{
	public:
		cPortalContainerCallback(cPortalContainer *a_pContainer);

		void OnTransformUpdate(iEntity3D *a_pEntity);
	private:
		cPortalContainer *m_pContainer;
	};

	//--------------------------------------------------------------

	class cPortalContainerEntityCallback : public iEntityCallback
	{
	public:
		cPortalContainerEntityCallback(cPortalContainer *a_pContainer);

		void OnTransformUpdate(iEntity3D *a_pEntity);
	private:
		cPortalContainer *m_pContainer;
	};

	class cPortal;

	typedef std::map<int, cPortal*> tPortalMap;
	typedef tPortalMap::iterator tPortalMapIt;

	typedef std::list<cPortal*> tPortalList;
	typedef tPortalList::iterator tPortalListIt;

	class cPortal
	{
		friend class cSector;
	public:
		cPortal(int a_lId, cPortalContainer *a_pContainer);
		~cPortal();

		void SetTargetSector(tString a_sSectorId);
		cSector *GetTargetSector();

		cSector *GetSector();

		tPortalList *GetPortalList();

		//Debug stuffs
		cBoundingVolume *GetBV(){return &m_BV;}
		int GetId(){return m_lId;}
		cPlanef &GetPlane(){return m_Plane;}

		bool GetActive(){return m_bActive;}
		void SetActivee(bool a_bX){m_bActive = a_bX;}
	private:
		cPortalContainer *m_pContainer;

		int m_lId;
		tString m_sSectorId;

		tString m_sTargetSectorId;
		cSector *m_pTargetSector;
		cSector *m_pSector;

		tIntVec m_vPortalIds;
		tPortalList m_lstPortals;
		bool m_bPortalsNeedUpdate;

		bool m_bActive;

		cVector3f m_vNormal;
		cPlanef m_Plane;
		cBoundingVolume m_BV;
		tVector3fList m_lstPoints;
	};

	class cSector : public iRenderContainerData
	{
		friend class cPortalContainer;
		friend class cPortalContainerEntityIterator;
	public:
		cSector(tString a_sId, cPortalContainer *a_pContainer);
		~cSector();

		bool TryToAdd(iRenderable *a_pObject, bool a_bStatic);
		bool TryToAddEntity(iEntity3D *a_pEntity);

		void AddPortal(cPortal *a_pPortal);

		void GetVisible(cFrustum *a_pFrusutm, cRenderList *a_pRenderList, cPortal *a_pStartPortal);

		void RemoveDynamic(iRenderable *a_pObject);
		void RemoveEntity(iEntity3D *a_pEntity);

		cPortal *GetPortal(int a_lId);

		//Debug stuffs
		cBoundingVolume *GetBV(){return &m_BV;}
		tPortalList *GetPortalList(){return &m_lstPortals;}
		tString &GetId(){return m_sId;}
	private:
		cPortalContainer *m_pContainer;

		tString m_sId;
		cBoundingVolume m_BV;

		int m_lVisitCount;

		tRenderableSet m_setStaticObjects;
		tRenderableSet m_setDynamicObjects;

		tEntity3DSet m_setEntities;

		tPortalList m_lstPortals;

		cColor m_Ambient;
	};

	class cPortalContainer : public iRenderableContainer
	{
		friend class cPortalContainerCallback;
		friend class cPortalContainerEntityCallback;
		friend class cPortalContainerEntityIterator;
	public:
		cPortalContainer();
		~cPortalContainer();

		bool AddEntity(iEntity3D *a_pEntity);
		bool RemoveEntity(iEntity3D *a_pEntity);

		bool Add(iRenderable *a_pRenderable, bool a_bStatic);
		bool Remove(iRenderable *a_pRenderable);

		void AddLightShadowCasters(iLight3D *a_pLight, cFrustum *a_pFrustum, cRenderList *a_pRenderList);

		void AddToRenderList(iRenderable *a_pObject, cFrustum *a_pFrustum, cRenderList *a_pRenderList);

		void GetVisible(cFrustum *a_pFrustum, cRenderList *a_pRenderList);

		void Compile();

		cSector *GetSector(tString a_sId);

		cPortalContainerEntityIterator GetEntityIterator(cBoundingVolume *a_pBV);

		//Visibility tools
		cSectorVisibilityContainer *CreateVisibilityFromBV(cBoundingVolume *a_pBV);
		cSectorVisibilityContainer *CreateVisibilityFromFrustum(cFrustum *a_pFrustum);

		//Debug stuuff
		tSectorMap *GetSectorMap(){return &m_mapSectors;}
		tStringList *GetVisibilitySectorList(){return &m_lstVisibleSectors;}

	private:
		void ComputeSectorVisibility(cSectorVisibilityContainer *a_pContainer);

		tSectorMap m_mapSectors;

		int m_lSectorVisitCount;

		cPortalContainerCallback *m_pEntityCallback;
		cPortalContainerEntityCallback *m_pNormalEntityCallback;

		tRenderableSet m_setGlobalDynamicObjects;
		tRenderableList m_lstGlobalStaticObjects;

		tEntity3DSet m_setGlobalEntities;

		tStringList m_lstVisibleSectors;

		int m_lEntityIterateCount;
	};
};
#endif