#include "impl/MouseRaw.h"

#include "graphics/LowLevelGraphics.h"
#include "impl/LowLevelInputRaw.h"

namespace efe
{
	cMouseDI::cMouseDI(cLowLevelInputRaw *a_pLowLevelInputImpl, iLowLevelGraphics *a_pLowLevelGraphics) : iMouse("Raw Mouse")
	{
		m_fMaxPercent = 0.7f;
		m_fMinPercent = 0.1f;
		m_lBufferSize = 6;

		m_vMButtonArray.resize(eMButton_LastEnum);
		m_vMButtonArray.assign(m_vMButtonArray.size(), false);

		m_pLowLevelInputRaw = a_pLowLevelInputImpl;
		m_pLowLevelGraphics = a_pLowLevelGraphics;

		m_vMouseRelPos = cVector2f(0,0);
		m_vMouseAbsPos = cVector2f(0,0);

		m_bWheelUpMoved = false;
		m_bWheelDownMoved = false;
	}

	void cMouseDI::Update()
	{
		cVector2f vScreenSize = m_pLowLevelGraphics->GetScreenSize();
		cVector2f vVirtualSize = m_pLowLevelGraphics->GetVirtualSize();

		m_bWheelUpMoved = false;
		m_bWheelDownMoved = false;

		for (int i=0; i<(int)m_pLowLevelInputRaw->m_lBufferIndex; i++)
		{
			RAWINPUT *raw = /*(RAWINPUT *)*/&m_pLowLevelInputRaw->m_vInputs[i];/*m_pLowLevelInputImpl->m_pBuffer[i];*/

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				RAWMOUSE rawMouse = raw->data.mouse;
				if (rawMouse.usButtonFlags == 0x0)
				{
					//m_vMouseAbsPos += cVector2f((float)raw->data.mouse.lLastX, (float)raw->data.mouse.lLastY);

					POINT vPos;
					GetCursorPos(&vPos);
					//GetActiveWindow();
					ScreenToClient(GetActiveWindow(), &vPos);

					m_vMouseAbsPos = cVector2f((float)vPos.x, (float)vPos.y);

					m_vMouseAbsPos = (m_vMouseAbsPos/vScreenSize) * vVirtualSize;

					//POINT vPos = {(int)vScreenSize.x/2, (int)vScreenSize.y/2};
					//ClientToScreen(Get
					//SetCursorPos((int)vScreenSize.x/2, (int)vScreenSize.y/2);

					m_vMouseRelPos = cVector2f((float)raw->data.mouse.lLastX, (float)raw->data.mouse.lLastY);
					m_vMouseRelPos = (m_vMouseRelPos/vScreenSize)*vVirtualSize;
				}
				else
				{
					switch(rawMouse.usButtonFlags)
					{
					case RI_MOUSE_LEFT_BUTTON_UP: m_vMButtonArray[eMButton_Left] = false;break;
					case RI_MOUSE_LEFT_BUTTON_DOWN: m_vMButtonArray[eMButton_Left] = true;break;
					case RI_MOUSE_MIDDLE_BUTTON_UP: m_vMButtonArray[eMButton_Middle] = false;break;
					case RI_MOUSE_MIDDLE_BUTTON_DOWN: m_vMButtonArray[eMButton_Middle] = true;break;
					case RI_MOUSE_RIGHT_BUTTON_UP: m_vMButtonArray[eMButton_Right] = false;break;
					case RI_MOUSE_RIGHT_BUTTON_DOWN: m_vMButtonArray[eMButton_Right] = true;break;
					case RI_MOUSE_WHEEL:
						{
							if (rawMouse.usButtonData > 0.0)
								m_vMButtonArray[eMButton_WheelUp] = true;
							else
								m_vMButtonArray[eMButton_WheelDown] = true;
						}
					}

					bool bButtonIsDown = (rawMouse.usButtonFlags & 0x15);

					int i = RI_MOUSE_BUTTON_1_UP;
					/*switch(rawMouse.usButtonFlags)
					{
					case 
					}*/
				}
			}
		}
	}

	bool cMouseDI::ButtonIsDown(eMButton mButton)
	{
		return m_vMButtonArray[mButton];
	}

	cVector2f cMouseDI::GetAbsPosition()
	{
		cVector2f vPos = m_vMouseAbsPos;

		return vPos;
	}

	cVector2f cMouseDI::GetRelPosition()
	{
		cVector2f vPos = m_vMouseRelPos;

		m_vMouseRelPos = cVector2f(0,0);

		return vPos;
	}

	void cMouseDI::Reset()
	{
		m_lstMouseCoord.clear();
		m_vMouseRelPos = cVector2f(0, 0);
	}
}