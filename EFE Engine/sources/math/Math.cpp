#include <stdlib.h>

#include "system/LowLevelSystem.h"
#include "math/Math.h"
#include <math.h>
#include <time.h>
#include <map>

namespace efe
{
	static char m_pTempChar[1024];

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	int cMath::RandRectl(int a_lMin, int a_lMax)
	{
		return (rand()%(a_lMax-a_lMin+1))+a_lMin;
	}

	//--------------------------------------------------------------

	float cMath::RandRectf(float a_fMin, float a_fMax)
	{
		float fRand = (float)rand()/(float)RAND_MAX;

		return a_fMin + fRand*(a_fMax-a_fMin);
	}

	//--------------------------------------------------------------

	bool cMath::BoxCollision(cRectf a_Rect1, cRectf a_Rect2)
	{
		return (a_Rect1.x>a_Rect2.x+(a_Rect2.w) || a_Rect2.x>a_Rect1.x+(a_Rect1.w) ||
			a_Rect1.y>a_Rect2.y+(a_Rect2.h) || a_Rect2.y>a_Rect1.y+(a_Rect1.h))==false;
	}

	bool cMath::PointBoxCollision(cVector2f a_vPoint, cRectf a_Rect)
	{
		if (a_vPoint.x<a_Rect.x || a_vPoint.x>a_Rect.x+a_Rect.w || a_vPoint.y<a_Rect.y || a_vPoint.y>a_Rect.y+a_Rect.h)
			return false;
		else
			return true;
	}

	//--------------------------------------------------------------

	bool cMath::BoxFit(cRectl a_RectSrc, cRectl a_RectDest)
	{
		if (a_RectSrc.w>a_RectDest.w || a_RectSrc.h>a_RectDest.h ||
			a_RectSrc.x+a_RectSrc.w > a_RectDest.x+a_RectDest.w ||
			a_RectSrc.y+a_RectSrc.h > a_RectDest.y+a_RectDest.h) return false;

		if (a_RectSrc.x<a_RectDest.x || a_RectSrc.y<a_RectDest.y ||
			a_RectSrc.x>a_RectDest.x+a_RectDest.w || a_RectSrc.y>a_RectDest.y+a_RectDest.h) return false;

		return true;
	}

	//--------------------------------------------------------------

	bool cMath::BoxFit(cRectf a_RectSrc, cRectf a_RectDest)
	{
		if (a_RectSrc.w>a_RectDest.w || a_RectSrc.h>a_RectDest.h ||
			a_RectSrc.x+a_RectSrc.w > a_RectDest.x+a_RectDest.w ||
			a_RectSrc.y+a_RectSrc.h > a_RectDest.y+a_RectDest.h) return false;

		if (a_RectSrc.x<a_RectDest.x || a_RectSrc.y<a_RectDest.y ||
			a_RectSrc.x>a_RectDest.x+a_RectDest.w || a_RectSrc.y>a_RectDest.y+a_RectDest.h) return false;

		return true;
	}

	//--------------------------------------------------------------

	cRectf &cMath::ClipRect(cRectf &a_RectSrc, const cRectf &a_RectDest)
	{
		if (a_RectSrc.x < a_RectDest.x)
		{
			a_RectSrc.w -= a_RectDest.x-a_RectSrc.x;
			a_RectSrc.x = a_RectDest.x;
		}
		if (a_RectSrc.y < a_RectDest.y)
		{
			a_RectSrc.h -= a_RectDest.y-a_RectSrc.y;
			a_RectSrc.y = a_RectDest.y;
		}

		if (a_RectSrc.x+a_RectSrc.w > a_RectDest.x+a_RectDest.w)
			a_RectSrc.w -= (a_RectSrc.x+a_RectSrc.w)-(a_RectDest.x+a_RectDest.w);

		if (a_RectSrc.y+a_RectSrc.h > a_RectDest.y+a_RectDest.h)
			a_RectSrc.h -= (a_RectSrc.y+a_RectSrc.h)-(a_RectDest.y+a_RectDest.h);

		return a_RectSrc;
	}

	//--------------------------------------------------------------

	bool cMath::CheckCollisionBV(cBoundingVolume &a_BV1, cBoundingVolume &a_BV2)
	{
		a_BV1.UpdateSize();
		a_BV2.UpdateSize();

		const cVector3f &vMin1 = a_BV1.m_vWorldMin;
		const cVector3f &vMin2 = a_BV2.m_vWorldMin;

		const cVector3f &vMax1 = a_BV1.m_vWorldMax;
		const cVector3f &vMax2 = a_BV2.m_vWorldMax;

		if (vMax1.x < vMin2.x || vMax1.y < vMin2.y || vMax1.z < vMin2.z ||
			vMax2.x < vMin1.x || vMax2.y < vMin1.y || vMax2.z < vMin1.z)
			return false;
		return true;
	}

	bool cMath::PointBVCollision(const cVector3f &a_vPoint, cBoundingVolume &a_BV)
	{
		cVector3f vMax = a_BV.GetMax();
		cVector3f vMin = a_BV.GetMin();

		if (a_vPoint.x > vMax.x || a_vPoint.y > vMax.y || a_vPoint.z > vMax.z ||
			a_vPoint.x < vMin.x || a_vPoint.y < vMin.y || a_vPoint.z < vMin.z)
			return false;
		return true;
	}

	bool cMath::GetClipRectFromBV(cRectl &a_DestRect, cBoundingVolume &a_BV,
							const cMatrixf &a_mtxView, const cMatrixf &a_mtxProj,
							float a_fNearClipPlane, const cVector2l &a_vScreenSize)
	{
		cVector3f vMax = a_BV.GetMax();
		cVector3f vMin = a_BV.GetMin();
		cVector3f vCorners[8];
		vCorners[0] = cVector3f(vMax.x,vMax.y,vMax.z);
		vCorners[1] = cVector3f(vMax.x,vMax.y,vMin.z);
		vCorners[2] = cVector3f(vMax.x,vMin.y,vMax.z);
		vCorners[3] = cVector3f(vMax.x,vMin.y,vMin.z);

		vCorners[4] = cVector3f(vMin.x,vMax.y,vMax.z);
		vCorners[5] = cVector3f(vMin.x,vMax.y,vMin.z);
		vCorners[6] = cVector3f(vMin.x,vMin.y,vMax.z);
		vCorners[7] = cVector3f(vMin.x,vMin.y,vMin.z);

		bool bVisible = true;

		float fMaxZ = -a_fNearClipPlane;
		for (int i=0;i<8;i++)
		{
			vCorners[i] = MatrixMul(a_mtxView, vCorners[i]);

			if (vCorners[i].z > fMaxZ)
			{
				vCorners[i].z = 0;
				bVisible = false;
			}

			vCorners[i] = MatrixMulDivideW(a_mtxProj, vCorners[i]);
		}

		if (bVisible == false) return false;

		vMax = vCorners[0];
		vMin = vCorners[0];
		for (int i=0;i<8;i++)
		{
			if (vCorners[i].x < vMin.x) vMin.x = vCorners[i].x;
			if (vCorners[i].x > vMax.x) vMax.x = vCorners[i].x;

			if (vCorners[i].y < vMin.y) vMin.y = vCorners[i].y;
			if (vCorners[i].y > vMax.y) vMax.y = vCorners[i].y;
		}

		if (vMin.x < -1) vMin.x = -1;
		if (vMin.y < -1) vMin.y = -1;
		if (vMin.x > 1) vMin.x = 1;
		if (vMin.y > 1) vMin.y = 1;

		cVector2f vHalScreenSize = cVector2f((float)a_vScreenSize.x, (float)a_vScreenSize.y) * 0.5f;
		cVector2l vTopLeft;
		cVector2l vBottomRight;
		vTopLeft.x = (int)(vHalScreenSize.x + vMin.x * vHalScreenSize.x);
		vTopLeft.y = (int)(vHalScreenSize.y + vMax.y * -vHalScreenSize.y);
		vBottomRight.x = (int)(vHalScreenSize.x + vMin.x * vHalScreenSize.x);
		vBottomRight.y = (int)(vHalScreenSize.y + vMax.y * -vHalScreenSize.y);

		if (vTopLeft.x < 0) vTopLeft.x = 0;
		if (vTopLeft.y < 0) vTopLeft.y = 0;
		if (vBottomRight.x > a_vScreenSize.x-1) vBottomRight.x = a_vScreenSize.x-1;
		if (vBottomRight.y > a_vScreenSize.y-1) vBottomRight.y = a_vScreenSize.y-1;

		a_DestRect.x = vTopLeft.x;
		a_DestRect.y = vTopLeft.y;
		a_DestRect.w = (vBottomRight.x - vTopLeft.x)+1;
		a_DestRect.h = (vBottomRight.y - vTopLeft.y)+1;

		return true;
	}

	//--------------------------------------------------------------

	float cMath::Modulus(float a_fDivident, float a_fDivisor)
	{
		float fNum = std::floor(std::abs(a_fDivident/a_fDivisor));

		float fRemain = std::abs(a_fDivident) - std::abs(a_fDivisor)*fNum;

		return fRemain;
	}

	//--------------------------------------------------------------

	float cMath::ToRad(float a_fAngle)
	{
		return (a_fAngle/360.0f)*k2Pif;
	}

	//--------------------------------------------------------------

	float cMath::ToDeg(float a_fAngle)
	{
		return (a_fAngle/k2Pif)*360.0f;
	}

	//--------------------------------------------------------------

	int cMath::Log2ToInt(int a_lX)
	{
		switch(a_lX)
		{
		case 1: return 0;
		case 2: return 1;
		case 4: return 2;
		case 8: return 3;
		case 16: return 4;
		case 32: return 5;
		case 64: return 6;
		case 128: return 7;
		case 256: return 8;
		case 512: return 9;
		default: return (int)floor((log((float)a_lX) / log(2.0f)) + 0.5f);
		}
	}

	//--------------------------------------------------------------

	bool cMath::IsPow2(int a_lX)
	{
		switch(a_lX)
		{
		case 0:			return true;
		case 1:			return true;
		case 2:			return true;
		case 4:			return true;
		case 8:			return true;
		case 16:		return true;
		case 32:		return true;
		case 64:		return true;
		case 128:		return true;
		case 256:		return true;
		case 512:		return true;
		case 1024:		return true;
		case 2048:		return true;
		case 4096:		return true;
		case 8192:		return true;
		default:		return false;
		}
	}

	//--------------------------------------------------------------

	float cMath::Wrap(float a_fX, float a_fMin, float a_fMax)
	{
		if (a_fX >= a_fMin && a_fX <= a_fMax) return a_fX;

		a_fMax = a_fMax - a_fMin;
		float fOffset = a_fMin;
		a_fMin = 0;
		a_fX = a_fX - fOffset;

		float fNumOfMax = std::floor(std::abs(a_fX/a_fMax));

		if (a_fX>=a_fMax) a_fX = a_fX - fNumOfMax * a_fMax;
		else if (a_fX<a_fMin) a_fX = ((fNumOfMax + 1.0f) * a_fMax) + a_fX;

		return a_fX + fOffset;
	}

	//--------------------------------------------------------------

	float cMath::GetAngleFromPoints2D(const cVector2f &a_vStartPos, const cVector2f &a_vGoalPos)
	{
		float fDx;
		float fDy;
		float fAns;

		fDx = a_vGoalPos.x - a_vStartPos.x;
		fDy = a_vGoalPos.y - a_vStartPos.y;
		if (fDx==0) fDx = 0.00001f;
		if (fDy==0) fDy = 0.00001f;

		if (fDx>=0 && fDy<0)
			fAns = atan(fDx/(-fDy));
		else if (fDx>=0 && fDy>=0)
			fAns = atan(fDy/fDx) + kPi2f;
		else if (fDx<0 && fDy>=0)
			fAns = atan((-fDx)/fDy) + kPi2f;
		else if (fDx<0 && fDy<0)
			fAns = atan((-fDy)/(-fDx)) + kPi2f + kPi2f;

		return fAns;
	}

	cVector3f cMath::Vector3Cross(const cVector3f &a_vVecA, const cVector3f &a_vVecB)
	{
		cVector3f vResult;

		vResult.x = a_vVecA.y*a_vVecB.z - a_vVecA.z*a_vVecB.y;
		vResult.y = a_vVecA.z*a_vVecB.x - a_vVecA.x*a_vVecB.z;
		vResult.z = a_vVecA.x*a_vVecB.y - a_vVecA.y*a_vVecB.x;

		return vResult;
	}

	float cMath::Vector3Dot(const cVector3f &a_vVecA, const cVector3f &a_vVecB)
	{
		return a_vVecA.x*a_vVecB.x + a_vVecA.y*a_vVecB.y + a_vVecA.z*a_vVecB.z;
	}

	//--------------------------------------------------------------

	float cMath::Vector3Angle(const cVector3f &a_vVecA, const cVector3f &a_vVecB)
	{
		float fCos = Vector3Dot(a_vVecA, a_vVecB);

		if (std::abs(fCos - 1) <= kEpsilonf) return 0;

		return acos(fCos);
	}

	//--------------------------------------------------------------

	static inline cVector3f GetVector3(const float *a_pVertexArray, int a_lIdx, int a_lStride)
	{
		const float *a_pVec = &a_pVertexArray[a_lIdx * a_lStride];
		return cVector3f(a_pVec[0], a_pVec[1], a_pVec[2]);
	}

	static inline void SetVector4(const cVector3f &a_vVec, float a_fW, float *a_pArray, int a_lIdx)
	{
		float *pVec = &a_pArray[a_lIdx*4];
		pVec[0] = a_vVec.x;
		pVec[1] = a_vVec.y;
		pVec[2] = a_vVec.z;
		pVec[3] = a_fW;
	}

	static inline bool Vector3Equal(const float *a_pArrayA, int a_lIdxA, const float *a_pArrayB, int a_lIdxB,
									int a_lStride)
	{
		if (a_pArrayA[a_lIdxA*a_lStride] == a_pArrayB[a_lIdxB*a_lStride] &&
			a_pArrayA[a_lIdxA*a_lStride+1] == a_pArrayB[a_lIdxB*a_lStride+1] &&
			a_pArrayA[a_lIdxA*a_lStride+2] == a_pArrayB[a_lIdxB*a_lStride+2])
			return true;
		else return false;
	}

	float cMath::PlaneToPointDist(const cPlanef &a_Plane, const cVector3f &a_vVec)
	{
		return (a_Plane.a * a_vVec.x) + (a_Plane.b * a_vVec.y) + (a_Plane.c * a_vVec.z) + a_Plane.d;
	}

	//--------------------------------------------------------------

	cQuaternion cMath::QuaternionSlerp(float a_fT, const cQuaternion &a_qA, const cQuaternion &a_qB,
			bool a_bShortestPath)
	{
		float fCos = QuaternionDot(a_qA, a_qB);

		if (std::abs(fCos - 1) <= kEpsilonf)
			return a_qA;

		float fAngle = acos(fCos);

		float fSin = sin(fAngle);
		float fInvSin = 1.0f/fSin;
		float fCoeff0 = sin((1.0f-a_fT)*fAngle)*fInvSin;
		float fCoeff1 = sin(a_fT*fAngle)*fInvSin;

		if (fCos < 0.0f && a_bShortestPath)
		{
			fCoeff0 = -fCoeff0;
			cQuaternion qT(a_qA * fCoeff0 + a_qB * fCoeff1);
			qT.Normalize();
			return qT;
		}
		else
			return a_qA * fCoeff0 + a_qB * fCoeff1;
	}

	//--------------------------------------------------------------

	float cMath::QuaternionDot(const cQuaternion &a_qA, const cQuaternion &a_qB)
	{
		return a_qA.w*a_qB.w + a_qA.v.x*a_qB.v.x + a_qA.v.y*a_qB.v.y + a_qA.v.z*a_qB.v.z;
	}

	//--------------------------------------------------------------

	cMatrixf cMath::MatrixMul(const cMatrixf &a_mtxA,const cMatrixf &a_mtxB)
	{
		cMatrixf mtxC;

		mtxC.m[0][0] = a_mtxA.m[0][0] * a_mtxB.m[0][0] + a_mtxA.m[0][1] * a_mtxB.m[1][0] + a_mtxA.m[0][2] * a_mtxB.m[2][0] + a_mtxA.m[0][3] * a_mtxB.m[3][0];
		mtxC.m[0][1] = a_mtxA.m[0][0] * a_mtxB.m[0][1] + a_mtxA.m[0][1] * a_mtxB.m[1][1] + a_mtxA.m[0][2] * a_mtxB.m[2][1] + a_mtxA.m[0][3] * a_mtxB.m[3][1];
		mtxC.m[0][2] = a_mtxA.m[0][0] * a_mtxB.m[0][2] + a_mtxA.m[0][1] * a_mtxB.m[1][2] + a_mtxA.m[0][2] * a_mtxB.m[2][2] + a_mtxA.m[0][3] * a_mtxB.m[3][2];
		mtxC.m[0][3] = a_mtxA.m[0][0] * a_mtxB.m[0][3] + a_mtxA.m[0][1] * a_mtxB.m[1][3] + a_mtxA.m[0][2] * a_mtxB.m[2][3] + a_mtxA.m[0][3] * a_mtxB.m[3][3];

		mtxC.m[1][0] = a_mtxA.m[1][0] * a_mtxB.m[0][0] + a_mtxA.m[1][1] * a_mtxB.m[1][0] + a_mtxA.m[1][2] * a_mtxB.m[2][0] + a_mtxA.m[1][3] * a_mtxB.m[3][0];
		mtxC.m[1][1] = a_mtxA.m[1][0] * a_mtxB.m[0][1] + a_mtxA.m[1][1] * a_mtxB.m[1][1] + a_mtxA.m[1][2] * a_mtxB.m[2][1] + a_mtxA.m[1][3] * a_mtxB.m[3][1];
		mtxC.m[1][2] = a_mtxA.m[1][0] * a_mtxB.m[0][2] + a_mtxA.m[1][1] * a_mtxB.m[1][2] + a_mtxA.m[1][2] * a_mtxB.m[2][2] + a_mtxA.m[1][3] * a_mtxB.m[3][2];
		mtxC.m[1][3] = a_mtxA.m[1][0] * a_mtxB.m[0][3] + a_mtxA.m[1][1] * a_mtxB.m[1][3] + a_mtxA.m[1][2] * a_mtxB.m[2][3] + a_mtxA.m[1][3] * a_mtxB.m[3][3];

		mtxC.m[2][0] = a_mtxA.m[2][0] * a_mtxB.m[0][0] + a_mtxA.m[2][1] * a_mtxB.m[1][0] + a_mtxA.m[2][2] * a_mtxB.m[2][0] + a_mtxA.m[2][3] * a_mtxB.m[3][0];
		mtxC.m[2][1] = a_mtxA.m[2][0] * a_mtxB.m[0][1] + a_mtxA.m[2][1] * a_mtxB.m[1][1] + a_mtxA.m[2][2] * a_mtxB.m[2][1] + a_mtxA.m[2][3] * a_mtxB.m[3][1];
		mtxC.m[2][2] = a_mtxA.m[2][0] * a_mtxB.m[0][2] + a_mtxA.m[2][1] * a_mtxB.m[1][2] + a_mtxA.m[2][2] * a_mtxB.m[2][2] + a_mtxA.m[2][3] * a_mtxB.m[3][2];
		mtxC.m[2][3] = a_mtxA.m[2][0] * a_mtxB.m[0][3] + a_mtxA.m[2][1] * a_mtxB.m[1][3] + a_mtxA.m[2][2] * a_mtxB.m[2][3] + a_mtxA.m[2][3] * a_mtxB.m[3][3];

		mtxC.m[3][0] = a_mtxA.m[3][0] * a_mtxB.m[0][0] + a_mtxA.m[3][1] * a_mtxB.m[1][0] + a_mtxA.m[3][2] * a_mtxB.m[2][0] + a_mtxA.m[3][3] * a_mtxB.m[3][0];
		mtxC.m[3][1] = a_mtxA.m[3][0] * a_mtxB.m[0][1] + a_mtxA.m[3][1] * a_mtxB.m[1][1] + a_mtxA.m[3][2] * a_mtxB.m[2][1] + a_mtxA.m[3][3] * a_mtxB.m[3][1];
		mtxC.m[3][2] = a_mtxA.m[3][0] * a_mtxB.m[0][2] + a_mtxA.m[3][1] * a_mtxB.m[1][2] + a_mtxA.m[3][2] * a_mtxB.m[2][2] + a_mtxA.m[3][3] * a_mtxB.m[3][2];
		mtxC.m[3][3] = a_mtxA.m[3][0] * a_mtxB.m[0][3] + a_mtxA.m[3][1] * a_mtxB.m[1][3] + a_mtxA.m[3][2] * a_mtxB.m[2][3] + a_mtxA.m[3][3] * a_mtxB.m[3][3];

		return mtxC;
	}

	//--------------------------------------------------------------

	cVector3f cMath::MatrixMul(const cMatrixf &a_mtxA,const cVector3f &a_vB)
	{
		cVector3f vC;

		vC.x = a_mtxA.m[0][0] * a_vB.x + a_mtxA.m[0][1] * a_vB.y + a_mtxA.m[0][2] * a_vB.z + a_mtxA.m[0][3];
		vC.y = a_mtxA.m[1][0] * a_vB.x + a_mtxA.m[1][1] * a_vB.y + a_mtxA.m[1][2] * a_vB.z + a_mtxA.m[1][3];
		vC.z = a_mtxA.m[2][0] * a_vB.x + a_mtxA.m[2][1] * a_vB.y + a_mtxA.m[2][2] * a_vB.z + a_mtxA.m[2][3];

		return vC;
	}

	//--------------------------------------------------------------

	cVector3f cMath::MatrixMulDivideW(const cMatrixf &a_mtxA,const cVector3f &a_vB)
	{
		cVector3f vC;

		float fInvW = 1.0f / (a_mtxA.m[3][0] * a_vB.x + a_mtxA.m[3][1] * a_vB.y + a_mtxA.m[3][2] * a_vB.z + a_mtxA.m[3][3]);

		vC.x = a_mtxA.m[0][0] * a_vB.x + a_mtxA.m[0][1] * a_vB.y + a_mtxA.m[0][2] * a_vB.z + a_mtxA.m[0][3] * fInvW;
		vC.y = a_mtxA.m[1][0] * a_vB.x + a_mtxA.m[1][1] * a_vB.y + a_mtxA.m[1][2] * a_vB.z + a_mtxA.m[1][3] * fInvW;
		vC.z = a_mtxA.m[2][0] * a_vB.x + a_mtxA.m[2][1] * a_vB.y + a_mtxA.m[2][2] * a_vB.z + a_mtxA.m[2][3] * fInvW;

		return vC;
	}

	//--------------------------------------------------------------

	cMatrixf cMath::MatrixMulScalar(const cMatrixf &a_mtxA, float a_fB)
	{
		cMatrixf mtxC;

		mtxC.m[0][0] = a_mtxA.m[0][0]*a_fB;mtxC.m[0][1] = a_mtxA.m[0][1]*a_fB;mtxC.m[0][2] = a_mtxA.m[0][2]*a_fB;mtxC.m[0][3] = a_mtxA.m[0][3]*a_fB;
		mtxC.m[1][0] = a_mtxA.m[1][0]*a_fB;mtxC.m[1][1] = a_mtxA.m[1][1]*a_fB;mtxC.m[1][2] = a_mtxA.m[1][2]*a_fB;mtxC.m[1][3] = a_mtxA.m[1][3]*a_fB;
		mtxC.m[2][0] = a_mtxA.m[2][0]*a_fB;mtxC.m[2][1] = a_mtxA.m[2][1]*a_fB;mtxC.m[2][2] = a_mtxA.m[2][2]*a_fB;mtxC.m[2][3] = a_mtxA.m[2][3]*a_fB;
		mtxC.m[3][0] = a_mtxA.m[3][0]*a_fB;mtxC.m[3][1] = a_mtxA.m[3][1]*a_fB;mtxC.m[3][2] = a_mtxA.m[3][2]*a_fB;mtxC.m[3][3] = a_mtxA.m[3][3]*a_fB;

		return mtxC;
	}

	//--------------------------------------------------------------

	cMatrixf cMath::MatrixRotate(cVector3f a_vRot, eEulerRotationOrder a_Order)
	{
		cMatrixf mtxRot = cMatrixf::Identity;
		switch(a_Order)
		{
		case eEulerRotationOrder_XYZ:	mtxRot = MatrixMul(MatrixRotateX(a_vRot.x),mtxRot);
										mtxRot = MatrixMul(MatrixRotateY(a_vRot.y),mtxRot);
										mtxRot = MatrixMul(MatrixRotateZ(a_vRot.z),mtxRot);
										break;
		}

		return mtxRot;
	}

	//--------------------------------------------------------------

	cMatrixf cMath::MatrixRotateX(float a_fAngle)
	{
		return cMatrixf(1,0,0,0,
						0, cos(a_fAngle), -sin(a_fAngle),0,
						0, sin(a_fAngle), cos(a_fAngle),0,
						0,0,0,1);
	}

	//--------------------------------------------------------------

	cMatrixf cMath::MatrixRotateY(float a_fAngle)
	{
		return cMatrixf(cos(a_fAngle),0,sin(a_fAngle),0,
						0,1,0,0,
						-sin(a_fAngle),0, cos(a_fAngle),0,
						0,0,0,1);
	}

	//--------------------------------------------------------------

	cMatrixf cMath::MatrixRotateZ(float a_fAngle)
	{
		return cMatrixf(cos(a_fAngle),-sin(a_fAngle),0,0,
						sin(a_fAngle), cos(a_fAngle), 0,0,
						0,0,1,0,
						0,0,0,1);
	}

	//--------------------------------------------------------------

	cMatrixf cMath::MatrixPerspectiveY(float a_fAspect, int m_lWidth, int m_lHeight, float a_fNearPlane, float a_fFarPlane)
	{
		float h = cos(0.5f*a_fAspect) / sin(0.5f*a_fAspect);
		float w = (h * m_lHeight) / m_lWidth;

		return cMatrixf(w,0,0,0,
			0,h,0,0,
			0,0,(a_fFarPlane+a_fNearPlane)/(a_fFarPlane-a_fNearPlane), -(2*a_fFarPlane*a_fNearPlane)/(a_fFarPlane-a_fNearPlane),
			0,0,1,0);
	}

	//--------------------------------------------------------------

	cMatrixf cMath::ToD3DPerspective(const cMatrixf &a_mtxA)
	{
		cMatrixf mtxRes = a_mtxA;
		mtxRes.m[2][0] = 0.5f * (mtxRes.m[2][0] + mtxRes.m[3][0]);
		mtxRes.m[2][1] = 0.5f * (mtxRes.m[2][1] + mtxRes.m[3][1]);
		mtxRes.m[2][2] = 0.5f * (mtxRes.m[2][2] + mtxRes.m[3][2]);
		mtxRes.m[2][3] = 0.5f * (mtxRes.m[2][3] + mtxRes.m[3][3]);

		return mtxRes;
	}

	//--------------------------------------------------------------

	cMatrixf cMath::MatrixScale(cVector3f a_vScale)
	{
		return cMatrixf(a_vScale.x, 0,0,0,
						0, a_vScale.y,0,0,
						0,0, a_vScale.z,0,
						0,0,0,1);
	}

	//--------------------------------------------------------------

	cMatrixf cMath::MatrixTranslate(cVector3f a_vTrans)
	{
		return cMatrixf(1, 0,0,a_vTrans.x,
						0, 1,0,a_vTrans.y,
						0,0, 1,a_vTrans.z,
						0,0,0,1);
	}

	//--------------------------------------------------------------

	cMatrixf cMath::MatrixQuaternion(const cQuaternion &a_qRot)
	{
		cMatrixf mtxOut;
		mtxOut.m[0][3] = 0;
		mtxOut.m[1][3] = 0;
		mtxOut.m[2][3] = 0;
		mtxOut.m[3][0] = 0;
		mtxOut.m[3][1] = 0;
		mtxOut.m[3][2] = 0;
		mtxOut.m[3][3] = 1;

		a_qRot.ToRotationMatrix(mtxOut);

		return mtxOut;
	}

	//--------------------------------------------------------------

	float cMath::MatrixMinor(const cMatrixf a_mtxA,
			const size_t r0, const size_t r1, const size_t r2,
			const size_t c0, const size_t c1, const size_t c2)
	{
		return a_mtxA.m[r0][c0] * (a_mtxA.m[r1][c1] * a_mtxA.m[r2][c2] - a_mtxA.m[r2][c1] * a_mtxA.m[r1][c2]) -
				a_mtxA.m[r0][c1] * (a_mtxA.m[r1][c0] * a_mtxA.m[r2][c2] - a_mtxA.m[r2][c0] * a_mtxA.m[r1][c2]) +
				a_mtxA.m[r0][c2] * (a_mtxA.m[r1][c0] * a_mtxA.m[r2][c1] - a_mtxA.m[r2][c0] * a_mtxA.m[r1][c1]);
	}

	//--------------------------------------------------------------

	cMatrixf cMath::MatrixAdjoint(const cMatrixf &a_mtxA)
	{
		return cMatrixf(MatrixMinor(a_mtxA, 1, 2, 3, 1, 2, 3),
						-MatrixMinor(a_mtxA, 0, 2, 3, 1, 2, 3),
						MatrixMinor(a_mtxA, 0, 1, 3, 1, 2, 3),
						-MatrixMinor(a_mtxA, 0, 1, 2, 1, 2, 3),

						-MatrixMinor(a_mtxA, 1, 2, 3, 0, 2, 3),
						MatrixMinor(a_mtxA, 0, 2, 3, 0, 2, 3),
						-MatrixMinor(a_mtxA, 0, 1, 3, 0, 2, 3),
						MatrixMinor(a_mtxA, 0, 1, 2, 0, 2, 3),

						MatrixMinor(a_mtxA, 1, 2, 3, 0, 1, 3),
						-MatrixMinor(a_mtxA, 0, 2, 3, 0, 1, 3),
						MatrixMinor(a_mtxA, 0, 1, 3, 0, 1, 3),
						-MatrixMinor(a_mtxA, 0, 1, 2, 0, 1, 3),

						-MatrixMinor(a_mtxA, 1, 2, 3, 0, 1, 2),
						MatrixMinor(a_mtxA, 0, 2, 3, 0, 1, 2),
						-MatrixMinor(a_mtxA, 0, 1, 3, 0, 1, 2),
						MatrixMinor(a_mtxA, 0, 1, 2, 0, 1, 2));
	}

	//--------------------------------------------------------------

	float cMath::MatrixDetermiant(const cMatrixf &a_mtxA)
	{
		return	a_mtxA.m[0][0] * MatrixMinor(a_mtxA, 1, 2, 3, 1, 2, 3) - 
				a_mtxA.m[0][1] * MatrixMinor(a_mtxA, 1, 2, 3, 0, 2, 3) + 
				a_mtxA.m[0][2] * MatrixMinor(a_mtxA, 1, 2, 3, 0, 1, 3) - 
				a_mtxA.m[0][3] * MatrixMinor(a_mtxA, 1, 2, 3, 0, 1, 2);
	}

	//--------------------------------------------------------------

	cMatrixf cMath::MatrixInverse(const cMatrixf &a_mtxA)
	{
		return MatrixMulScalar(MatrixAdjoint(a_mtxA), (1.0f / MatrixDetermiant(a_mtxA)));
	}

	//--------------------------------------------------------------

	const char *cMath::MatrixToChar(const cMatrixf &a_mtxA)
	{
		sprintf(m_pTempChar,"[%.3f, %.3f, %.3f, %.3f] [%.3f, %.3f, %.3f, %.3f] [%.3f, %.3f, %.3f, %.3f] [%.3f, %.3f, %.3f, %.3f]",
							a_mtxA.m[0][0],a_mtxA.m[0][1],a_mtxA.m[0][2],a_mtxA.m[0][3],
							a_mtxA.m[1][0],a_mtxA.m[1][1],a_mtxA.m[1][2],a_mtxA.m[1][3],
							a_mtxA.m[2][0],a_mtxA.m[2][1],a_mtxA.m[2][2],a_mtxA.m[2][3],
							a_mtxA.m[3][0],a_mtxA.m[3][1],a_mtxA.m[3][2],a_mtxA.m[3][3]);

		return m_pTempChar;
	}

	//--------------------------------------------------------------

	bool cMath::CreateTriTangentVectors(float *a_pDestArray,
										const unsigned int *a_pIndexArray, int a_lIndexNum,
										const float *a_pVertexArray, int a_lVtxStride,
										const float *a_pTexArray,
										const float *a_pNormalArray, int a_lVertexNum)
	{
		tVector3fVec vTempTangents1;
		tVector3fVec vTempTangents2;

		vTempTangents1.resize(a_lVertexNum, cVector3f(0,0,0));
		vTempTangents2.resize(a_lVertexNum, cVector3f(0,0,0));

		for (int triIdx = 0;triIdx<a_lIndexNum;triIdx+=3)
		{
			int idx1 = a_pIndexArray[triIdx];
			int idx2 = a_pIndexArray[triIdx+1];
			int idx3 = a_pIndexArray[triIdx+2];

			cVector3f vPos1 = GetVector3(a_pVertexArray, idx1, a_lVtxStride);
			cVector3f vPos2 = GetVector3(a_pVertexArray, idx2, a_lVtxStride);
			cVector3f vPos3 = GetVector3(a_pVertexArray, idx3, a_lVtxStride);

			cVector3f vTex1 = GetVector3(a_pTexArray,idx1,3);
			cVector3f vTex2 = GetVector3(a_pTexArray,idx2,3);
			cVector3f vTex3 = GetVector3(a_pTexArray,idx3,3);

			cVector3f vPos1To2 = vPos2 - vPos1;
			cVector3f vPos1To3 = vPos3 - vPos1;

			cVector3f vTex1To2 = vTex2 - vTex1;
			cVector3f vTex1To3 = vTex3 - vTex1;

			float fR = 1.0f / (vTex1To2.x * vTex1To3.y - vTex1To2.y * vTex1To3.x);

			cVector3f vSDir((vTex1To3.y * vPos1To2.x - vTex1To2.y * vPos1To3.x)*fR,
								(vTex1To3.y * vPos1To2.y - vTex1To2.y * vPos1To3.y)*fR,
								(vTex1To3.y * vPos1To2.z - vTex1To2.y * vPos1To3.z)*fR);
			
			cVector3f vTDir((vTex1To2.x * vPos1To3.x - vTex1To3.x * vPos1To2.x)*fR,
								(vTex1To2.x * vPos1To3.y - vTex1To3.x * vPos1To2.y)*fR,
								(vTex1To2.x * vPos1To3.z - vTex1To3.x * vPos1To2.z)*fR);

			vTempTangents1[idx1] += vSDir;
			vTempTangents1[idx2] += vSDir;
			vTempTangents1[idx3] += vSDir;

			vTempTangents2[idx1] += vTDir;
			vTempTangents2[idx2] += vTDir;
			vTempTangents2[idx3] += vTDir;
		}

		float fMaxCosAngle = -1.0f;
		for (int i=0; i < a_lVertexNum; i++)
		{
			for (int j=i+1; j<a_lVertexNum; j++)
			{
				if (Vector3Equal(a_pVertexArray, i, a_pVertexArray, j, a_lVtxStride) &&
					Vector3Equal(a_pNormalArray, i, a_pNormalArray, j, 3))
				{
					cVector3f vAT1 = vTempTangents1[i];
					cVector3f vAT2 = vTempTangents2[i];

					cVector3f vBT1 = vTempTangents1[j];
					cVector3f vBT2 = vTempTangents2[j];

					if (Vector3Dot(vAT1, vBT1)>=fMaxCosAngle)
					{
						vTempTangents1[j] += vAT1;
						vTempTangents1[i] += vBT1;
					}

					if (Vector3Dot(vAT2, vBT2)>=fMaxCosAngle)
					{
						vTempTangents2[j] += vAT2;
						vTempTangents2[i] += vBT2;
					}
				}
			}
		}

		for (int vtxIdx=0; vtxIdx < a_lVertexNum; vtxIdx++)
		{
			cVector3f vNormal = GetVector3(a_pNormalArray, vtxIdx, 3);
			cVector3f &vTempTan1 = vTempTangents1[vtxIdx];
			cVector3f &vTempTan2 = vTempTangents2[vtxIdx];

			cVector3f vTan = vTempTan1 - (vNormal * cMath::Vector3Dot(vNormal, vTempTan1));
			vTan.Normalize();

			float fW = (cMath::Vector3Dot(cMath::Vector3Cross(vNormal, vTempTan1), vTempTan2) < 0.0f) ? -1.0f : 1.0f;

			SetVector4(vTan, fW, a_pDestArray, vtxIdx);
		}

		return true;
	}

	//--------------------------------------------------------------

	bool cMath::CreateTriangleData(tTriangleDataVec &a_vTriangles,
										const unsigned int *a_pIndexArray, int a_lIndexNum,
										const float *a_pVertexArray, int a_lVtxStride, int a_lVertexNum)
	{
		int lNumOfTri = a_lIndexNum / 3;
		if ((int)a_vTriangles.size() < lNumOfTri) a_vTriangles.resize(lNumOfTri);

		for (int tri=0,idx=0; tri<lNumOfTri; tri++,idx+=3)
		{
			//Calculate normal
			const float *pVtx0 = &a_pVertexArray[a_pIndexArray[idx]*a_lVtxStride];
			const float *pVtx1 = &a_pVertexArray[a_pIndexArray[idx+1]*a_lVtxStride];
			const float *pVtx2 = &a_pVertexArray[a_pIndexArray[idx+2]*a_lVtxStride];

			cVector3f vEdge1(pVtx1[0] - pVtx0[0], pVtx1[1] - pVtx0[1], pVtx1[2] - pVtx0[2]);
			cVector3f vEdge2(pVtx2[0] - pVtx0[0], pVtx2[1] - pVtx0[1], pVtx2[2] - pVtx0[2]);

			a_vTriangles[tri].normal = Vector3Cross(vEdge2 ,vEdge1);
		}

		return true;
	}

	//--------------------------------------------------------------

	cVector3f cMath::RGBEToVector3f(unsigned char *pRGBE)
	{
		if (pRGBE[3])
			return cVector3f(pRGBE[0], pRGBE[1], pRGBE[2]) * ldexpf(1.0f, pRGBE[3] - (int)(128 + 8));
		else
			return cVector3f(0,0,0);
	}

	float cMath::Clamp(float a_fX, float a_fMin, float a_fMax)
	{
		if (a_fX > a_fMax) a_fX = a_fMax;
		if (a_fX < a_fMin) a_fX = a_fMin;

		return a_fX;
	}

	cVector3f cMath::Clamp(const cVector3f &a_vVec, const float a_fC0, const float a_fC1)
	{
		return cVector3f(Min(Max(a_vVec.x, a_fC0), a_fC1), Min(Max(a_vVec.y, a_fC0), a_fC1), Min(Max(a_vVec.z, a_fC0), a_fC1));
	}

	float cMath::Saturate(const float a_vV)
	{
		return Clamp(a_vV, 0, 1);
	}

	static const float *g_pVertexArray;
	static const unsigned int *g_pIndexArray;
	static int g_lVertexStride;

	class cVertexIndices
	{
	public:
		cVertexIndices(unsigned int a_lIdx)
		{
			m_lstIndices.push_back(a_lIdx);
		}
		tUIntList m_lstIndices;
	};

	typedef std::map<cVector3f, cVertexIndices> tVtxIdxMap;
	typedef tVtxIdxMap::iterator tVtxIdxMapIt;

	class cEdgeCompare
	{
	public:
		bool operator()(const cTriEdge &Edge1, const cTriEdge &Edge2)const
		{
			cVector3f vPoint1_1 = GetVector3(g_pVertexArray,Edge1.point1, g_lVertexStride);
			cVector3f vPoint1_2 = GetVector3(g_pVertexArray,Edge1.point2, g_lVertexStride);
			cVector3f vPoint2_1 = GetVector3(g_pVertexArray,Edge2.point1, g_lVertexStride);
			cVector3f vPoint2_2 = GetVector3(g_pVertexArray,Edge2.point2, g_lVertexStride);

			//1 - 1
			if (vPoint1_1.x != vPoint2_1.x) return vPoint1_1.x > vPoint2_1.x;
			if (vPoint1_1.y != vPoint2_1.y) return vPoint1_1.y > vPoint2_1.y;
			if (vPoint1_1.z != vPoint2_1.z) return vPoint1_1.z > vPoint2_1.z;

			//2 - 2
			if (vPoint1_2.x != vPoint2_2.x) return vPoint1_2.x > vPoint2_2.x;
			if (vPoint1_2.y != vPoint2_2.y) return vPoint1_2.y > vPoint2_2.y;
			if (vPoint1_2.z != vPoint2_2.z) return vPoint1_2.z > vPoint2_2.z;

			return false;
		}
	};

	typedef std::set<cTriEdge, cEdgeCompare> tTriEdgeListMap;
	typedef tTriEdgeListMap::iterator tTriEdgeListMapIt;

	static void CheckEdgeSwitch(cTriEdge *a_pEdge)
	{
		cVector3f vPoint1 = GetVector3(g_pVertexArray, a_pEdge->point1, g_lVertexStride);
		cVector3f vPoint2 = GetVector3(g_pVertexArray, a_pEdge->point2, g_lVertexStride);
		if (vPoint1 < vPoint2)
		{
			unsigned int lTemp = a_pEdge->point1;
			a_pEdge->point1 = a_pEdge->point2;
			a_pEdge->point2 = lTemp;
		}
	}

	void AddEdgeToMap(cTriEdge &a_Edge, tTriEdgeListMap &a_Map)
	{
		tTriEdgeListMapIt it = a_Map.find(a_Edge);

		if (it == a_Map.end())
		{
			a_Edge.tri2 = -1;
			a_Map.insert(a_Edge);
		}
		else
		{
			if (it->tri2 != -1) return;
			if (it->tri1 != a_Edge.tri1) return;

			it->tri2 = a_Edge.tri1;
		}
	}

	bool cMath::CreateEdges(tTriEdgeVec &a_vEdges,
								const unsigned int *a_pIndexArray, int a_lIndexNum,
								const float *a_pVertexArray, int a_lVtxStride, int a_lVertexNum,
								bool *a_pIsDoubleSized)
	{
		const bool bLog = false;

		*a_pIsDoubleSized = false;

		tVtxIdxMap mapVtxIndices;
		tTriEdgeListMap mapTriEdgeLists;

		g_pIndexArray = a_pIndexArray;
		g_pVertexArray = a_pVertexArray;
		g_lVertexStride = a_lVtxStride;

		for (int idx=0; idx<a_lIndexNum; idx++)
		{
			cVector3f vVtx = GetVector3(a_pVertexArray, a_pIndexArray[idx], a_lVtxStride);
			if (bLog) Log("Checking idx: %d with vec: %s, ", idx, vVtx.ToString());

			tVtxIdxMapIt it = mapVtxIndices.find(vVtx);
			if (it != mapVtxIndices.end())
			{
				if (bLog) Log("Allready addeed, appending!\n");
				it->second.m_lstIndices.push_back(idx);
			}
			else
			{
				if (bLog) Log("Adding as new!");
				mapVtxIndices.insert(tVtxIdxMap::value_type(vVtx, cVertexIndices(idx)));
			}
		}
		if (bLog)
		{
		}

		tVtxIdxMapIt VtxIt = mapVtxIndices.begin();
		for (; VtxIt != mapVtxIndices.end(); ++VtxIt)
		{
			const cVector3f vVtx = VtxIt->first;
			cVertexIndices &Data = VtxIt->second;

			tUIntListIt it = Data.m_lstIndices.begin();
			for (; it != Data.m_lstIndices.end(); ++it)
			{
				int lTriIdx = ((*it)/3)*3;
				unsigned int lVtx = a_pIndexArray[*it];

				cTriEdge edge1, edge2;
				edge1.point1 = lVtx; edge2.point1 = lVtx;
				edge1.tri1 = lTriIdx/3; edge2.tri1 = lTriIdx/3;

				//Get the index the vertex has in the tri (0 - 2)
				int lIdxInTri = (*it) % 3;

				//Get the end points of the edge
				int lPoint1 = lIdxInTri+1;
				if (lPoint1>2) lPoint1 = 0;
				int lPoint2 = lIdxInTri-1;
				if (lPoint2<0) lPoint2 = 2;

				edge1.point2 = a_pIndexArray[lTriIdx + lPoint1];
				edge2.point2 = a_pIndexArray[lTriIdx + lPoint2];

				CheckEdgeSwitch(&edge1);
				CheckEdgeSwitch(&edge2);

				AddEdgeToMap(edge1, mapTriEdgeLists);
				AddEdgeToMap(edge2, mapTriEdgeLists);
			}
		}

		if (bLog)
		{
		}

		a_vEdges.reserve(mapTriEdgeLists.size());
		tTriEdgeListMapIt EdgeIt = mapTriEdgeLists.begin();
		for (;EdgeIt != mapTriEdgeLists.end(); ++EdgeIt)
		{
			cTriEdge &Edge = const_cast<cTriEdge&>(*EdgeIt);
			const unsigned int *pTri1 = &a_pIndexArray[Edge.tri1 * 3];
			const unsigned int *pTri2 = NULL;
			if (Edge.tri2 >= 0) pTri2 = &a_pIndexArray[Edge.tri2 * 3];

			if (Edge.tri2 == -1)
			{
				Edge.invert_tri2 = true;
				*a_pIsDoubleSized = true;
			}
			else
				Edge.invert_tri2 = false;

			int lPoint1InTri = 0;
			for (int i=0; i<3; i++)
			{
				if (Vector3Equal(a_pVertexArray, pTri1[i], a_pVertexArray, Edge.point1, a_lVtxStride))
				{
					lPoint1InTri = i;
					break;
				}
			}
			//Next position in the triangle
			int lNextInTri = lPoint1InTri + 1;
			if (lNextInTri >= 3) lNextInTri = 0;

			if (Vector3Equal(a_pVertexArray, pTri1[lNextInTri], a_pVertexArray, Edge.point2, a_lVtxStride))
			{
				unsigned int lTemp = Edge.point1;
				Edge.point1 = Edge.point2;
				Edge.point2 = lTemp;
			}

			a_vEdges.push_back(Edge);
		}

		return true;
	}
}