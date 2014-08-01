#ifndef EFE_LOGICTIMER_H
#define EFE_LOGICTIMER_H

namespace efe
{
	class iLowLevelSystem;

	class cLogicTimer
	{
	public:
		cLogicTimer(int a_lUpdatesPerSecond, iLowLevelSystem *a_pLowLevelSystem);
		~cLogicTimer();

		void Reset();

		bool WantUpdate();

		void EndUpdateLoop();

		void SetUdpatesPerSecond(int a_lUpdatesPerSecond);

		void SetMaxUpdates(int a_lMax);

		int GetUpdatesPerSecond();

		float GetStepSize();
	private:
		void Update();

		double m_fLocalTime;
		double m_fLocalTimeAdd;

		int m_lMaxUpdates;
		int m_lUdpateCount;

		iLowLevelSystem *m_pLowLevelSystem;
	};
};

#endif