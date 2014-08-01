#ifndef EFE_JOYSTICK_H
#define EFE_JOYSTICK_H

#include "input/InputTypes.h"
#include "input/InputDevice.h"

namespace efe
{
	class iJoystick : public iInputDevice
	{
	public:
		iJoystick(tString a_sName);
		virtual ~iJoystick(){}
	};
};

#endif