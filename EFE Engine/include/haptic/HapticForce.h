#ifndef EFE_HAPTIC_FORCE_H
#define EFE_HAPTIC_FORCE_H

#include "math/MathTypes.h"
#include "haptic/HapticTypes.h"

namespace efe
{
	class iHapticForce
	{
	public:
		iHapticForce(eHapticForceType a_Type) : m_Type(a_Type) {}
		virtual ~iHapticForce() {}

		eHapticForceType GetType() {return m_Type;}

		virtual void SetActive(bool a_bX)=0;
		virtual bool IsActive()=0;

		virtual void SetForce(const cVector3f &a_vForce)=0;
		virtual void SetRelativeForce(const cVector3f &a_vForce)=0;

		virtual void SetDirection(const cVector3f &a_vDir)=0;
		virtual void SetFreq(float a_fFreq)=0;
		virtual void SetAmp(float a_fAmp)=0;

		virtual void SetTimeControl(bool a_bLoop, float a_fTime, float a_fIdleTime,
			float a_fFadeInTime, float a_fFadeOutTime)=0;


	protected:
		eHapticForceType m_Type;
	};
};

#endif