#include "impl/AScript.h"
#include "system/LowLevelSystem.h"
#include "system/System.h"
#include "math/Math.h"
#include "impl/Platform.h"
#include <stdio.h>

#include "impl/scriptbuilder.h"
#include "impl/scripthelper.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cAScript::cAScript(const tString &a_sName, asIScriptEngine *a_pScriptEngine,
			cScriptOutput *a_pScriptOutput, int a_lHandle)
			: iScript(a_sName)
	{
		m_pScriptEngine = a_pScriptEngine;
		m_pScriptOutput = a_pScriptOutput;
		m_lHandle = a_lHandle;

		m_pContext = m_pScriptEngine->CreateContext();

		m_sModuleName = "Module_"+cString::ToString(cMath::RandRectl(0,1000000))+
						"_"+cString::ToString(m_lHandle);
	}

	cAScript::~cAScript()
	{
		m_pScriptEngine->DiscardModule(m_sModuleName.c_str());
		m_pContext->Release();
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	bool cAScript::CreateFromFile(const tString &a_sFileName)
	{
		CScriptBuilder builder;

		builder.StartNewModule(m_pScriptEngine, m_sModuleName.c_str());

		if (builder.AddSectionFromFile(a_sFileName.c_str())<0)
		{
			Error("Couldn't add script '%s'!\n", a_sFileName.c_str());
			return false;
		}

		if (builder.BuildModule()<0)
		{
			Error("Couldn't build script '%s'!\n", a_sFileName.c_str());
			Log("------- SCRIPT OUTPUT BEGIN -----------------------------\n");
			m_pScriptOutput->Display();
			m_pScriptOutput->Clear();
			Log("------- SCRIPT OUTPUT END -------------------------------\n");

			return false;
		}
		m_pScriptOutput->Clear();

		return true;
	}

	//--------------------------------------------------------------

	int cAScript::GetFuncHandle(const tString &a_sFunc)
	{
		return m_pScriptEngine->GetModule(m_sModuleName.c_str(), asGM_CREATE_IF_NOT_EXISTS)->GetFunctionIdByName(a_sFunc.c_str());
	}

	//--------------------------------------------------------------

	void cAScript::AddArg(const tString &a_sArg)
	{
		
	}

	//--------------------------------------------------------------

	bool cAScript::Run(const tString &a_sFuncLine)
	{
		ExecuteString(m_pScriptEngine, a_sFuncLine.c_str(), m_pScriptEngine->GetModule(m_sModuleName.c_str(), asGM_ONLY_IF_EXISTS),
			m_pContext);

		return true;
	}

	//--------------------------------------------------------------

	bool cAScript::Run(int a_lHandle)
	{
		m_pContext->Prepare(a_lHandle);

		m_pContext->Execute();

		return true;
	}

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	/*char *cAScript::LoadCharBuffer(const tString &a_sFileName, int &a_lLength)
	{
		FILE *pFile = fopen(a_sFileName.c_str(), "rb");
		if (pFile == NULL)
			return NULL;

		int lLength = (int)Platform::FileLength(pFile);
		a_lLength = lLength;

		char *pBuffer = efeNewArray(char, lLength);
		fread(pBuffer, lLength, 1, pFile);

		fclose(pFile);

		return pBuffer;
	}*/
}