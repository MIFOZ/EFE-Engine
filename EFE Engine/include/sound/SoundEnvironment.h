#ifndef EFE_SOUNDENVIRONMENT_H
#define EFE_SOUNDENVIRONMENT_H

#include "system/String.h"

namespace efe
{
	class cSoundManager;

	class iSoundEnvironment
	{
	public:
		iSoundEnvironment(){}
		virtual ~iSoundEnvironment(){}

		virtual bool CreateFromFile(const tString &a_sFile) {return false;}

		tString &GetName() {return m_sName;}
		tString &GetFileName() {return m_sFileName;}
		void SetFileName(const tString &a_sFileName) {m_sFileName = a_sFileName;}

	protected:
		tString m_sName;
		tString m_sFileName;
	};
};

#endif