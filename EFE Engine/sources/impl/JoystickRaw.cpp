#include "impl/JoystickRaw.h"
#include "impl/LowLevelInputRaw.h"

#include <hidsdi.h>

#pragma comment (lib, "hid.lib")

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cJoystickRaw::cJoystickRaw(cLowLevelInputRaw *a_pLowLevelInputRaw) : iJoystick("Raw Joystick")
	{
		m_pLowLevelInputRaw = a_pLowLevelInputRaw;

		m_bInitialized = false;

		m_vKeyArray.resize(eKey_LastEnum);
		//ClearKeyList();
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cJoystickRaw::Update()
	{
		for (int i=0; i<(int)m_pLowLevelInputRaw->m_lBufferIndex; i++)
		{
			RAWINPUT *pRawInput = &m_pLowLevelInputRaw->m_vInputs[i];

			unsigned lSize;
			GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, NULL, &lSize);
			PHIDP_PREPARSED_DATA pPreparsedData = (PHIDP_PREPARSED_DATA)efeNewArray(char, lSize);
			GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &lSize);
			if (m_bInitialized)
			{
				HIDP_CAPS Caps;
				//HidP_GetCaps(pPreparsedData, &Caps);
				PHIDP_BUTTON_CAPS pButtonCaps = efeNewArray(HIDP_BUTTON_CAPS, Caps.NumberInputButtonCaps);

				m_bInitialized = true;
			}

			
		}
	}
}