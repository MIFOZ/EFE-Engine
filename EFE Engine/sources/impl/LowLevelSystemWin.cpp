#ifdef WIN32
#if defined(_DEBUG) | defined(DEBUG)
#pragma comment(lib, "angelscriptd.lib")
#else
#pragma comment(lib, "angelscript.lib")
#endif
#define UNICODE
#include <Windows.h>
#include <ShlObj.h>
#endif

//#pragma comment(lib, "WINMM.lib")

#define _UNICODE

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <string>

#include "impl/LowLevelSystemWin.h"
#include "impl/AScript.h"

#include "impl/scriptstdstring.h"

#include "system/String.h"

#include <clocale>

extern int efeMain(const efe::tString &a_sCommandLine);

#ifdef WIN32
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return efeMain(lpCmdLine);
}
#endif

namespace efe
{
	static cLogWriter g_LogWriter(_W("efe.log"));
	static cLogWriter g_UpdateLogWriter(_W("efe_update.log"));

	//--------------------------------------------------------------

	cLogWriter::cLogWriter(const tWString &a_sFileName)
	{
		m_sFileName = a_sFileName;
	}

	cLogWriter::~cLogWriter()
	{
		if (m_pFile) fclose(m_pFile);
	}

	void cLogWriter::Write(const tString &a_sMessage)
	{
		if (!m_pFile) ReopenFile();

		if (m_pFile)
		{
			fputs(a_sMessage.c_str(), m_pFile);
			fflush(m_pFile);
		}
		fputs(a_sMessage.c_str(), stdout);
	}

	void cLogWriter::Clear()
	{
		ReopenFile();
		if (m_pFile) fflush(m_pFile);
	}

	//--------------------------------------------------------------

	void cLogWriter::SetFileName(const tWString &a_sFile)
	{
		if (m_sFileName == a_sFile) return;

		m_sFileName = a_sFile;
		ReopenFile();
	}

	//--------------------------------------------------------------

	void cLogWriter::ReopenFile()
	{
		if (m_pFile) fclose(m_pFile);

		m_pFile = _wfopen(m_sFileName.c_str(), _W("w"));
	}

	//--------------------------------------------------------------

	cLowLevelSystemWin::cLowLevelSystemWin()
	{
		m_pScriptEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

		m_pScriptOutput = efeNew(cScriptOutput, ());
		m_pScriptEngine->SetMessageCallback(asMETHOD(cScriptOutput, AddMessage), m_pScriptOutput, asCALL_THISCALL);

#ifdef AS_MAX_PORTABILITY

#else
		RegisterStdString(m_pScriptEngine);
#endif

		m_lHandleCount = 0;

		//timeBeginPeriod(1);

		Log("-------- THE EFE ENGINE LOG ------------\n\n");
	}

	cLowLevelSystemWin::~cLowLevelSystemWin()
	{
		//timeEndPeriod(1);

		m_pScriptEngine->Release();
		efeDelete(m_pScriptOutput);
	}

	//--------------------------------------------------------------

	void cScriptOutput::AddMessage(const asSMessageInfo *msg)
	{
		char sMess[1024];

		tString type = "ERR ";
		if (msg->type == asMSGTYPE_WARNING)
			type = "WARN";
		else if (msg->type == asMSGTYPE_INFORMATION)
			type = "INFO";

		sprintf(sMess, "%s (%d %d) : %s : %s\n", msg->section, msg->row, msg->col, type.c_str(), msg->message);

		m_sMessage += sMess;
	}

	void cScriptOutput::Display()
	{
		if (m_sMessage.size()>500)
		{
			while (m_sMessage.size() > 500)
			{
				tString sSub = m_sMessage.substr(0,500);
				m_sMessage = m_sMessage.substr(500);
				Log(sSub.c_str());
			}
			Log(m_sMessage.c_str());
		}
		else
			Log(m_sMessage.c_str());
	}

	//--------------------------------------------------------------

	void cScriptOutput::Clear()
	{
		m_sMessage = "";
	}

	//--------------------------------------------------------------

	iScript *cLowLevelSystemWin::CreateScript(const tString &a_sName)
	{
		return efeNew(cAScript, (a_sName, m_pScriptEngine, m_pScriptOutput, m_lHandleCount++));
	}

	//--------------------------------------------------------------

	bool cLowLevelSystemWin::AddScriptFunc(const tString &a_sFuncDecl, void *a_pFunc, int callConv)
	{
		if (m_pScriptEngine->RegisterGlobalFunction(a_sFuncDecl.c_str(),
			asFUNCTION(a_pFunc), callConv),0)
		{
			Error("Couldn't add func '%s'\n", a_sFuncDecl.c_str());
			return false;
		}

		return true;
	}

	//--------------------------------------------------------------

	void SetLogFile(const tWString &a_sFile)
	{
		g_LogWriter.SetFileName(a_sFile);
	}

	void FatalError(const char *fmt, ...)
	{
		char text[2048];
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		tString sMess = "FATAL ERROR: ";
		sMess += text;
		g_LogWriter.Write(sMess);

#ifdef WIN32
		MessageBox(NULL, cString::To16Char(text).c_str(), _W("FATAL ERROR"), MB_ICONERROR);
#endif
		exit(1);
	}

	void Error(const char *fmt,...)
	{
		char text[2048];
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		tString sMess = "ERROR: ";
		sMess += text;
		g_LogWriter.Write(sMess);
	}

	void Warning(const char *fmt, ...)
	{
		char text[2048];
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		tString sMess = "WARNING: ";
		sMess += text;
		g_LogWriter.Write(sMess);
	}

	void Log(const char *fmt, ...)
	{
		char text[2048];
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		tString sMess = "";
		sMess += text;
		g_LogWriter.Write(sMess);
	}

	static bool g_bUpdateLogIsActive;
	void SetUpdateLogFile(const tWString &a_sFile)
	{
		g_UpdateLogWriter.SetFileName(a_sFile);
	}

	void ClearUpdateLogFile()
	{
		if (!g_bUpdateLogIsActive) return;

		g_UpdateLogWriter.Clear();
	}

	void SetUpdateLogActive(bool a_bX)
	{
		g_bUpdateLogIsActive = a_bX;
	}

	void LogUpdate(const char *fmt, ...)
	{
		if (!g_bUpdateLogIsActive) return;

		char text[2048];
		va_list ap;
		if (fmt==NULL)
			return;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		tString sMess = "";
		sMess += text;
		g_UpdateLogWriter.Write(sMess);
	}

	//--------------------------------------------------------------

	void CreateMessageBoxW(eMsgBoxType a_Type, const wchar_t *a_sCaption, const wchar_t *fmt, va_list ap)
	{
		wchar_t text[2048];

		if (fmt == NULL)
			return;

		vswprintf(text, 2047, fmt, ap);

		tWString sMess = _W("");

		sMess += text;

		UINT lType = MB_OK;

		switch(a_Type)
		{
		case eMsgBoxType_Info:
			lType += MB_ICONINFORMATION; break;
		case eMsgBoxType_Error:
			lType += MB_ICONERROR; break;
		case eMsgBoxType_Warning:
			lType += MB_ICONWARNING; break;
		default: break;
		}

		MessageBox(NULL, sMess.c_str(), a_sCaption, lType);
	}

	void CreateMessageBoxW(const wchar_t *a_sCaption, const wchar_t *fmt, ...)
	{
		va_list ap;

		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		CreateMessageBoxW(eMsgBoxType_Default, a_sCaption, fmt, ap);
		va_end(ap);
	}

	extern void CreateMessageBoxW(eMsgBoxType a_Type, const wchar_t *a_sCaption, const wchar_t *fmt, ...)
	{
		va_list ap;

		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		CreateMessageBoxW(a_Type, a_sCaption, fmt, ap);
		va_end(ap);
	}

	//--------------------------------------------------------------

	void OpenBrowserWindow(const tWString &a_sUrl)
	{
		ShellExecute(NULL, _W("open"), a_sUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}

	//--------------------------------------------------------------

	tString ShowOpenFileDialog()
	{
		return "";
	}

	//--------------------------------------------------------------

	void CopyTextToClipboard(const tWString &a_sText)
	{
		OpenClipboard(NULL);
		EmptyClipboard();

		HGLOBAL clipbuffer;
		wchar_t *pBuffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, (a_sText.size()+1) * sizeof(wchar_t));
		pBuffer = (wchar_t *) GlobalLock(clipbuffer);
		wcscpy(pBuffer, a_sText.c_str());

		SetClipboardData(CF_UNICODETEXT, pBuffer);

		GlobalUnlock(clipbuffer);

		CloseClipboard();
	}

	tWString LoadTextFromClipboard()
	{
		tWString sText = _W("");
		OpenClipboard(NULL);

		HGLOBAL clipbuffer = GetClipboardData(CF_UNICODETEXT);

		wchar_t *pBuffer = (wchar_t*)GlobalLock(clipbuffer);

		if (pBuffer != NULL) sText = pBuffer;

		GlobalUnlock(clipbuffer);

		CloseClipboard();

		return sText;
	}

	//--------------------------------------------------------------

	tWString GetSystemSpecialPath(eSystemPath a_PathType)
	{
		int type;
		switch (a_PathType)
		{
		case efe::eSystemPath_Personal: type = CSIDL_PERSONAL;
			break;
		default: return _W("");
		}

		TCHAR sPath[1024];
		if (SUCCEEDED(SHGetFolderPath(NULL,
			type | CSIDL_FLAG_CREATE,
			NULL, 0, sPath)))
		{
			return tWString(sPath);
		}
		else
			return _W("");
	}

	//--------------------------------------------------------------

	static cDate DateFromGMTIme(struct tm*	a_pClock)
	{
		cDate date;

		date.seconds = a_pClock->tm_sec;
		date.minuts = a_pClock->tm_min;
		date.hours = a_pClock->tm_hour;
		date.month_day = a_pClock->tm_mday;
		date.month = a_pClock->tm_mon;
		date.year = 1990 + a_pClock->tm_year;
		date.week_day = a_pClock->tm_wday;
		date.year_day = a_pClock->tm_yday;

		return date;
	}

	//--------------------------------------------------------------

	/*void OpenBrowserWindow(const tWString &a_sURL)
	{
		ShellExecute(NULL, _W("open"), a_sURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}*/

	bool FileExists(const tWString &a_sFileName)
	{
		FILE *f  = _wfopen(a_sFileName.c_str(), _W("r"));

		if (f == NULL)
			return false;
		fclose(f);
		return true;
	}

	//--------------------------------------------------------------

	cDate FileModifiedDate(const tWString &a_sFilePath)
	{
		struct tm* pClock;
		struct _stat attrib;
		_wstat(a_sFilePath.c_str(), &attrib);

		pClock = gmtime(&(attrib.st_ctime));

		cDate date = DateFromGMTIme(pClock);

		return date;
	}

	//--------------------------------------------------------------

	void SetWindowCaption(const tString &a_sName)
	{
		tWString wsName = cString::To16Char(a_sName);
		SetWindowText(NULL, wsName.c_str());
	}

	//--------------------------------------------------------------

	unsigned long GetApplicationTime()
	{
		return GetCurrentTime();//timeGetTime();
	}

	unsigned long cLowLevelSystemWin::GetTime()
	{
		return GetCurrentTime();
	}
}