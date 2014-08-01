#ifndef EFE_UPDATEABLE_H
#define EFE_UPDATEABLE_H

#include "system/SystemTypes.h"

namespace efe
{
	class iUpdateable
	{
	public:
		iUpdateable(const tString &a_sName) : m_sName(a_sName){}

		virtual void OnDraw(){}

		virtual void OnPostSceneDraw(){}

		virtual void OnPostGUIDraw(){}

		virtual void OnPostBufferSwap(){}

		virtual void OnStart(){}

		virtual void Update(float a_fTimeStep){}

		virtual void OnExit(){}

		virtual void Reset(){}

		const tString GetName()
		{
			return m_sName;
		}
	private:
		tString m_sName;
	};
};
#endif