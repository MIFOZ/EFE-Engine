#include "math/Quaternion.h"

#include "math/Math.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	cQuaternion::cQuaternion()
	{
	}

	cQuaternion::cQuaternion(float a_fAngle, const cVector3<float> &a_vAxis)
	{
		FromAngleAxis(a_fAngle, a_vAxis);
	}

	cQuaternion::cQuaternion(float a_fW, float a_fX, float a_fY, float a_fZ)
	{
		w = a_fW;
		v.x = a_fX;
		v.y = a_fY;
		v.z = a_fZ;
	}

	const cQuaternion cQuaternion::Identity = cQuaternion(1.0f, 0.0f, 0.0f, 0.0f);

	void cQuaternion::Normalize()
	{
		float fLen = w*w + v.x*v.x + v.y*v.y + v.z*v.z;
		float fFactor = 1.0f / sqrt(fLen);
		v = v * fFactor;
		w = w * fFactor;
	}

	void cQuaternion::ToRotationMatrix(cMatrix<float> &a_mtxDest) const
	{
		float fTx = 2.0f*v.x;
		float fTy = 2.0f*v.y;
		float fTz = 2.0f*v.z;
		float fTwx = fTx*w;
		float fTwy = fTy*w;
		float fTwz = fTz*w;
		float fTxx = fTx*v.x;
		float fTxy = fTy*v.x;
		float fTxz = fTz*v.x;
		float fTyy = fTy*v.y;
		float fTyz = fTz*v.y;
		float fTzz = fTz*v.z;

		a_mtxDest.m[0][0] = 1.0f-(fTyy+fTzz);
		a_mtxDest.m[0][1] = fTxy-fTwz;
		a_mtxDest.m[0][2] = fTxz+fTwy;
		a_mtxDest.m[1][0] = fTxy+fTwz;
		a_mtxDest.m[1][1] = 1.0f-(fTxx+fTzz);
		a_mtxDest.m[1][2] = fTyz-fTwx;
		a_mtxDest.m[2][0] = fTxz-fTwy;
		a_mtxDest.m[2][1] = fTyz+fTwx;
		a_mtxDest.m[2][2] = 1.0f-(fTxx+fTyy);
	}

	void cQuaternion::FromRotationMatrix(cMatrix<float> &a_mtxRot)
	{
		float fTrace = a_mtxRot.m[0][0]+a_mtxRot.m[1][1]+a_mtxRot.m[2][2];
		float fRoot;

		if (fTrace > 0.0)
		{
			fRoot = sqrt(fTrace + 1.0f);
			w = 0.5f*fRoot;
			fRoot = 0.5f/fRoot;
			v.x = (a_mtxRot.m[2][1]-a_mtxRot.m[1][2])*fRoot;
			v.y = (a_mtxRot.m[0][2]-a_mtxRot.m[2][0])*fRoot;
			v.z = (a_mtxRot.m[1][0]-a_mtxRot.m[0][1])*fRoot;
		}
		else
		{
			static size_t s_iNext[3] = {1, 2, 0};
			size_t i = 0;
			if (a_mtxRot.m[1][1] > a_mtxRot.m[0][0])
				i = 1;
			if (a_mtxRot.m[2][2] > a_mtxRot.m[i][i])
				i = 2;
			size_t j = s_iNext[i];
			size_t k = s_iNext[j];

			fRoot = sqrt(a_mtxRot.m[i][i]-a_mtxRot.m[j][j]-a_mtxRot.m[k][k] + 1.0f);
			float *apkQuat[3] = {&v.x, &v.y, &v.z};
			*apkQuat[i] = 0.5f*fRoot;
			fRoot = 0.5f/fRoot;
			w = (a_mtxRot.m[k][j]-a_mtxRot.m[j][k])*fRoot;
			*apkQuat[j] = (a_mtxRot.m[j][i]+a_mtxRot.m[i][j])*fRoot;
			*apkQuat[k] = (a_mtxRot.m[k][i]+a_mtxRot.m[i][k])*fRoot;
		}
	}

	void cQuaternion::FromAngleAxis(float a_fAngle, const cVector3<float> &a_vAxis)
	{
		float fHalfAngle = 0.5f*a_fAngle;
		float fSin = sin(fHalfAngle);
		w = cos(fHalfAngle);
		v.x = fSin*a_vAxis.x;
		v.y = fSin*a_vAxis.y;
		v.z = fSin*a_vAxis.z;
	}

	void cQuaternion::FromEulerAngles(const cVector3<float> &a_vAngles)
	{
		float fHalfYaw = a_vAngles.x/2;
		float fHalfPitch = a_vAngles.y/2;
		float fHalfRoll = a_vAngles.z/2;

		float fSinYaw = sin(fHalfYaw);
		float fCosYaw = cos(fHalfYaw);

		float fSinPitch = sin(fHalfPitch);
		float fCosPitch = cos(fHalfPitch);

		float fSinRoll = sin(fHalfRoll);
		float fCosRoll = cos(fHalfRoll);

		v.x = fCosYaw * fSinPitch * fCosRoll + fSinYaw * fCosPitch * fSinRoll;
		v.y = fSinYaw * fCosPitch * fCosRoll - fCosYaw * fSinPitch * fSinRoll;
		v.z = fCosYaw * fCosPitch * fSinRoll - fSinYaw * fSinPitch * fCosRoll;
		w = fCosYaw * fCosPitch * fCosRoll + fSinYaw * fSinPitch * fSinRoll;
	}

	cQuaternion cQuaternion::operator+(const cQuaternion &a_qB) const
	{
		cQuaternion qOut;
		qOut.v = v + a_qB.v;
		qOut.w = w + a_qB.w;

		return qOut;
	}

	cQuaternion cQuaternion::operator-(const cQuaternion &a_qB) const
	{
		cQuaternion qOut;
		qOut.v = v - a_qB.v;
		qOut.w = w - a_qB.w;

		return qOut;
	}

	cQuaternion cQuaternion::operator*(const cQuaternion &a_qB) const
	{
		cQuaternion qOut;

		qOut.w = w * a_qB.w - v.x * a_qB.v.x - v.y * a_qB.v.y - v.z * a_qB.v.z;
		qOut.v.x = w * a_qB.v.x + v.x * a_qB.w + v.y * a_qB.v.z - v.z * a_qB.v.y;
		qOut.v.y = w * a_qB.v.y + v.y * a_qB.w - v.z * a_qB.v.x - v.x * a_qB.v.z;
		qOut.v.z = w * a_qB.v.z + v.z * a_qB.w - v.x * a_qB.v.y - v.y * a_qB.v.x;

		return qOut;
	}

	cQuaternion cQuaternion::operator*(float a_fScalar) const
	{
		cQuaternion qOut;
		qOut.v = v * a_fScalar;
		qOut.w = w * a_fScalar;

		return qOut;
	}
}