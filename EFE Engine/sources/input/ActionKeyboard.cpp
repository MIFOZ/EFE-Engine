#include "input/ActionKeyboard.h"
#include "input/Input.h"
#include "input/Keyboard.h"

#include "system/LowLevelSystem.h"

namespace efe
{
	cActionKeyboard::cActionKeyboard(tString a_sName, cInput *a_pInput, int a_Key) : iAction(a_sName)
	{
		m_Key = (eKey)(a_Key & eKey_MASK);
		m_Mod = (eKeyModifier)(a_Key & eKeyModifier_MASK);
		m_pInput = a_pInput;
	}

	bool cActionKeyboard::IsTriggered()
	{
		return m_pInput->GetKeyboard()->KeyIsDown(m_Key);// &&
			//((m_pInput->GetKeyboard()->GetModifier() & m_Mod) > 0 || m_Mod == eKeyModifier_NONE);
	}

	float cActionKeyboard::GetValue()
	{
		return 1;
	}

	tString cActionKeyboard::GetInputName()
	{
		tString t_sKey = "";
		if (m_Mod & eKeyModifier_SHIFT){
			t_sKey.append("Shift");
		}
		if (m_Mod & eKeyModifier_ALT){
			t_sKey.append("Alt");
		}
		if (m_Mod & eKeyModifier_CTRL){
			t_sKey.append("Control");
		}
		if (m_Mod & eKeyModifier_META){
			t_sKey.append("Windows");
		}

		switch(m_Key)
		{
		case eKey_BACKSPACE: t_sKey.append("Backspace");break;
		case eKey_TAB: t_sKey.append("Tab");break;
		case eKey_CLEAR: t_sKey.append("Clear");break;
		case eKey_RETURN: t_sKey.append("Return");break;
		case eKey_PAUSE: t_sKey.append("Pause");break;
		case eKey_ESCAPE: t_sKey.append("Escape");break;
		case eKey_SPACE: t_sKey.append("Space");break;
		case eKey_EXCLAIM: t_sKey.append("Exclaim");break;
		case eKey_QUOTEDBL: t_sKey.append("DblQuote");break;
		case eKey_HASH: t_sKey.append("Hash");break;
		case eKey_DOLLAR: t_sKey.append("Dollar");break;
		case eKey_AMPERSAND: t_sKey.append("Ampersand");break;
		case eKey_QUOTE: t_sKey.append("Quote");break;
		case eKey_LEFTPAREN: t_sKey.append("LeftParent");break;
		case eKey_RIGHTPAREN: t_sKey.append("RightParent");break;
		case eKey_ASTERISK: t_sKey.append("Asterisk");break;
		case eKey_PLUS: t_sKey.append("Plus");break;
		case eKey_COMMA: t_sKey.append("Comma");break;
		case eKey_MINUS: t_sKey.append("Minus");break;
		case eKey_PERIOD: t_sKey.append("Period");break;
		case eKey_SLASH: t_sKey.append("Slash");break;
		case eKey_0: t_sKey.append("0");break;
		case eKey_1: t_sKey.append("1");break;
		case eKey_2: t_sKey.append("2");break;
		case eKey_3: t_sKey.append("3");break;
		case eKey_4: t_sKey.append("4");break;
		case eKey_5: t_sKey.append("5");break;
		case eKey_6: t_sKey.append("6");break;
		case eKey_7: t_sKey.append("7");break;
		case eKey_8: t_sKey.append("8");break;
		case eKey_9: t_sKey.append("9");break;
		case eKey_COLON: t_sKey.append("Colon");break;
		case eKey_SEMICOLON: t_sKey.append("SemiColon");break;
		case eKey_LESS: t_sKey.append("Less");break;
		case eKey_EQUALS: t_sKey.append("Equals");break;
		case eKey_GREATER: t_sKey.append("Greater");break;
		case eKey_QUASTION: t_sKey.append("Question");break;
		case eKey_AT: t_sKey.append("At");break;
		case eKey_LEFTBRACKET: t_sKey.append("LeftBracket");break;
		case eKey_BACKSLASH: t_sKey.append("BackSlash");break;
		case eKey_RIGHTBRACKET: t_sKey.append("RightBracket");break;
		case eKey_CARET: t_sKey.append("Caret");break;
		case eKey_UNDERSCORE: t_sKey.append("Underscore");break;
		case eKey_BACKQUOTE: t_sKey.append("BackQuote");break;
		case eKey_a: t_sKey.append("A");break;
		case eKey_b: t_sKey.append("B");break;
		case eKey_c: t_sKey.append("C");break;
		case eKey_d: t_sKey.append("D");break;
		case eKey_e: t_sKey.append("E");break;
		case eKey_f: t_sKey.append("F");break;
		case eKey_g: t_sKey.append("G");break;
		case eKey_h: t_sKey.append("H");break;
		case eKey_i: t_sKey.append("I");break;
		case eKey_j: t_sKey.append("J");break;
		case eKey_k: t_sKey.append("K");break;
		case eKey_l: t_sKey.append("L");break;
		case eKey_m: t_sKey.append("M");break;
		case eKey_n: t_sKey.append("N");break;
		case eKey_o: t_sKey.append("O");break;
		case eKey_p: t_sKey.append("P");break;
		case eKey_q: t_sKey.append("Q");break;
		case eKey_r: t_sKey.append("R");break;
		case eKey_s: t_sKey.append("S");break;
		case eKey_t: t_sKey.append("T");break;
		case eKey_u: t_sKey.append("U");break;
		case eKey_v: t_sKey.append("V");break;
		case eKey_w: t_sKey.append("W");break;
		case eKey_x: t_sKey.append("X");break;
		case eKey_y: t_sKey.append("Y");break;
		case eKey_z: t_sKey.append("Z");break;
		case eKey_LSHIFT: t_sKey.append("LeftShift");break;
		case eKey_LCTRL: t_sKey.append("LeftControl");break;
		}
		if (t_sKey != "")
			return t_sKey;
		else return "Unknown";
	}
}