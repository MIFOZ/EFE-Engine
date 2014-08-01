#ifndef EFE_SYSTEM_H
#define EFE_SYSTEM_H

namespace efe
{
	class iLowLevelSystem;
	class cLogicTimer;

	class cSystem
	{
	public:
		cSystem(iLowLevelSystem *a_pLowLevelSystem);
		~cSystem();

		iLowLevelSystem *GetLowLevel();

		cLogicTimer *CreateLogicTimer(unsigned int a_iUpdatesPerSec);
	private:
		iLowLevelSystem *m_pLowLevelSystem;
	};
};
#endif