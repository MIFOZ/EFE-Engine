#include "input/ActionMouseButton.h"
#include "input/Input.h"
#include "input/Mouse.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cActionMouseButton::cActionMouseButton(tString a_sName, cInput *a_pInput, eMButton a_Button) : iAction(a_sName)
	{
		m_Button = a_Button;
		m_pInput = a_pInput;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cActionMouseButton::IsTriggered()
	{
		return m_pInput->GetMouse()->ButtonIsDown(m_Button);
	}

	//--------------------------------------------------------------

	float cActionMouseButton::GetValue()
	{
		if (IsTriggered())return 1.0;
		else return 0.0;
	}

	tString cActionMouseButton::GetInputName()
	{
		switch(m_Button)
		{
		case eMButton_Left: return "LeftMouse";
		case eMButton_Middle: return "MiddleMouse";
		case eMButton_Right: return "RightMouse";
		case eMButton_WheelUp: return "WheelUp";
		case eMButton_WheelDown: return "WheelDown";
		case eMButton_6: return "Mouse6";
		case eMButton_7: return "Mouse7";
		case eMButton_8: return "Mouse8";
		case eMButton_9: return "Mouse9";
		}
		return "Unknown";
	}
}