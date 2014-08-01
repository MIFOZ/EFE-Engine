#ifndef EFE_STATE_MONITOR
#define EFE_STATE_MONITOR

#include "system/SystemTypes.h"

namespace efe
{
	template <class T> class StateMonitor
	{
	public:
		StateMonitor(T a_InitState)
			: m_bUpdateNeeded(false),
			m_InitialState(a_InitState),
			m_State(a_InitState),
			m_pSister(NULL)
		{
		}

		~StateMonitor(){}

		void SetSister(StateMonitor<T> *a_pSister)
		{
			m_pSister = a_pSister;
		}
		bool SameAsSister()
		{
			return m_State == m_pSister->m_State;
		}

		void SetState(T a_State)
		{
			m_State = a_State;

			if (m_pSister == 0)
			{
				m_bUpdateNeeded = true;

				return;
			}

			m_bUpdateNeeded = !SameAsSister();
		}
		T GetState() const
		{
			return m_State;
		}

		bool IsUpdateNeeded()
		{
			return m_bUpdateNeeded;
		}
		void InitializeState()
		{
			SetState(m_InitialState);
		}
		void ResetTracking()
		{
			m_bUpdateNeeded = false;
		}

	private:
		bool m_bUpdateNeeded;

		T m_InitialState;
		T m_State;

		StateMonitor<T> *m_pSister;
	};

	typedef StateMonitor<int> StateMonitorl;
	typedef StateMonitor<unsigned int> StateMonitorul;

	template <class T, unsigned int N> class StateArrayMonitor
	{
	public:
		StateArrayMonitor(T a_InitState)
			: m_lStartSlot(0),
			m_lEndSlot(0),
			m_bUpdateNeeded(false),
			m_InitialState(a_InitState),
			m_pSister(NULL)
		{
			InitializeStates();
			ResetTracking();
		}
		~StateArrayMonitor(){}

		void SetSister(StateArrayMonitor<T,N> *a_pSister)
		{
			m_pSister = a_pSister;
		}
		bool SameAsSister(unsigned int a_lSlot)
		{
			return m_States[a_lSlot] == m_pSister->m_States[a_lSlot];
		}

		void SetState(unsigned int a_lSlot, T a_State)
		{
			m_States[a_lSlot] = a_State;

			if (m_pSister == NULL)
			{
				m_bUpdateNeeded = true;
				m_lStartSlot = 0;
				m_lEndSlot = N-1;

				return;
			}

			bool bSameAsSister = SameAsSister(a_lSlot);

			if (!m_bUpdateNeeded && !bSameAsSister)
			{
				m_bUpdateNeeded = true;
				m_lStartSlot = a_lSlot;
				m_lEndSlot = a_lSlot;
			}

			if (m_bUpdateNeeded)
			{
				if (a_lSlot < m_lStartSlot)
				{
					if (!bSameAsSister)
						m_lStartSlot = a_lSlot;
				}
				else if (a_lSlot == m_lStartSlot)
				{
					if (bSameAsSister)
						SearchFromBelow();
				}
				else if (m_lStartSlot < a_lSlot && a_lSlot < m_lEndSlot)
				{

				}
				else if (a_lSlot == m_lEndSlot)
				{
					if (bSameAsSister)
						SearchFromAbove();
				}
				else if (m_lEndSlot < a_lSlot)
				{
					if (!bSameAsSister)
						m_lEndSlot = a_lSlot;
				}
			}
		}
		T GetState(unsigned int a_lSlot) const
		{
			return m_States[a_lSlot];
		}
		T *GetFirstSlotLocation()
		{
			return &m_States[m_lStartSlot];
		}
		T *GetSlotLocation(unsigned int a_lSlot)
		{
			return &m_States[a_lSlot];
		}

		unsigned int GetStartSlot()
		{
			return m_lStartSlot;
		}
		unsigned int GetEndSlot()
		{
			return m_lEndSlot;
		}
		unsigned int GetRange()
		{
			return m_lEndSlot - m_lStartSlot + 1;
		}

		bool IsUpdateNeeded()
		{
			return m_bUpdateNeeded;
		}

		void InitializeStates()
		{
			for (unsigned int i = 0; i < N; i++)
			{
				SetState(i, m_InitialState);
			}
		}
		void ResetTracking()
		{
			m_lStartSlot = 0;
			m_lEndSlot = 0;
			m_bUpdateNeeded = false;
		}

	private:
		void SearchFromBelow()
		{
			for (; m_lStartSlot < m_lEndSlot; m_lStartSlot++)
			{
				if (!SameAsSister(m_lStartSlot))
					break;
			}

			if (m_lStartSlot == m_lEndSlot && SameAsSister(m_lStartSlot))
				ResetTracking();
		}
		void SearchFromAbove()
		{
			for (; m_lEndSlot > m_lStartSlot; m_lEndSlot--)
			{
				if (!SameAsSister(m_lEndSlot))
					break;
			}

			if (m_lEndSlot == m_lStartSlot && SameAsSister(m_lEndSlot))
				ResetTracking();
		}

		unsigned int m_lStartSlot;
		unsigned int m_lEndSlot;

		bool m_bUpdateNeeded;

		T m_InitialState;
		T m_States[N];

		StateArrayMonitor<T,N> *m_pSister;
	};
};
#endif