#ifndef EFE_HAPTIC_SURFACE_H
#define EFE_HAPTIC_SURFACE_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "haptic/HapticTypes.h"

namespace efe
{
	class iHapticSurface
	{
	public:
		iHapticSurface(const tString &a_sName, eHapticSurfaceType a_Type) : m_Type(a_Type) {}
		virtual ~iHapticSurface() {}

		eHapticSurfaceType GetType() {return m_Type;}
		tString &GetName() {return m_sName;}

	private:
		eHapticSurfaceType m_Type;
		tString m_sName;
	};
};

#endif