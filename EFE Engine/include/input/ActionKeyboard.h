#ifndef EFE_ACTIONKEYBOARD_H
#define EFE_ACTIONKEYBOARD_H

#include "input/InputTypes.h"
#include "input/Action.h"

namespace efe
{
	class cInput;

	class cActionKeyboard : public iAction
	{
	public:
		cActionKeyboard(tString a_sName, cInput *a_pInput, int a_Key);

		bool IsTriggered();
		float GetValue();

		tString GetInputName();

		tString GetInputType(){return "Keyboard";}

		eKey GetKey(){return m_Key;}
		eKeyModifier GetModifier(){return m_Mod;}
	private:
		eKey m_Key;
		eKeyModifier m_Mod;
		cInput *m_pInput;
	};
};

#endif