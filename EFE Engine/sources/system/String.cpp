#include "system/String.h"
#include <stdlib.h>

namespace efe
{
	tWString cString::To16Char(const tString &a_sString)
	{
		tWString wsTemp;
		size_t needed = mbstowcs(NULL, &a_sString[0],0);
		wsTemp.resize(needed);
		mbstowcs(&wsTemp[0],&a_sString[0],needed);

		return wsTemp;
	}

	tString cString::To8Char(const tWString &a_wsString)
	{
		tString sTemp;
		size_t needed = wcstombs(NULL, &a_wsString[0],0);
		sTemp.resize(needed);
		wcstombs(&sTemp[0],&a_wsString[0],needed);

		return sTemp;
	}

	//--------------------------------------------------------------

	tString cString::Sub(const tString &a_sString, int a_lStart, int a_lCount)
	{
		int lStringSize = (int)a_sString.size();
		if (a_lStart >= lStringSize) return "";
		if (a_lStart + a_lCount > lStringSize) a_lCount = lStringSize - a_lStart;

		if (a_lCount < 0) return a_sString.substr(a_lStart);
		else return a_sString.substr(a_lStart, a_lCount);
	}

	tWString cString::SubW(const tWString &a_sString, int a_lStart, int a_lCount)
	{
		int lStringSize = (int)a_sString.size();
		if (lStringSize == 0) return _W("");
		if (a_lStart >= lStringSize) return _W("");
		if (a_lStart + a_lCount > lStringSize) a_lCount = lStringSize - a_lStart;

		if (a_lCount < 0) return a_sString.substr(a_lStart);
		else return a_sString.substr(a_lStart, a_lCount);
	}

	//--------------------------------------------------------------

	tString cString::GetFileExt(tString a_String)
	{
		int pos = GetLastStringPos(a_String, ".");

		if (pos < 0)
			return "";
		else
			return a_String.substr(pos+1);
	}

	tWString cString::GetFileExtW(tWString a_String)
	{
		int pos = GetLastStringPosW(a_String, _W("."));

		if (pos < 0)
			return _W("");
		else
			return a_String.substr(pos+1);
	}

	tString cString::ToLowerCase(tString a_String)
	{
		for (int i=0;i<(int)a_String.size();i++)
			a_String[i] = tolower(a_String[i]);

		return a_String;
	}

	tWString cString::ToLowerCaseW(tWString a_String)
	{
		for (int i=0;i<(int)a_String.size();i++)
			a_String[i] = tolower(a_String[i]);

		return a_String;
	}

	tString cString::SetFileExt(tString a_String, tString a_Ext)
	{
		if (a_Ext.substr(0,1)==".")a_Ext = a_Ext.substr(1);
		if (GetFileExt(a_String) != "")
			a_String = a_String.substr(0,GetLastStringPos(a_String,"."));

		if (a_Ext!="")
			a_String = a_String + "." + a_Ext;

		return a_String;
	}

	tWString cString::SetFileExtW(tWString a_String, tWString a_Ext)
	{
		if (a_Ext.substr(0,1)==_W("."))a_Ext = a_Ext.substr(1);
		if (GetFileExtW(a_String) != _W(""))
			a_String = a_String.substr(0,GetLastStringPosW(a_String,_W(".")));

		if (a_Ext!=_W(""))
			a_String = a_String + _W(".") + a_Ext;

		return a_String;
	}

	tString cString::GetFilePath(tString a_String)
	{
		if (GetLastStringPos(a_String ,"")<0)return a_String;

		int pos1 = GetLastStringPos(a_String, "\\");
		int pos2 = GetLastStringPos(a_String, "/");
		int pos = pos1>pos2 ? pos1 : pos2;

		if (pos<0)
			return "";
		else
			return a_String.substr(0, pos+1);
	}

	tWString cString::GetFilePathW(tWString a_String)
	{
		return L"";
	}

	tString cString::SetFilePath(tString a_String,tString a_Path)
	{
		if(GetLastChar(a_Path) != "/" && GetLastChar(a_Path) != "\\")
			a_Path += "/";
		a_String = GetFileName(a_String);

		return a_Path + a_String;
	}

	tWString cString::SetFilePathW(tWString a_String,tWString a_Path)
	{
		if(GetLastCharW(a_Path) != _W("/") && GetLastCharW(a_Path) != _W("\\"))
			a_Path += _W("/");
		a_String = GetFileNameW(a_String);

		return a_Path + a_String;
	}

	tString cString::GetFileName(tString a_String)
	{
		int pos1 = GetLastStringPos(a_String,"\\");
		int pos2 = GetLastStringPos(a_String,"/");
		int pos = pos1>pos2 ? pos1 : pos2;

		if (pos < 0)
			return a_String;
		else
			return a_String.substr(pos+1);
	}

	tWString cString::GetFileNameW(tWString a_String)
	{
		int pos1 = GetLastStringPosW(a_String,_W("\\"));
		int pos2 = GetLastStringPosW(a_String,_W("/"));
		int pos = pos1>pos2 ? pos1 : pos2;

		if (pos < 0)
			return a_String;
		else
			return a_String.substr(pos+1);
	}

	//--------------------------------------------------------------

	tString cString::ReplaceCharTo(tString a_String, tString a_sOldChar, tString a_sNewChar)
	{
		if (a_sNewChar != "")
		{
			for (int i=0;i<(int)a_String.size();i++)
				if (a_String[i] == a_sOldChar[0]) a_String[i] = a_sNewChar[0];
			return a_String;
		}
		else
		{
			tString sNewString;
			sNewString.reserve(a_String.size());

			for (int i=0;i<(int)a_String.size();i++)
				if (a_String[i] == a_sOldChar[0]) sNewString.push_back(a_String[i]);
			return sNewString;
		}
	}

	tWString cString::ReplaceCharToW(tWString a_String, tWString a_sOldChar, tWString a_sNewChar)
	{
		if (a_sNewChar != _W(""))
		{
			for (int i=0;i<(int)a_String.size();i++)
				if (a_String[i] == a_sOldChar[0]) a_String[i] = a_sNewChar[0];
			return a_String;
		}
		else
		{
			tWString sNewString;
			sNewString.reserve(a_String.size());

			for (int i=0;i<(int)a_String.size();i++)
				if (a_String[i] == a_sOldChar[0]) sNewString.push_back(a_String[i]);
			return sNewString;
		}
	}

	//--------------------------------------------------------------

	tString cString::GetLastChar(tString a_String)
	{
		if (a_String.size()==0) return "";
		return a_String.substr(a_String.size()-1);
	}

	tWString cString::GetLastCharW(tWString a_String)
	{
		if (a_String.size()==0) return _W("");
		return a_String.substr(a_String.size()-1);
	}

	//--------------------------------------------------------------

	tString cString::ToString(const char *a_sString, tString a_sDefault)
	{
		if (a_sString == NULL)
			return a_sDefault;
		return a_sString;
	}

	int cString::ToInt(const char *a_sString, int a_lDefault)
	{
		if (a_sString == NULL) return a_lDefault;

		return atoi(a_sString);
	}

	//--------------------------------------------------------------

	float cString::ToFloat(const char *a_sString, float a_fDefault)
	{
		if (a_sString == NULL) return a_fDefault;

		return (float)atof(a_sString);
	}

	//--------------------------------------------------------------

	bool cString::ToBool(const char *a_sString, bool a_bDefault)
	{
		if (a_sString == NULL) return a_bDefault;

		tString TempString = ToLowerCase(a_sString);
		return TempString == "true"?true:false;
	}

	//--------------------------------------------------------------

	cColor cString::ToColor(const char *a_sString, const cColor &a_Default)
	{
		if (a_sString==NULL) return a_Default;

		tFloatVec vValues;

		GetFloatVec(a_sString, vValues, NULL);
		
		if (vValues.size() != 4) return a_Default;

		return cColor(vValues[0], vValues[1], vValues[2], vValues[3]);
	}

	//--------------------------------------------------------------

	cVector2f cString::ToVector2f(const char *a_sString, const cVector2f &a_vDefault)
	{
		if (a_sString==NULL) return a_vDefault;

		tFloatVec vValues;

		GetFloatVec(a_sString, vValues, NULL);

		if (vValues.size() != 2) return a_vDefault;

		return cVector2f(vValues[0], vValues[1]);
	}

	//--------------------------------------------------------------

	cVector3f cString::ToVector3f(const char *a_sString, const cVector3f &a_vDefault)
	{
		if (a_sString==NULL) return a_vDefault;

		tFloatVec vValues;

		GetFloatVec(a_sString, vValues, NULL);

		if (vValues.size() != 3) return a_vDefault;

		return cVector3f(vValues[0], vValues[1], vValues[2]);
	}

	//--------------------------------------------------------------

	cVector2l cString::ToVector2l(const char *a_sString, const cVector2l &a_vDefault)
	{
		if (a_sString==NULL) return a_vDefault;

		tIntVec vValues;

		GetIntVec(a_sString, vValues, NULL);

		if (vValues.size() != 2) return a_vDefault;

		return cVector2l(vValues[0], vValues[1]);
	}

	//--------------------------------------------------------------

	cVector3l cString::ToVector3l(const char *a_sString, const cVector3l &a_vDefault)
	{
		if (a_sString==NULL) return a_vDefault;

		tIntVec vValues;

		GetIntVec(a_sString, vValues, NULL);

		if (vValues.size() != 2) return a_vDefault;

		return cVector3l(vValues[0], vValues[1], vValues[2]);
	}

	//--------------------------------------------------------------

	tIntVec &cString::GetIntVec(const tString &a_sData, tIntVec &a_vVec, tString *a_pSeparators)
	{
		tStringVec m_vStr;
		GetStringVec(a_sData, m_vStr, a_pSeparators);

		for (int i=0;i<(int)m_vStr.size();i++)
			a_vVec.push_back(ToInt(m_vStr[i].c_str(),0));

		return a_vVec;
	}

	//--------------------------------------------------------------

	tFloatVec &cString::GetFloatVec(const tString &a_sData, tFloatVec &a_vVec, tString *a_pSeparators)
	{
		tStringVec m_vStr;
		GetStringVec(a_sData, m_vStr, a_pSeparators);

		for (int i=0;i<(int)m_vStr.size();i++)
			a_vVec.push_back(ToFloat(m_vStr[i].c_str(),0));
		return a_vVec;
	}

	tString cString::ToString(int a_iX)
	{
		char buff[256];

		sprintf(buff, "%d", a_iX);

		return buff;
	}

	tString cString::ToString(float a_fX)
	{
		char buff[256];

		sprintf(buff, "%f", a_fX);

		return buff;
	}

	//--------------------------------------------------------------

	tWString cString::ToStringW(int a_iX)
	{
		wchar_t buff[256];

		swprintf(buff, _W("%d"), a_iX);

		return buff;
	}

	tWString cString::ToStringW(float a_fX)
	{
		wchar_t buff[256];

		swprintf(buff, _W("%f"), a_fX);

		return buff;
	}

	//--------------------------------------------------------------

	tStringVec &cString::GetStringVec(const tString &a_sData, tStringVec &a_vVec, tString *a_pSeparators)
	{
		tString str = "";
		bool start = false;
		tString c = "";

		for (int i=0; i<(int)a_sData.length();i++)
		{
			c = a_sData.substr(i,1);
			bool bNewWord = false;

			if (a_pSeparators)
			{
				for (size_t j=0; j < a_pSeparators->size(); j++)
				{
					if ((*a_pSeparators)[j] == c[0])
					{
						bNewWord = true;
						break;
					}
				}
			}
			else
			{
				if (c[0]==' ' || c[0]=='\n' || c[0]=='\t' || c[0]==',')
					bNewWord = true;
			}

			if (bNewWord)
			{
				if(start)
				{
					start = false;
					a_vVec.push_back(str);
					str = "";
				}
			}
			else
			{
				start = true;
				str += c;
				if (i==a_sData.length()-1) a_vVec.push_back(str);
			}
		}
		return a_vVec;
	}

	//--------------------------------------------------------------

	int cString::GetLastStringPos(tString a_String, tString a_Char)
	{
		int pos = -1;
		for (int i=0;i<(int)a_String.size();i++)
			if (a_String.substr(i,a_Char.size())==a_Char)
				pos = i;
		return pos;
	}

	int cString::GetLastStringPosW(tWString a_String, tWString a_Char)
	{
		int pos = -1;
		for (int i=0;i<(int)a_String.size();i++)
			if (a_String.substr(i,a_Char.size())==a_Char)
				pos = i;
		return pos;
	}
}