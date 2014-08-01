#ifndef EFE_INPUTDEVICE_H
#define EFE_INPUTDEVICE_H

#include "system/SystemTypes.h"
#include "input/InputTypes.h"

namespace efe
{
	class iInputDevice
	{
	public:
		iInputDevice(tString a_sName, eInputDeviceType a_Type);
		virtual ~iInputDevice(){}

		tString GetName();

		eInputDeviceType GetType();

		virtual void Update()=0;
	private:
		tString m_sName;
		eInputDeviceType m_Type;
	};
};

#endif