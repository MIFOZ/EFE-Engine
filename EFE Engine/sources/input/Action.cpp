#include "input/Action.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	iAction::iAction(tString a_sName)
	{
		m_sName = a_sName;

		m_bBecameTriggered = false;
		m_bIsTriggered =false;

		m_fTimeCount = -1.0f;

		m_bTriggerDown = false;
	}

	bool iAction::WasTriggered()
	{
		if (m_bBecameTriggered && !IsTriggered())
		{
			m_bBecameTriggered = false;
			return true;
		}

		return false;
	}

	bool iAction::BecameTriggered()
	{
		if (!m_bIsTriggered && IsTriggered())
		{
			m_bIsTriggered = true;
			return true;
		}

		return false;
	}

	bool iAction::DoubleTriggered(float a_fLimit)
	{
		if (m_bTriggerDown && IsTriggered())
		{
			m_bTriggerDown = true;

			if (m_fTimeCount < 0 || m_fTimeCount > a_fLimit)
			{
				m_fTimeCount = 0;
				return false;
			}
			else
			{
				m_fTimeCount = 0;
				m_bIsTriggered = true;
				return true;
			}
		}

		return false;
	}

	void iAction::Update(float a_fTimeStep)
	{
		UpdateLogic(a_fTimeStep);

		if (!IsTriggered())
		{
			m_bIsTriggered = false;
			m_bTriggerDown = false;

			if (m_fTimeCount >= 0) m_fTimeCount += a_fTimeStep;
		}
		else
			m_bBecameTriggered = true;
	}

	void iAction::UpdateLogic(float a_fTimeStep)
	{
	}

	tString iAction::GetName()
	{
		return m_sName;
	}
}