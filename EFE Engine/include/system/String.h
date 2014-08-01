#ifndef EFE_STRING_H
#define EFE_STRING_H

#include "system/SystemTypes.h"

#include "graphics/GraphicsTypes.h"

namespace efe
{
	class cString
	{
	public:
		static tWString To16Char(const tString &a_sString);

		static tString To8Char(const tWString &a_wsString);

		static tString Sub(const tString &a_sString, int a_lStart, int a_lCount = -1);
		static tWString SubW(const tWString &a_sString, int a_lStart, int a_lCount = -1);

		static tString GetFileExt(tString a_String);
		static tWString GetFileExtW(tWString a_String);

		static tString SetFileExt(tString a_String, tString a_Ext);
		static tWString SetFileExtW(tWString a_String, tWString a_Ext);

		static tString GetFileName(tString a_String);
		static tWString GetFileNameW(tWString a_String);

		static tString GetFilePath(tString a_String);
		static tWString GetFilePathW(tWString a_String);

		static tString SetFilePath(tString a_String,tString a_Path);
		static tWString SetFilePathW(tWString a_String, tWString a_Path);

		static tString ToLowerCase(tString a_Strng);
		static tWString ToLowerCaseW(tWString a_String);

		static tString ReplaceCharTo(tString a_String, tString a_sOldChar, tString a_sNewChar);
		static tWString ReplaceCharToW(tWString a_String, tWString a_sOldChar, tWString a_sNewChar);


		static tString ToString(const char *a_sString, const tString a_sDefault);
		static int ToInt(const char *a_sString, int a_lDefault);
		static bool ToBool(const char *a_sString, bool a_bDefault);
		static float ToFloat(const char *a_sString, float a_fDefault);
		static cColor ToColor(const char *a_sString, const cColor &a_Default);
		static cVector2f ToVector2f(const char *a_sString, const cVector2f &a_vDefault);
		static cVector3f ToVector3f(const char *a_sString, const cVector3f &a_vDefault);
		static cVector2l ToVector2l(const char *a_sString, const cVector2l &a_vDefault);
		static cVector3l ToVector3l(const char *a_sString, const cVector3l &a_vDefault);

		static tString ToString(int a_iX);
		static tString ToString(float a_fX);

		static tWString ToStringW(int a_iX);
		static tWString ToStringW(float a_fX);

		static tIntVec &GetIntVec(const tString &a_sData, tIntVec &a_vVec, tString *a_pSeparators=NULL);
		static tFloatVec &GetFloatVec(const tString &a_sData, tFloatVec &a_vVec, tString *a_pSeparators=NULL);
		static tStringVec &GetStringVec(const tString &a_sData, tStringVec &a_vVec, tString *a_pSeparators=NULL);

		static tString GetLastChar(tString a_String);
		static tWString GetLastCharW(tWString a_String);

		static int GetLastStringPos(tString a_String, tString a_Char);
		static int GetLastStringPosW(tWString a_String, tWString a_Char);
	};
};

#endif