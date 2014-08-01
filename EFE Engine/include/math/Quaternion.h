#ifndef EFE_QUATERNION_H
#define EFE_QUATERNION_H

#include "math/Vector3.h"
#include "math/Matrix.h"

namespace efe
{
	class cQuaternion
	{
	public:
		cVector3<float> v;
		float w;

		cQuaternion();
		cQuaternion(float a_fAngle, const cVector3<float> &a_vAxis);
		cQuaternion(float a_fW, float a_fX, float a_fY, float a_fZ);

		void Normalize();
		void ToRotationMatrix(cMatrix<float> &a_mtxDest) const;
		void FromRotationMatrix(cMatrix<float> &a_mtxRot);

		void FromAngleAxis(float a_fAngle, const cVector3<float> &a_vAxis);
		void FromEulerAngles(const cVector3<float> &a_vAngles);

		cQuaternion operator+(const cQuaternion &a_qB) const;
		cQuaternion operator-(const cQuaternion &a_qB) const;
		cQuaternion operator*(const cQuaternion &a_qB) const;
		cQuaternion operator*(float a_fScalar) const;

		static const cQuaternion Identity;
	};
};
#endif