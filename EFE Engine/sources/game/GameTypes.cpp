#include "game/GameTypes.h"

#include "system/LowLevelSystem.h"

namespace efe
{
	cScriptVar::cScriptVar()
	{
		
	}

	kBeginSerializeBase(cScriptVar)
		kSerializeVar(m_sName, eSerializeType_String)
		kSerializeVar(m_lVal, eSerializeType_Int32)
	kEndSerialize()
}