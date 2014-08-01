#ifndef EFE_SECTOR_VISIBILITY_H
#define EFE_SECTOR_VISIBILITY_H

#include "math/BoundingVolume.h"
#include "math/Frustum.h"

namespace efe
{
	class cPortal;
	class cSector;
	class cPortalContainer;
	class cSectorVisibilityContainer;

	enum eSectorVisibilityType
	{
		eSectorVisibilityType_BV,
		eSectorVisibilityType_Frustum
	};

	//--------------------------------------------------------------

	class cPortalVisibility
	{
	public:
		cPortalVisibility();
		~cPortalVisibility();

		cShadowVolumeBV m_Shadow;
		cPortal *m_pPortal;
		bool m_bNullShadow;
	};

	//--------------------------------------------------------------

	class cPortalVisibilitySet
	{
		friend class cSectorVisibilityContainer;
	public:
		cPortalVisibilitySet(cSectorVisibilityContainer *a_pContainer, cPortalVisibilitySet *a_pParent);
		~cPortalVisibilitySet();

		cPortalVisibilitySet *GetParent(){return m_pParent;}

		__forceinline cPortalVisibility *GetVisibility(size_t a_lIdx){return m_vVisibility[a_lIdx];}
		__forceinline size_t GetVisibilittyNum(){return m_vVisibility.size();}

		bool PortalExists(cPortal *a_pPortal);

		int AddPortalVisibilty(cPortal *a_pPortal);
	private:
		std::vector<cPortalVisibility*> m_vVisibility;
		cPortalVisibilitySet *m_pParent;

		cSectorVisibilityContainer *m_pContainer;
	};

	//--------------------------------------------------------------

	class cSectorVisibility
	{
		friend class cSectorVisibilityContainer;
	public:
		cSectorVisibility(cSectorVisibilityContainer *a_pContainer);
		~cSectorVisibility();

		bool PortalExists(cPortal *a_pPortal);

		cPortalVisibilitySet *GetSetConnectingFromSector(cSector *a_pSector);

		void AddVisibilitySet(cPortalVisibilitySet *a_pSet);

		cSector *GetSector() const {return m_pSector;}

		bool IntersectionBV(cBoundingVolume *a_pBV);
	private:
		cSector *m_pSector;
		std::vector<cPortalVisibilitySet*> m_vVisibilitySets;
		bool bStart;

		cSectorVisibilityContainer *m_pContainer;
	};

	typedef std::map<cSector*, cSectorVisibility*> tSectorVisibilityMap;
	typedef tSectorVisibilityMap::iterator tSectorVisibilityMapIt;

	//--------------------------------------------------------------

	typedef std::list<cPortalVisibilitySet*> tPortalVisibilitySetList;
	typedef tPortalVisibilitySetList::iterator tPortalVisibilitySetListIt;

	typedef std::set<cSector*> tSectorSet;
	typedef tSectorSet::iterator tSectorSetIt;

	typedef cSTLMapIterator<cSectorVisibility*, tSectorVisibilityMap, tSectorVisibilityMapIt> tSectorVisibilityIterator;

	class cSectorVisibilityContainer
	{
	public:
		cSectorVisibilityContainer(eSectorVisibilityType a_Type);
		~cSectorVisibilityContainer();

		void SetFrustum(const cFrustum &a_Frustum){m_Frustum = a_Frustum;}
		void SetBV(const cBoundingVolume &a_BV){m_BoundingVolume = a_BV;}

		const cVector3f &GetOrigin(){return m_vOrigin;}

		void Compute(cPortalContainer *a_pContainer);

		cPortalVisibilitySet *CreatePortalVisibilitySet(cPortalVisibilitySet *a_pParent);

		cSectorVisibility *GetSectorVisibility(cSector *a_pSector);

		tSectorVisibilityIterator GetSectorIterator();

		bool IntersectionBV(cBoundingVolume *a_pBV, cPortalVisibilitySet *pVisibilitySet);

		bool m_bLog;
	private:
		void SearchSector(cSector *a_pSector, cPortalVisibilitySet *a_pParentSet, int a_lPortalIndex);

		tString GetTabs();

		cPortalContainer *m_pContainer;

		tSectorSet m_setStartSectors;

		tSectorVisibilityMap m_mapSectors;
		eSectorVisibilityType m_Type;

		tPortalVisibilitySetList m_lstPortalVisibility;

		cVector3f m_vOrigin;
		cBoundingVolume m_BoundingVolume;
		cFrustum m_Frustum;

		int m_lTabs;
	};
};
#endif