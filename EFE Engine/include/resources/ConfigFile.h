#ifndef EFE_CONFIGFILE_H
#define EFE_CONFIGFILE_H

#include "system/SystemTypes.h"

class TiXmlDocument;
class cFileSearcher;

namespace efe
{
	class cConfigFile
	{
	public:
		cConfigFile(tWString a_sFile);
		~cConfigFile();

		bool Load();

		bool Save();

		void SetString(tString a_sLevel, tString a_sName, tString a_sVal);
		void SetInt(tString a_sLevel, tString a_sName, int a_lVal);
		void SetFloat(tString a_sLevel, tString a_sName, float a_fVal);
		void SetBool(tString a_sLevel, tString a_sName, bool a_bVal);

		tString GetString(tString a_sLevel, tString a_sName, tString a_sDefault);
		int GetInt(tString a_sLevel, tString a_sName, int a_lDefault);
		float GetFloat(tString a_sLevel, tString a_sName, float a_fDefault);
		bool GetBool(tString a_sLevel, tString a_sName, bool a_bDefault);

	private:
		tWString m_sFile;
		cFileSearcher *m_pFileSearcher;

		TiXmlDocument *m_pXmlDoc;

		const char *GetCharArray(tString a_sLevel, tString a_sName);
	};
};
#endif