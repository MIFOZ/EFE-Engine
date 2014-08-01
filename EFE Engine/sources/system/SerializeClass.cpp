#include "system/SerializeClass.h"

#include "system/LowLevelSystem.h"

#include "system/String.h"

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"
#include "system/Container.h"

#include "stdio.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// SERIALIZE CLASS
	//////////////////////////////////////////////////////////////

	int g_lSerializeTempClassesNum = 0;

	bool cSerializeClass::m_bDataSetup = false;
	tString cSerializeClass::m_sTempString = "";
	char cSerializeClass::m_sTempCharArray[2048];

	cSerializeClass::cSerializeClass(const char *a_sName, const char *a_sParent, cSerializeMemberField *a_pMemberFields,
							size_t a_lSize, iSerializable *(*a_pCreateFunc)())
	{
		
	}
}