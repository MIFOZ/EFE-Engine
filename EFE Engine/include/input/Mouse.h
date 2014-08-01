#ifndef EFE_MOUSE_H
#define EFE_MOUSE_H

#include "input/InputTypes.h"
#include "input/InputDevice.h"
#include "math/MathTypes.h"

namespace efe
{
	class iMouse : public iInputDevice
	{
	public:
		iMouse(tString a_sName);
		virtual ~iMouse(){}

		virtual bool ButtonIsDown(eMButton)=0;

		virtual cVector2f GetAbsPosition()=0;

		virtual cVector2f GetRelPosition()=0;

		virtual void Reset()=0;

		//virtual void SetSmoothProperties(float a_fminPercent,
									//	float a_fMaxPercent, unsigned int a_lBufferSize)= 0;
	};
};

#endif