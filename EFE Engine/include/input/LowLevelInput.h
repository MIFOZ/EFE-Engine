#ifndef EFE_LOWLEVELINPUT_H
#define EFE_LOWLEVELINPUT_H

namespace efe
{
	class iMouse;
	class iKeyboard;
	class iJoystick;

	class iLowLevelInput
	{
	public:
		virtual ~iLowLevelInput(){}

		virtual void LockInput(bool a_bX)=0;
		virtual void BeginInputUpdate()=0;
		virtual void EndInputUpdate()=0;

		virtual iMouse *CreateMouse()=0;
		virtual iKeyboard *CreateKeyboard()=0;
		virtual iJoystick *CreateJoystick()=0;
	};
};
#endif