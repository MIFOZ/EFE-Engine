#ifndef EFE_GAME_TYPES_H
#define EFE_GAME_TYPES_H

#include <map>

#include "system/SerializeClass.h"

namespace efe
{
	class cScriptVar : public iSerializable
	{
		kSerializableClassInit(cScriptVar)
	public:
		cScriptVar();

		tString m_sName;
		int m_lVal;
	};

	typedef std::map<tString, cScriptVar> tScriptVarMap;
	typedef tScriptVarMap::iterator tScriptVarMapIt;
};
#endif