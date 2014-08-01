#ifndef EFE_MATHTYPES_H
#define EFE_MATHTYPES_H

#include <list>
#include <vector>
#include <cmath>
#include <cstdlib>
#include "math/Half.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix.h"
#include "system/SystemTypes.h"
#include "math/Point.h"

namespace efe
{
	#define kPif (3.141592654f)
	#define kPi2f (1.570796327f)
	#define kPi4f (0.7853981634f)
	#define k2Pif (6.283185307f)

	#define kEpsilonf (0.0001f)

	#define kSqrt2f (1.414213562f)

	enum eEulerRotationOrder
	{
		eEulerRotationOrder_XYZ,
		eEulerRotationOrder_LastEnum
	};

	template <class T> class cPlane
	{
	public:
		T a, b, c, d;
		cVector3<T> normal;

		cPlane(T a_A, T a_B, T a_C, T a_D)
		{
			a = a_A; b = a_B; c = a_C; d = a_D;
		}
		cPlane()
		{
			a = 0; b = 0; c = 0; d = 0;
		}

		cPlane(const cVector3<T> &a_vNormal, const cVector3<T> &a_vPoint)
		{
			FromNormalPoint(a_vNormal, a_vPoint);
		}

		inline void FromNormalPoint(const cVector3<T> &a_vNormal, const cVector3<T> &a_vPoint)
		{
			a = a_vNormal.x;
			b = a_vNormal.y;
			c = a_vNormal.z;

			d = -(a_vNormal.x*a_vPoint.x + a_vNormal.y*a_vPoint.y + a_vNormal.z*a_vPoint.z);
		}

		inline void FromPoints(const cVector3<T> &a_vPoints0, const cVector3<T> &a_vPoints1, 
			const cVector3<T> &a_vPoints2)
		{
			cVector3<T> vEdge1 = a_vPoints1 - a_vPoints0;
			cVector3<T> vEdge2 = a_vPoints2 - a_vPoints0;

			//Cross product
			normal.x = vEdge1.y * vEdge2.z -  vEdge1.z * vEdge2.y;
			normal.y = vEdge1.z * vEdge2.x -  vEdge1.x * vEdge2.z;
			normal.z = vEdge1.x * vEdge2.y -  vEdge1.y * vEdge2.x;

			normal.Normalize();

			a = normal.x;
			b = normal.y;
			c = normal.z;

			//Dot product
			d = -(normal.x*a_vPoints0.x, normal.y*a_vPoints0.y, normal.z*a_vPoints0.z);
		}

		inline void Normalize()
		{
			T fMag = sqrt(a*a + b*b + c*c);
			a = a / fMag;
			b = b / fMag;
			c = c / fMag;
			d = d / fMag;
		}

		inline void CalcNormal()
		{
			normal = cVector3<T>(a,b,c);
			normal.Normalize();
		}
	};
	typedef cPlane<float> cPlanef;
	typedef std::vector<cPlanef> tPlanefVec;
	typedef tPlanefVec::iterator tPlanefVecIt;

	template <class T> class cSphere
	{
	public:
		T r;
		cVector3<T> center;

		cSphere(){}

		cSphere(cVector3<T> a_V, T a_R)
		{
			center = a_V; r = a_R;
		}
	};

	typedef cSphere<float> cSpheref;

	template <class T> class cRect
	{
	public:
		T x,y,w,h;
		cRect(T a_X, T a_Y, T a_W, T a_H)
		{
			x = a_X;y = a_Y;w = a_W;h = a_H;
		}
		cRect(T a_X, T a_Y)
		{
			x = a_X;y = a_Y;w = 0;h = 0;
		}
		cRect()
		{
			x = 0;y = 0;w = 0;h = 0;
		}
		cRect(cVector2<T> a_Pos, cVector2<T> a_Size)
		{
			x = a_Pos.x;y = a_Pos.y;w = a_Size.x;h = a_Size.y;
		}

		inline bool operator==(const cRect<T> &a_Rect) const
		{
			if (x == a_Rect.x && y == a_Rect.y && w == a_Rect.w && h == a_Rect.h) return true;
			else return false;
		}

		void FromVec(T *a_pV)
		{
			x = a_pV[0];
			y = a_pV[1];
			w = a_pV[2];
			h = a_pV[3];
		}
	};

	typedef cRect<int> cRectl;
	typedef cRect<float> cRectf;
	typedef std::list<cRectf> tRect2fList;
	typedef tRect2fList::iterator tRect2fListIt;

	typedef cVector2<float> cVector2f;
	typedef cVector2<int> cVector2l;

	typedef cMatrix<float> cMatrixf;

	typedef std::list<cVector2f> tVector2fList;
	typedef tVector2fList::iterator tVector2fListIt;

	typedef cVector3<float> cVector3f;
	typedef cVector3<int> cVector3l;

	typedef std::list<cVector3f> tVector3fList;
	typedef tVector3fList::iterator tVector3fListIt;

	typedef std::vector<cVector2f> tVector2fVec;
	typedef tVector2fVec::iterator tVector2fVecIt;

	typedef std::vector<cVector3f> tVector3fVec;
	typedef tVector3fVec::iterator tVector3fVecIt;
};

#include "math/Quaternion.h"

#endif