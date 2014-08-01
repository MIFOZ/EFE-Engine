#ifndef EFE_LOWLEVELSYSTEM_H
#define EFE_LOWLEVELSYSTEM_H

#include "system/MemoryManager.h"
#include "system/SystemTypes.h"

namespace efe
{
#define START_TIMING_EX(x,y)	LogUpdate("Updating %s in file %s at line %d\n", x, __FILE__,__LINE__);\
								unsigned int y##_lTime =  GetApplicationTime();
#define START_TIMING(x)			LogUpdate("Updating %s in file %s at line %d\n", #x, __FILE__,__LINE__);\
								unsigned int x##_lTime =  GetApplicationTime();
#define STOP_TIMING(x)			LogUpdate(" Time spent: %d ms\n", GetApplicationTime() - x##_lTime);
#define START_TIMING_TAB(x)		LogUpdate("\tUpdating %s in file %s at line %d\n", #x, __FILE__,__LINE__);\
								unsigned int x##_lTime =  GetApplicationTime();
#define STOP_TIMING_TAB(x)		LogUpdate("\t Time spent: %d ms\n", GetApplicationTime() - x##_lTime);

	class iScript;

	extern void SetLogFile(const tWString &a_sFile);
	extern void FatalError(const char *fmt,...);
	extern void Error(const char *fmt, ...);
	extern void Warning(const char *fmt, ...);
	extern void Log(const char *fmt, ...);

	extern void SetUpdateLogFile(const tWString &a_sFile);
	extern void ClearUpdateLogFile();
	extern void SetUpdateLogActive(bool a_bX);
	extern void LogUpdate(const char *fmt, ...);

	extern void CreateMessageBoxW(const wchar_t *a_sCaption, const wchar_t *fmt, ...);
	extern void CreateMessageBoxW(eMsgBoxType a_Type, const wchar_t *a_sCaption, const wchar_t *fmt, ...);

	extern void OpenBrowserWindow(const tWString &a_sUrl);

	extern tString ShowOpenFileDialog();

	extern void CopyTextToClipboard(const tWString &a_sText);
	extern tWString LoadTextFromClipboard();

	extern tWString GetSystemSpecialPath(eSystemPath a_PathType);

	extern bool FileExists(const tWString &a_sFileName);
	extern cDate FileModifiedDate(const tWString &a_sFilePath);

	extern void SetWindowCaption(const tString &a_sName);

	extern unsigned long GetApplicationTime();

	class iLowLevelSystem
	{
	public:
		virtual ~iLowLevelSystem(){}

		virtual unsigned long GetTime() = 0;

		virtual iScript *CreateScript(const tString &a_sName)=0;

		virtual bool AddScriptFunc(const tString &a_sFuncDecl, void *a_pFunc, int callConv)=0; 
	};
};
#endif