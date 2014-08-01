#ifndef EFE_A_SCRIPT_H
#define EFE_A_SCRIPT_H

#include "system/Script.h"
#include "impl/LowLevelSystemWin.h"
#include <angelscript.h>

namespace efe
{
	class cAScript : public iScript
	{
	public:
		cAScript(const tString &a_sName, asIScriptEngine *a_pScriptEngine,
			cScriptOutput *a_pScriptOutput, int a_lHandle);
		~cAScript();

		bool CreateFromFile(const tString &a_sFileName);

		int GetFuncHandle(const tString &a_sFunc);
		void AddArg(const tString &a_sArg);

		bool Run(const tString &a_sFuncLine);
		bool Run(int a_lHandle);
	private:
		asIScriptEngine *m_pScriptEngine;
		cScriptOutput *m_pScriptOutput;

		asIScriptContext *m_pContext;

		int m_lHandle;
		tString m_sModuleName;
	};
};
#endif