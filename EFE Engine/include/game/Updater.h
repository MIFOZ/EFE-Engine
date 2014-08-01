#ifndef EFE_UPDATER_H
#define EFE_UPDATER_H

#include <map>
#include <list>

#include "system/SystemTypes.h"

namespace efe
{
	class iUpdateable;
	class iLowLevelSystem;

	typedef std::list<iUpdateable*> tUpdateableList;
	typedef tUpdateableList::iterator tUpdateableListIt;

	typedef std::map<tString, tUpdateableList> tUpdateContainerMap;
	typedef tUpdateContainerMap::iterator tUpdateContainerMapIt;

	class cUpdater
	{
	public:
		cUpdater(iLowLevelSystem *a_pLowLevelSystem);
		~cUpdater();

		void Reset();

		void OnDraw();
		void OnPostSceneDraw();
		void OnPostGUIDraw();
		void OnPostBufferSwap();

		void OnStart();

		void Update(float a_fTimeStep);

		void OnExit();

		bool SetContainer(tString a_sContainer);

		tString GetCurrentContainerName();

		bool AddContainer(tString a_sName);

		bool AddUpdate(tString a_sContainer, iUpdateable *a_pUpdate);

		bool AddGlobalUpdate(iUpdateable *a_pUpdate);
	private:
		tString m_sCurrentUpdates;

		tUpdateContainerMap m_mapUpdateContainer;

		iLowLevelSystem *m_pLowLevelSystem;

		tUpdateableList *m_pCurrentUpdates;
		tUpdateableList m_lstGlobalUpdateableList;
	};
};
#endif