#include "impl/LowLevelInputRaw.h"
#include "impl/LowLevelGraphicsDX11.h"

#include "impl/MouseRaw.h"
#include "impl/KeyboardRaw.h"
#include "impl/JoystickRaw.h"

#include "system/LowLevelSystem.h"

namespace efe
{
	cLowLevelInputRaw::cLowLevelInputRaw(iLowLevelGraphics *a_pLowLevelGraphics)
	{
		m_pLowLevelGraphics = a_pLowLevelGraphics;
		LockInput(true);

		RAWINPUTDEVICE InputDevices[3];
		InputDevices[0].dwFlags = 0;
		InputDevices[0].hwndTarget = 0;
		InputDevices[0].usUsage = 0x02; // Mouse
		InputDevices[0].usUsagePage = 0x01;

		InputDevices[1].dwFlags = 0;
		InputDevices[1].hwndTarget = 0;
		InputDevices[1].usUsage = 0x06; // Keyboard
		InputDevices[1].usUsagePage = 0x01;

		InputDevices[2].dwFlags = 0;
		InputDevices[2].hwndTarget = 0;
		InputDevices[2].usUsage = 0x04; // Joystick
		InputDevices[2].usUsagePage = 0x01;

		bool w = RegisterRawInputDevices(InputDevices, 3, sizeof(InputDevices[0]));

		m_vInputs.resize(40);

		m_lBufferIndex = 0;
	}

	cLowLevelInputRaw::~cLowLevelInputRaw()
	{
	
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cLowLevelInputRaw::LockInput(bool a_bX)
	{
		if (a_bX)
		{
			RECT Bounds;
			GetWindowRect(NULL, &Bounds);
			ClipCursor(&Bounds);

			POINT Pos;
			Pos.x = (int)m_pLowLevelGraphics->GetScreenSize().x/2;
			Pos.y = (int)m_pLowLevelGraphics->GetScreenSize().y/2;
			ClientToScreen(NULL, &Pos);
			SetCursorPos(Pos.x, Pos.y);
		}
		else
		{
			ClipCursor(NULL);

			POINT Pos;
			Pos.x = m_pLowLevelGraphics->GetScreenSize().x/2;
			Pos.y = m_pLowLevelGraphics->GetScreenSize().y/2;
			ClientToScreen(NULL, &Pos);
			//SetCursorPos(Pos.x, Pos.y);
		}
	}

	//--------------------------------------------------------------

	void cLowLevelInputRaw::BeginInputUpdate()
	{
		MSG msg;
		m_lBufferIndex = 0;
		while(PeekMessage(&msg, NULL, WM_INPUT, WM_INPUT, PM_REMOVE))
		{
			HRAWINPUT hRawInput = reinterpret_cast<HRAWINPUT>(msg.lParam);

			PRAWINPUT pRawInput = reinterpret_cast<PRAWINPUT>(&hRawInput);

			UINT dbSize;

			if (m_lBufferIndex >= m_vInputs.size())
				m_vInputs.resize(m_vInputs.size() * 2);

			GetRawInputData(hRawInput, RID_INPUT, &m_vInputs[m_lBufferIndex], &dbSize, sizeof(RAWINPUTHEADER));
			m_lBufferIndex++;

			DefRawInputProc(&pRawInput, 1, sizeof(RAWINPUTHEADER));
		}
	}

	void cLowLevelInputRaw::EndInputUpdate()
	{
		memset(m_vInputs.data(), 0, sizeof(m_vInputs.data()));
	}

	iMouse *cLowLevelInputRaw::CreateMouse()
	{
		return efeNew(cMouseDI,(this, m_pLowLevelGraphics));
	}

	iKeyboard *cLowLevelInputRaw::CreateKeyboard()
	{
		return efeNew(cKeyboardRaw,(this));
	}

	iJoystick *cLowLevelInputRaw::CreateJoystick()
	{
		return efeNew(cJoystickRaw,(this));
	}
}