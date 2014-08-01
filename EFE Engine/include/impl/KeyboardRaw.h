#ifndef EFE_KEYBOARD_RAW_H
#define EFE_KEYBOARD_RAW_H

#include <vector>
#include <list>
#include "system/SystemTypes.h"
#include "input/Keyboard.h"

namespace efe
{
	
#define MAX_KEY_PRESSES (20)

	class cLowLevelInputRaw;

	class cKeyboardRaw : public iKeyboard
	{
	public:
		cKeyboardRaw(cLowLevelInputRaw *a_pLowLevelInputRaw);

		void Update();

		bool KeyIsDown(eKey a_Key);
		cKeyPress GetKey();
		bool KeyIsPressed();
		eKeyModifier GetModifier();
		tString KeyToString(eKey);
		eKey StringToKey(tString);
	private:
		eKey CharToKey(unsigned short a_sKey);
		void ClearKeyList();
		eKeyModifier m_Modifier;

		std::vector<bool> m_vKeyArray;

		std::list<cKeyPress> m_lstKeysPressed;

		cLowLevelInputRaw *m_pLowLevelInputRaw;
	};

};

#endif