#ifndef EFE_ACTIONMOUSEBUTTON_H
#define EFE_ACTIONMOUSEBUTTON_H

#include "input/InputTypes.h"
#include "input/Action.h"

namespace efe
{
	class cInput;

	class cActionMouseButton : public iAction
	{
	public:
		cActionMouseButton(tString a_sName, cInput *a_pInput, eMButton a_Button);

		bool IsTriggered();
		float GetValue();

		tString GetInputName();

		tString GetInputType(){return "MouseButton";}

		eMButton GetButton(){return m_Button;}

	private:
		eMButton m_Button;
		cInput *m_pInput;
	};
};
#endif