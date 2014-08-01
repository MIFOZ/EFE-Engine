#ifndef EFE_ACTION_H
#define EFE_ACTION_H

#include "system/SystemTypes.h"

namespace efe
{
	class iAction
	{
	public:
		iAction(tString a_sName);
		~iAction(){}

		bool WasTriggered();

		bool BecameTriggered();

		bool DoubleTriggered(float a_fLimit);

		void Update(float a_fTimeStep);

		tString GetName();

		virtual void UpdateLogic(float a_fTimeStep);

		virtual bool IsTriggered()=0;

		virtual float GetValue()=0;

		virtual tString GetInputName()=0;

		virtual tString GetInputType()=0;
	private:
		tString m_sName;

		bool m_bBecameTriggered;
		bool m_bIsTriggered;

		bool m_bTriggerDown;

		double m_fTimeCount;
	};
};
#endif