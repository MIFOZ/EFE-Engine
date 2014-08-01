#include "system/LogicTimer.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	cLogicTimer::cLogicTimer(int a_lUpdatesPerSecond, iLowLevelSystem *a_pLowLevelSystem)
	{
		m_lMaxUpdates = a_lUpdatesPerSecond;
		m_lUdpateCount = 0;

		m_pLowLevelSystem = a_pLowLevelSystem;

		SetUdpatesPerSecond(a_lUpdatesPerSecond);
	}

	cLogicTimer::~cLogicTimer()
	{
	}

	void cLogicTimer::Reset()
	{
		m_fLocalTime = (double)GetApplicationTime();
	}

	bool cLogicTimer::WantUpdate()
	{
		++m_lUdpateCount;
		if (m_lUdpateCount > m_lMaxUpdates)
			return false;

		if (m_fLocalTime < (double)GetApplicationTime())
		{
			Update();
			return true;
		}
		return false;
	}

	void cLogicTimer::EndUpdateLoop()
	{
		if (m_lUdpateCount > m_lMaxUpdates)
			Reset();
		m_lUdpateCount = 0;
	}

	void cLogicTimer::SetUdpatesPerSecond(int a_lUpdatesPerSecond)
	{
		m_fLocalTimeAdd = 1000.0 / ((double)a_lUpdatesPerSecond);
		Reset();
	}

	void cLogicTimer::SetMaxUpdates(int a_lMax)
	{
		m_lMaxUpdates = a_lMax;
	}

	float cLogicTimer::GetStepSize()
	{
		return ((float)m_fLocalTimeAdd) / 1000.0f;
	}

	void cLogicTimer::Update()
	{
		m_fLocalTime += m_fLocalTimeAdd;
	}
}