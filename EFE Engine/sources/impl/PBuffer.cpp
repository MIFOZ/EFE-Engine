#include "impl/PBuffer.h"
#include "impl/LowLevelGraphicsDX.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPBuffer::cPBuffer(iLowLevelGraphics *a_pLowLevelGraphics, bool a_bShareObjects, bool a_bUseMipMaps, bool a_bUseDepth, bool a_bUseStencil)
	{
		m_bShareObjects =  a_bShareObjects;
		m_pLowLevelGraphics = a_pLowLevelGraphics;

		//m_vAttribBuffer.push_back(
		m_vAttribBuffer.push_back(true);


	}
}