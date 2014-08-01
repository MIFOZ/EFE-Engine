#ifndef EFE_JOYSTICK_RAW_H
#define EFE_JOYSTICK_RAW_H

#include "system/SystemTypes.h"
#include "input/Joystick.h"

namespace efe
{
	
#define MAX_KEY_PRESSES (20)

	class cLowLevelInputRaw;

	class cJoystickRaw : public iJoystick
	{
	public:
		cJoystickRaw(cLowLevelInputRaw *a_pLowLevelInputRaw);

		void Update();

	private:
		bool m_bInitialized;

		std::vector<bool> m_vKeyArray;

		std::list<cKeyPress> m_lstKeysPressed;

		cLowLevelInputRaw *m_pLowLevelInputRaw;
	};

};

#endif