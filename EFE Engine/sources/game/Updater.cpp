#include "game/Updater.h"

#include "game/Updateable.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	cUpdater::cUpdater(iLowLevelSystem *a_pLowLevelSystem)
	{
		m_pCurrentUpdates = NULL;

		m_pLowLevelSystem = a_pLowLevelSystem;
	}

	cUpdater::~cUpdater()
	{
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cUpdater::OnDraw()
	{
		for (tUpdateableListIt it =  m_lstGlobalUpdateableList.begin();it!= m_lstGlobalUpdateableList.end();++it)
			(*it)->OnDraw();
		
		if(m_pCurrentUpdates)
		{
			for (tUpdateableListIt it =  m_pCurrentUpdates->begin();it!= m_pCurrentUpdates->end();++it)
				(*it)->OnDraw();
		}
	}

	//--------------------------------------------------------------

	void cUpdater::OnPostSceneDraw()
	{
		for (tUpdateableListIt it = m_lstGlobalUpdateableList.begin();it!=m_lstGlobalUpdateableList.end();it++)
			(*it)->OnPostSceneDraw();

		if (m_pCurrentUpdates)
		{
			for (tUpdateableListIt it = m_pCurrentUpdates->begin();it!=m_pCurrentUpdates->end();it++)
				(*it)->OnPostSceneDraw();
		}
	}

	void cUpdater::OnPostGUIDraw()
	{
		for (tUpdateableListIt it = m_lstGlobalUpdateableList.begin();it!=m_lstGlobalUpdateableList.end();it++)
			(*it)->OnPostGUIDraw();

		if (m_pCurrentUpdates)
		{
			for (tUpdateableListIt it = m_pCurrentUpdates->begin();it!=m_pCurrentUpdates->end();it++)
				(*it)->OnPostGUIDraw();
		}
	}

	//--------------------------------------------------------------

	void cUpdater::OnStart()
	{
		for (tUpdateableListIt it =  m_lstGlobalUpdateableList.begin();it!= m_lstGlobalUpdateableList.end();it++)
			(*it)->OnStart();

		tUpdateContainerMapIt ContIt = m_mapUpdateContainer.begin();
		while (ContIt != m_mapUpdateContainer.end())
		{
			tUpdateableListIt UpIt = ContIt->second.begin();
			while (UpIt != ContIt->second.end())
			{
				(*UpIt)->OnStart();
				UpIt++;
			}

			ContIt++;
		}
	}

	//--------------------------------------------------------------

	void cUpdater::OnExit()
	{
		for (tUpdateableListIt it = m_lstGlobalUpdateableList.begin();it!=m_lstGlobalUpdateableList.end();++it)
			(*it)->Reset();

		tUpdateContainerMapIt ContIt = m_mapUpdateContainer.begin();
		while (ContIt != m_mapUpdateContainer.end())
		{
			tUpdateableList *pUpdates = &ContIt->second;
			tUpdateableListIt UpIt = pUpdates->begin();
			while (UpIt != pUpdates->end())
			{
				iUpdateable *pUpdate = *UpIt;

				pUpdate->Reset();

				UpIt++;
			}

			ContIt++;
		}
	}

	void cUpdater::Update(float a_fTimeStep)
	{
		for (tUpdateableListIt it = m_lstGlobalUpdateableList.begin();it != m_lstGlobalUpdateableList.end(); it++)
		{
			START_TIMING_EX((*it)->GetName().c_str(),game)
			(*it)->Update(a_fTimeStep);
			STOP_TIMING(game)
		}

		if (m_pCurrentUpdates)
		{
			tUpdateableList *pList = m_pCurrentUpdates;
			for (tUpdateableListIt it = pList->begin(); it!=pList->end();++it)
			{
				START_TIMING_EX((*it)->GetName().c_str(),game)
				(*it)->Update(a_fTimeStep);
				STOP_TIMING(game)
			}
		}
	}

	bool cUpdater::SetContainer(tString a_sContainer)
	{
		tUpdateContainerMapIt it = m_mapUpdateContainer.find(a_sContainer);
		if (it == m_mapUpdateContainer.end()) return false;

		m_sCurrentUpdates = a_sContainer;
		if (m_sCurrentUpdates == "Default")
			SetUpdateLogActive(true);
		else
			SetUpdateLogActive(false);

		m_pCurrentUpdates = &it->second;

		return true;
	}

	tString cUpdater::GetCurrentContainerName()
	{
		if (m_pCurrentUpdates==NULL) return "";
		return m_sCurrentUpdates;
	}

	bool cUpdater::AddContainer(tString a_sName)
	{
		tUpdateContainerMap::value_type val = tUpdateContainerMap::value_type(a_sName, tUpdateableList());

		m_mapUpdateContainer.insert(val);

		return true;
	}

	bool cUpdater::AddUpdate(tString a_sContainer, iUpdateable *a_pUpdate)
	{
		if (a_pUpdate == NULL)
		{
			Error("Couldn't add NULL updatable!");
			return false;
		}
		tUpdateContainerMapIt it = m_mapUpdateContainer.find(a_sContainer);
		if(it == m_mapUpdateContainer.end()) return false;

		it->second.push_back(a_pUpdate);
		return true;
	}

	bool cUpdater::AddGlobalUpdate(iUpdateable *a_pUpdate)
	{
		m_lstGlobalUpdateableList.push_back(a_pUpdate);
		return true;
	}
}