#ifndef EFE_KEYBOARD_H
#define EFE_KEYBOARD_H

#include "input/InputTypes.h"
#include "input/InputDevice.h"

namespace efe
{
	class iKeyboard : public iInputDevice
	{
	public:
		iKeyboard(tString a_sName);
		virtual ~iKeyboard(){}

		virtual bool KeyIsDown(eKey a_Key)=0;

		virtual cKeyPress GetKey()=0;

		virtual bool KeyIsPressed()=0;

		virtual eKeyModifier GetModifier()=0;

		virtual tString KeyToString(eKey)=0;

		virtual eKey StringToKey(tString)=0;
	};
};

#endif