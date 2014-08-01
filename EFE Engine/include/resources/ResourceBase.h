#ifndef EFE_RESOURCEBASE_H
#define EFE_RESOURCEBASE_H

#include <time.h>
#include "system/SystemTypes.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	class iResourceBase
	{
	public:
		iResourceBase(tString a_sName, unsigned long a_lPrio);

		virtual ~iResourceBase();

		virtual bool Reload()=0;

		virtual void Unload()=0;

		virtual void Destroy()=0;

		tString GetName(){return m_sName;}
		unsigned long GetHandle(){return m_lHandle;}
		void SetHanlde(unsigned long a_lHandle){m_lHandle = a_lHandle;}
		tString GetFilePath(){return m_sFilePath;}
		unsigned long GetTime(){return m_lTime;}
		unsigned long GetPrio(){return m_lPrio;}
		unsigned long GetSize(){return m_lSize;}

		void SetLogDestruction(bool a_bX){m_bLogDestruction = a_bX;}

		unsigned int GetUserCount(){return m_lUserCount;}
		void IncUserCount();
		void DecUserCount(){if(m_lUserCount>0)m_lUserCount--;};
		bool HasUsers(){return m_lUserCount>0;}

		static bool GetLogCreateAndDelete(){return m_bLogCreateAndDelete;}
		static void SetLogCreateAndDelete(bool a_bX){m_bLogCreateAndDelete = a_bX;}
	protected:
		static bool m_bLogCreateAndDelete;

		unsigned int m_lPrio;
		unsigned long m_lTime;
		unsigned long m_lSize;

		unsigned int m_lUserCount;
		unsigned long m_lHandle;
		tString m_sName;
		tString m_sFilePath;
		bool m_bLogDestruction;
	};
};
#endif