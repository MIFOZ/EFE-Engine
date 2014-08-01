#ifndef EFE_LOWLEVELINPUT_RAW_H
#define EFE_LOWLEVELINPUT_RAW_H

#include <vector>
#include "input/LowLevelInput.h"
#include <windows.h>

namespace efe
{
	class iLowLevelGraphics;
	
	class cLowLevelInputRaw : public iLowLevelInput
	{
	public:
		cLowLevelInputRaw(iLowLevelGraphics *a_pLowLevelGraphics);
		~cLowLevelInputRaw();

		void LockInput(bool a_bX);

		void BeginInputUpdate();
		void EndInputUpdate();

		iMouse *CreateMouse();
		iKeyboard *CreateKeyboard();
		iJoystick *CreateJoystick();

	public:
		std::vector<RAWINPUT> m_vInputs;
		unsigned int m_lBufferIndex;

	private:
		iLowLevelGraphics *m_pLowLevelGraphics;
	};
};
#endif