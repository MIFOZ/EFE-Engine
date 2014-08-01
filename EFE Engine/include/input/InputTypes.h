#ifndef EFE_INPUT_TYPES_H
#define EFE_INPUT_TYPES_H

namespace efe
{
	enum eInputDeviceType
	{
		eInputDeviceType_Keyboard,
		eInputDeviceType_Mouse,
		eInputDeviceType_Joystick,
		eInputDeviceType_LastEnum
	};

	enum eMButton
	{
		eMButton_Left,
		eMButton_Middle,
		eMButton_Right,
		eMButton_WheelUp,
		eMButton_WheelDown,
		eMButton_6,
		eMButton_7,
		eMButton_8,
		eMButton_9,
		eMButton_LastEnum
	};

	enum eKey
	{
		eKey_BACKSPACE,
		eKey_TAB,
		eKey_CLEAR,
		eKey_RETURN,
		eKey_PAUSE,
		eKey_ESCAPE,
		eKey_SPACE,
		eKey_EXCLAIM,
		eKey_QUOTEDBL,
		eKey_HASH,
		eKey_DOLLAR,
		eKey_AMPERSAND,
		eKey_QUOTE,
		eKey_LEFTPAREN,
		eKey_RIGHTPAREN,
		eKey_ASTERISK,
		eKey_PLUS,
		eKey_COMMA,
		eKey_MINUS,
		eKey_PERIOD,
		eKey_SLASH,
		eKey_0,
		eKey_1,
		eKey_2,
		eKey_3,
		eKey_4,
		eKey_5,
		eKey_6,
		eKey_7,
		eKey_8,
		eKey_9,
		eKey_COLON,
		eKey_SEMICOLON,
		eKey_LESS,
		eKey_EQUALS,
		eKey_GREATER,
		eKey_QUASTION,
		eKey_AT,
		eKey_LEFTBRACKET,
		eKey_BACKSLASH,
		eKey_RIGHTBRACKET,
		eKey_CARET,
		eKey_UNDERSCORE,
		eKey_BACKQUOTE,
		eKey_a,
		eKey_b,
		eKey_c,
		eKey_d,
		eKey_e,
		eKey_f,
		eKey_g,
		eKey_h,
		eKey_i,
		eKey_j,
		eKey_k,
		eKey_l,
		eKey_m,
		eKey_n,
		eKey_o,
		eKey_p,
		eKey_q,
		eKey_r,
		eKey_s,
		eKey_t,
		eKey_u,
		eKey_v,
		eKey_w,
		eKey_x,
		eKey_y,
		eKey_z,
		eKey_DELETE,
		eKey_UP,
		eKey_DOWN,
		eKey_LEFT,
		eKey_RIGHT,
		eKey_RSHIFT,
		eKey_INSERT,
		eKey_HOME,
		eKey_END,
		eKey_LSHIFT,
		eKey_RCTRL,
		eKey_LCTRL,
		eKey_RALT,
		eKey_LALT,
		eKey_NONE,
		eKey_LastEnum
	};
	#define eKey_MASK 0x000ff

	typedef unsigned int eKeyModifier;

	enum eKeyModifier_Enums
	{
		eKeyModifier_NONE		= 0x00000,
		eKeyModifier_CTRL		= 0x00100,
		eKeyModifier_SHIFT		= 0x00200,
		eKeyModifier_ALT		= 0x00400,
		eKeyModifier_META		= 0x00800,
		eKeyModifier_LastEnum	= 5
	};
	#define eKeyModifier_MASK 0x00f00

	struct cKeyPress
	{
		cKeyPress(){}
		cKeyPress(eKey a_Key, int a_lUnicode, int a_lModifier)
			: m_Key(a_Key), m_lUnicode(a_lUnicode), m_lModifier(a_lModifier) {}

		eKey m_Key;
		int m_lUnicode;
		int m_lModifier;
	};
};
#endif