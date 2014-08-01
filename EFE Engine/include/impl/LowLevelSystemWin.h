#ifndef EFE_LOWLEVELSYSTEM_WIN_H
#define EFE_LOWLEVELSYSTEM_WIN_H

#include "system/LowLevelSystem.h"
#include <angelscript.h>
#include <stdio.h>

namespace efe
{
	class cLogWriter
	{
	public:
		cLogWriter(const tWString &a_sDefaultFile);
		~cLogWriter();

		void Write(const tString &a_sMessage);
		void Clear();

		void SetFileName(const tWString &a_sFile);
	private:
		void ReopenFile();

		FILE *m_pFile;
		tWString m_sFileName;
	};

	class cScriptOutput
	{
	public:
		cScriptOutput() : m_sMessage("") {}
		~cScriptOutput(){}

		void AddMessage(const asSMessageInfo *msg);
		void Display();
		void Clear();

	private:
		tString m_sMessage;
	};

	class cLowLevelSystemWin : public iLowLevelSystem
	{
	public:
		cLowLevelSystemWin();
		~cLowLevelSystemWin();

		void SetWindowsCaption(const tString &a_SName);

		unsigned long GetTime();
		cDate GetDate();

		iScript *CreateScript(const tString &a_sName);

		bool AddScriptFunc(const tString &a_sFuncDecl, void *a_pFunc, int callConv);

		void Sleep(const unsigned int a_lMillsecs);
	private:
		asIScriptEngine *m_pScriptEngine;
		cScriptOutput *m_pScriptOutput;
		int m_lHandleCount;
	};
};

#endif