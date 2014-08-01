#ifndef EFE_CAMERA_H
#define EFE_CAMERA_H

#include "math/MathTypes.h"

namespace efe
{
	class iLowLevelGraphics;

	enum eCameraType
	{
		eCameraType_2D,
		eCameraType_3D,
		eCameraType_LastEnum
	};

	class iCamera
	{
	public:
		virtual void SetModelViewMatrix(iLowLevelGraphics *a_pLowLEvel)=0;
		virtual void SetProjectionMatrix(iLowLevelGraphics *a_pLowLEvel)=0;
		virtual cVector3f GetEyePosition()=0;

		virtual eCameraType GetType()=0;
	};
};
#endif