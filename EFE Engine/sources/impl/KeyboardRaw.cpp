#include "impl/KeyboardRaw.h"

#include "impl/LowLevelInputRaw.h"
#include "impl/LowLevelSystemWin.h"

#include <conio.h>

namespace efe
{
	cKeyboardRaw::cKeyboardRaw(cLowLevelInputRaw *a_pLowLevelInputRaw) : iKeyboard("Raw Keyboard")
	{
		m_pLowLevelInputRaw = a_pLowLevelInputRaw;

		m_vKeyArray.resize(eKey_LastEnum);
		ClearKeyList();
	}

	void cKeyboardRaw::Update()
	{
		m_lstKeysPressed.clear();

		for (int i=0; i<(int)m_pLowLevelInputRaw->m_lBufferIndex; i++)
		{
			RAWINPUT *raw = &m_pLowLevelInputRaw->m_vInputs[i];

			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				eKey key = CharToKey(raw->data.keyboard.VKey);
				if (raw->data.keyboard.Flags & RI_KEY_BREAK)
					m_vKeyArray[key] = false;
				else 
					m_vKeyArray[key] = true;

				if (raw->data.keyboard.Message == WM_KEYDOWN)
				{
					HKL keyboardLayout = GetKeyboardLayout(0);
					unsigned char keyboardState[256];
					GetKeyboardState(keyboardState);
					unsigned int scancode = MapVirtualKey(raw->data.keyboard.VKey, MAPVK_VK_TO_VSC);
					wchar_t buffer;
					ToUnicodeEx(raw->data.keyboard.VKey,scancode,keyboardState,&buffer,1,0,keyboardLayout);

					m_Modifier = eKeyModifier_NONE;

					if (KeyIsDown(eKey_LCTRL)) m_Modifier |= eKeyModifier_CTRL;
					if (KeyIsDown(eKey_LSHIFT)) m_Modifier |= eKeyModifier_SHIFT;
					if (KeyIsDown(eKey_LALT)) m_Modifier |= eKeyModifier_ALT;

					m_lstKeysPressed.push_back(cKeyPress(key, (int)buffer, m_Modifier));
				}
				
			}
		}
	}

	bool cKeyboardRaw::KeyIsDown(eKey a_Key)
	{
		return m_vKeyArray[a_Key];
	}

	cKeyPress cKeyboardRaw::GetKey()
	{
		cKeyPress key = m_lstKeysPressed.front();
		m_lstKeysPressed.pop_front();
		return key;
	}

	bool cKeyboardRaw::KeyIsPressed()
	{
		return m_lstKeysPressed.empty()==false;
	}

	eKeyModifier cKeyboardRaw::GetModifier()
	{
		return m_Modifier;
	}

	tString cKeyboardRaw::KeyToString(eKey)
	{
		return "None";
	}

	eKey cKeyboardRaw::StringToKey(tString)
	{
		return eKey_NONE;
	}

	eKey cKeyboardRaw::CharToKey(unsigned short a_sKey)
	{
		switch(a_sKey)
		{
		case VK_BACK: return eKey_BACKSPACE;
		case 0x1B: return eKey_ESCAPE;

		case 0x30: return eKey_0;
		case 0x31: return eKey_1;
		case 0x32: return eKey_2;
		case 0x33: return eKey_3;
		case 0x34: return eKey_4;
		case 0x35: return eKey_5; 
		case 0x36: return eKey_6;
		case 0x37: return eKey_7;
		case 0x38: return eKey_8;
		case 0x39: return eKey_9;

		case 0x41: return eKey_a;
		case 0x42: return eKey_b;
		case 0x43: return eKey_c; 
		case 0x44: return eKey_d;
		case 0x45: return eKey_e;
		case 0x46: return eKey_f; 
		case 0x47: return eKey_g;
		case 0x48: return eKey_h;
		case 0x49: return eKey_i; 
		case 0x4A: return eKey_j;
		case 0x4B: return eKey_k;
		case 0x4C: return eKey_l;
		case 0x4D: return eKey_m; 
		case 0x4E: return eKey_n;
		case 0x4F: return eKey_o;
		case 0x50: return eKey_p;
		case 0x51: return eKey_q;
		case 0x52: return eKey_r;
		case 0x53: return eKey_s;
		case 0x54: return eKey_t;
		case 0x55: return eKey_u;
		case 0x56: return eKey_v;
		case 0x57: return eKey_w;
		case 0x58: return eKey_x;
		case 0x59: return eKey_y;
		case 0x5A: return eKey_z;
		case VK_DELETE: return eKey_DELETE;
		case VK_UP: return eKey_UP;
		case VK_DOWN: return eKey_DOWN;
		case VK_LEFT: return eKey_LEFT;
		case VK_INSERT: return eKey_INSERT;
		case VK_HOME: return eKey_HOME;
		case VK_END: return eKey_END;
		case VK_RIGHT: return eKey_RIGHT;
		case VK_RSHIFT: return eKey_RSHIFT;
		case VK_SHIFT: return eKey_LSHIFT;
		case VK_RCONTROL: return eKey_RCTRL;
		case VK_CONTROL: return eKey_LCTRL;
		}
		return eKey_NONE;
	}
	
	void cKeyboardRaw::ClearKeyList()
	{
		m_vKeyArray.assign(m_vKeyArray.size(), false);
	}
}