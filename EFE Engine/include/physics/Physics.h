#ifndef EFE_PHYSICS_H
#define EFE_PHYSICS_H

#include <list>
#include <map>
#include "game/Updateable.h"
#include "system/SystemTypes.h"
#include "physics/PhysicsMaterial.h"

namespace efe
{
	class iLowLevelPhysics;
	class iPhysicsWorld;
	class cSurfaceData;
	class cWorld3D;
	class cResources;
	class cHapticSurface;
	class cHaptic;

	typedef std::list<iPhysicsWorld*> tPhysicsWorldList;
	typedef tPhysicsWorldList::iterator tPhysicsWorldListIt;

	typedef std::map<tString, cSurfaceData*> tSurfaceDataMap;
	typedef tSurfaceDataMap::iterator tSurfaceDataMapIt;

	class cPhysicsImpactCount
	{
	public:
		cPhysicsImpactCount(){m_fCount = 0;}

		float m_fCount;
	};

	typedef std::list<cPhysicsImpactCount> tPhysicsImpactCountList;
	typedef tPhysicsImpactCountList::iterator tPhysicsImpactCountListIt;

	class cPhysics : public iUpdateable
	{
	public:
		cPhysics(iLowLevelPhysics *a_pLowLevelPhysics);
		~cPhysics();

		void Init(cResources *a_pResources);

		void Update(float a_fTimeStep);

		iPhysicsWorld *CreateWorld(bool a_bAddSurfaceData);
		void DestroyWorld(iPhysicsWorld *a_pWorld);

		cSurfaceData *CreateSurfaceData(const tString &a_sName);
		cSurfaceData *GetSurfaceData(const tString &a_sName);
		bool LoadSurfaceData(const tString &a_sFile, cHaptic *a_pHaptic = NULL);

		iLowLevelPhysics *GetLowLevel() {return m_pLowLevelPhysics;}

		void SetGameWorld(cWorld3D *a_pWorld) {m_pGameWorld = a_pWorld;}
		cWorld3D *GetGameWorld() {return m_pGameWorld;}

		void SetImpactDuration(float a_fX) {m_fImpactDuration = a_fX;}
		float GetImpactDuration() {return m_fImpactDuration;}

		void SetMaxImpacts(int a_lX) {m_lMaxImpacts = a_lX;}
		int GetMaxImpacts() {return m_lMaxImpacts;}
		int GetNumOfImpacts() {return (int)m_lstImpactCounts.size();}

		bool CanPlayImpact();
		void AddImpact();

		void SetDebugLog(bool a_bX) {m_bLog = a_bX;}
		bool GetDebugLog() {return m_bLog;}

	private:
		ePhysicsMaterialCombMode GetCombMode(const char *a_pName);

		void UpdateImpactCounts(float a_fTimeStep);

		iLowLevelPhysics *m_pLowLevelPhysics;
		cResources *m_pResources;

		cWorld3D *m_pGameWorld;

		tPhysicsWorldList m_lstWorlds;
		tSurfaceDataMap m_mapSurfaceData;

		tPhysicsImpactCountList m_lstImpactCounts;
		float m_fImpactDuration;
		int m_lMaxImpacts;
		bool m_bLog;
	};
};

#endif