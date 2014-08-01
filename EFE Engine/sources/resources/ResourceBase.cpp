#include "resources/ResourceBase.h"

#include "system/LowLevelSystem.h"

namespace efe
{
	bool iResourceBase::m_bLogCreateAndDelete = false;

	iResourceBase::iResourceBase(tString a_sName, unsigned long a_lPrio)
	{
		m_lTime = (unsigned long)time(NULL);
		m_lPrio = a_lPrio;
		m_lHandle = 0;
		m_lUserCount = 0;
		m_sName = a_sName;
		m_bLogDestruction = false;
	}

	iResourceBase::~iResourceBase()
	{
	}

	void iResourceBase::IncUserCount()
	{
		m_lUserCount++;
		m_lTime = (unsigned long)time(NULL);
	}
}