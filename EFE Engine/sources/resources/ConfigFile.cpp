#include "resources/ConfigFile.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/FileSearcher.h"
#include "impl/tinyXML/tinyxml.h"

#include <stdio.h>

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cConfigFile::cConfigFile(tWString a_sFile)
	{
		m_sFile = a_sFile;
		m_pXmlDoc = efeNew(TiXmlDocument, ());
	}

	cConfigFile::~cConfigFile()
	{
		efeDelete(m_pXmlDoc);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cConfigFile::Load()
	{
		FILE *pFile = _wfopen(m_sFile.c_str(), _W("rb"));

		bool bRet = m_pXmlDoc->LoadFile(pFile);

		if (pFile) fclose(pFile);

		return bRet;
	}

	//--------------------------------------------------------------

	bool cConfigFile::Save()
	{
		FILE *pFile = _wfopen(m_sFile.c_str(), _W("rb"));

		bool bRet = m_pXmlDoc->SaveFile(pFile);

		if (pFile) fclose(pFile);

		return bRet;
	}

	//--------------------------------------------------------------

	void cConfigFile::SetString(tString a_sLevel, tString a_sName, tString a_sVal)
	{
		TiXmlElement *pLevelElem = m_pXmlDoc->FirstChildElement(a_sLevel.c_str());

		if (pLevelElem==NULL)
		{
			TiXmlElement *pNodeChild = efeNew(TiXmlElement, (a_sLevel.c_str()));
			pLevelElem = static_cast<TiXmlElement*>(m_pXmlDoc->InsertEndChild(*pNodeChild));
			efeDelete(pNodeChild);
		}

		pLevelElem->SetAttribute(a_sName.c_str(), a_sVal.c_str());
	}

	//--------------------------------------------------------------

	void cConfigFile::SetInt(tString a_sLevel, tString a_sName, int a_lVal)
	{
		char sBuffer[40];
		sprintf(sBuffer, "%d", a_lVal);

		SetString(a_sLevel, a_sName, sBuffer);
	}

	//--------------------------------------------------------------

	void cConfigFile::SetFloat(tString a_sLevel, tString a_sName, float a_fVal)
	{
		char sBuffer[40];
		sprintf(sBuffer, "%f", a_fVal);

		SetString(a_sLevel, a_sName, sBuffer);
	}

	//--------------------------------------------------------------

	void cConfigFile::SetBool(tString a_sLevel, tString a_sName, bool a_bVal)
	{
		SetString(a_sLevel, a_sName, a_bVal ? "true" : "false");
	}

	//--------------------------------------------------------------

	tString cConfigFile::GetString(tString a_sLevel, tString a_sName, tString a_sDefault)
	{
		const char *sVal = GetCharArray(a_sLevel, a_sName);
		if (sVal == NULL)
			return a_sDefault;

		return sVal;
	}

	//--------------------------------------------------------------

	int cConfigFile::GetInt(tString a_sLevel, tString a_sName, int a_lDefault)
	{
		const char *sVal = GetCharArray(a_sLevel, a_sName);
		if (sVal == NULL) return a_lDefault;

		return cString::ToInt(sVal, a_lDefault);
	}

	//--------------------------------------------------------------

	float cConfigFile::GetFloat(tString a_sLevel, tString a_sName, float a_fDefault)
	{
		const char *sVal = GetCharArray(a_sLevel, a_sName);
		if (sVal == NULL) return a_fDefault;

		return cString::ToFloat(sVal, a_fDefault);
	}

	//--------------------------------------------------------------

	bool cConfigFile::GetBool(tString a_sLevel, tString a_sName, bool a_bDefault)
	{
		const char *sVal = GetCharArray(a_sLevel, a_sName);
		if (sVal == NULL) return a_bDefault;

		return cString::ToBool(sVal, a_bDefault);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	const char *cConfigFile::GetCharArray(tString a_sLevel, tString a_sName)
	{
		TiXmlElement *pLevelElem = m_pXmlDoc->FirstChildElement(a_sLevel.c_str());
		if (pLevelElem == NULL)
			return NULL;

		return pLevelElem->Attribute(a_sName.c_str());
	}
}