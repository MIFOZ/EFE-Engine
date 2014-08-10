#ifndef EFE_MATH_H
#define EFE_MATH_H

#include "math/MathTypes.h"
#include "math/BoundingVolume.h"
#include "math/MeshTypes.h"

#include "graphics/Color.h"

namespace efe
{
	class cMath
	{
	public:
		//////////////////////////////////////////////////////////////
		// RANDOM GENERATION
		//////////////////////////////////////////////////////////////

		static int RandRectl(int a_lMin, int a_lMax);

		static float RandRectf(float a_fMin, float a_fMax);

		//////////////////////////////////////////////////////////////
		// COLLISION
		//////////////////////////////////////////////////////////////

		static bool BoxCollision(cRectf a_Rect1, cRectf a_Rect2);

		static bool PointBoxCollision(cVector2f a_vPoint, cRectf a_Rect);

		static bool BoxFit(cRectl a_RectSrc, cRectl a_RectDest);
		static bool BoxFit(cRectf a_RectSrc, cRectf a_RectDest);

		static cRectf &ClipRect(cRectf &a_RectSrc, const cRectf &a_RectDest);

		static bool CheckCollisionBV(cBoundingVolume &a_BV1, cBoundingVolume &a_BV2);

		static bool PointBVCollision(const cVector3f &a_vPoint, cBoundingVolume &a_BV);

		static bool GetClipRectFromBV(cRectl &a_DestRect, cBoundingVolume &a_BV,
									const cMatrixf &a_mtxView, const cMatrixf &a_mtxProj,
									float a_fNearClipPlane, const cVector2l &a_vScreenSize);

		static float Modulus(float a_fDivident, float a_fDivisor);

		static float ToRad(float a_fAngle);
		static float ToDeg(float a_fAngle);

		static int Log2ToInt(int a_lX);

		static bool IsPow2(int a_lX);

		static float Wrap(float a_fX, float a_fMin, float a_fMax);

		__forceinline static float Max(float a_fX, float a_fY)
		{
			if (a_fX > a_fY) return a_fX;
			return a_fY;
		}

		__forceinline static float Min(float a_fX, float a_fY)
		{
			if (a_fX < a_fY) return a_fX;
			return a_fY;
		}

		inline static float Abs(float a_fX){return a_fX < 0 ? -a_fX : a_fX;}

		static float GetAngleFromPoints2D(const cVector2f &a_vStartPos, const cVector2f &a_vGoalPos); 

		static inline cVector3f Vector3Normalize(const cVector3f &a_vVec)
		{
			cVector3f vNorm = a_vVec;
			vNorm.Normalize();
			return vNorm;
		}

		static __forceinline float Vector3DistSqr(const cVector3f &a_vStartPos, const cVector3f &a_vEndPos)
		{
			float fDX = a_vEndPos.x - a_vStartPos.x;
			float fDY = a_vEndPos.y - a_vStartPos.y;
			float fDZ = a_vEndPos.z - a_vStartPos.z;

			return fDX*fDX + fDY*fDY + fDZ*fDZ;
		}

		static __forceinline float Vector3DDist(const cVector3f &a_vStartPos, const cVector3f &a_vEndPos)
		{
			return sqrt(Vector3DistSqr(a_vStartPos, a_vEndPos));
		}

		static cVector3f Vector3Cross(const cVector3f &a_vVecA, const cVector3f &a_vVecB);

		static float Vector3Dot(const cVector3f &a_vVecA, const cVector3f &a_vVecB);

		static float Vector3Angle(const cVector3f &a_vVecA, const cVector3f &a_vVecB);

		static float PlaneToPointDist(const cPlanef &a_Plane, const cVector3f &a_vVec);

		static cQuaternion QuaternionSlerp(float a_fT, const cQuaternion &a_qA, const cQuaternion &a_qB,
			bool a_bShortestPath);

		static float QuaternionDot(const cQuaternion &a_qA, const cQuaternion &a_qB);


		static cMatrixf MatrixMul(const cMatrixf &a_mtxA,const cMatrixf &a_mtxB);

		static cVector3f MatrixMul(const cMatrixf &a_mtxA,const cVector3f &a_vB);

		static cVector3f MatrixMulDivideW(const cMatrixf &a_mtxA,const cVector3f &a_vB);

		static cMatrixf MatrixMulScalar(const cMatrixf &a_mtxA, float a_fB);

		static cMatrixf MatrixRotate(cVector3f a_vRot, eEulerRotationOrder a_Order);

		static cMatrixf MatrixRotateX(float a_fAngle);

		static cMatrixf MatrixRotateY(float a_fAngle);

		static cMatrixf MatrixRotateZ(float a_fAngle);

		static cMatrixf MatrixPerspectiveY(float a_fAspect, int m_lWidth, int m_lHeight, float a_fNearPlane, float a_fFarPlane);

		static cMatrixf ToD3DPerspective(const cMatrixf &a_mtxA);

		static cMatrixf MatrixScale(cVector3f a_vScale);
		static cMatrixf MatrixTranslate(cVector3f a_vTrans);

		static cMatrixf MatrixQuaternion(const cQuaternion &a_qRot);

		static inline float MatrixMinor(const cMatrixf a_mtxA,
			const size_t r0, const size_t r1, const size_t r2,
			const size_t c0, const size_t c1, const size_t c2);

		static inline cMatrixf MatrixAdjoint(const cMatrixf &a_mtxA);

		static inline float MatrixDetermiant(const cMatrixf &a_mtxA);

		static cMatrixf MatrixInverse(const cMatrixf &a_mtxA);

		static const char *MatrixToChar(const cMatrixf &a_mtxA);

		static bool CreateTriTangentVectors(float *a_pDestArray,
										const unsigned int *a_pIndexArray, int a_lIndexNum,
										const float *a_pVertexArray, int a_lVtxStride,
										const float *a_pTexArray,
										const float *a_pNormalArray, int a_lVertexNum);

		static bool CreateTriangleData(tTriangleDataVec &a_vTriangles,
										const unsigned int *a_pIndexArray, int a_lIndexNum,
										const float *a_pVertexArray, int a_lVtxStride, int a_lVertexNum);

		static bool CreateEdges(tTriEdgeVec &a_vEdges,
								const unsigned int *a_pIndexArray, int a_lIndexNum,
								const float *a_pVertexArray, int a_lVtxStride, int a_lVertexNum,
								bool *a_pIsDoubleSized);

		//My Impl
		static cVector3f RGBEToVector3f(unsigned char *pRGBE);
		
		static float Clamp(float a_fX, float a_fMin, float a_fMax);
		static cVector3f Clamp(const cVector3f &a_vVec, const float a_fC0, const float a_fC1);

		static float Saturate(const float a_vV);

	};
};

#endif