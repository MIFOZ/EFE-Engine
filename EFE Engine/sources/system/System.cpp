#include "system/System.h"
#include "system/LowLevelSystem.h"
#include "system/LogicTimer.h"
#include "system/String.h"

namespace efe
{
	cSystem::cSystem(iLowLevelSystem *a_pLowLevelSystem)
	{
		m_pLowLevelSystem = a_pLowLevelSystem;
	}

	cSystem::~cSystem()
	{
		
	}

	cLogicTimer *cSystem::CreateLogicTimer(unsigned int a_iUpdatesPerSec)
	{
		return efeNew(cLogicTimer, (a_iUpdatesPerSec, m_pLowLevelSystem));
	}

	iLowLevelSystem *cSystem::GetLowLevel()
	{
		return m_pLowLevelSystem;
	}
}