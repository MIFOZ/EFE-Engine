#ifndef EFE_VECTOR2_H
#define EFE_VECTOR2_H

#include <math.h>
#include <cstdio>
#include "system/SystemTypes.h"

namespace efe
{
	template <class T> class cVector2
	{
	public:
		union
		{
			struct
			{
				T x,y;
			};
			T v[2];
		};

		cVector2()
		{
			x=0;y=0;
		}
		cVector2(T a_Val)
		{
			x=a_Val;y=a_Val;
		}
		cVector2(T a_X, T a_Y)
		{
			x=a_X;y=a_Y;
		}

		cVector2(cVector2<T> const &a_Vec)
		{
			x = a_Vec.x; y = a_Vec.y;
		}

		//////////////////////////////////////////////////////////////
		// Copy
		//////////////////////////////////////////////////////////////

		inline cVector2<T> &operator=(const cVector2<T> &a_Vec)
		{
			x = a_Vec.x;
			y = a_Vec.y;
			return *this;
		}

		inline cVector2<T> &operator=(const T a_Val)
		{
			x = a_Val;
			y = a_Val;
			return *this;
		}

		//////////////////////////////////////////////////////////////
		// Boolean
		//////////////////////////////////////////////////////////////

		inline bool operator==(const cVector2<T> &a_Vec) const
		{
			if (x == a_Vec.x && y == a_Vec.y) return true;
			else return false;
		}

		inline bool operator!=(const cVector2<T> &a_Vec) const
		{
			if (x == a_Vec.x && y == a_Vec.y) return false;
			else return true;
		}

		//////////////////////////////////////////////////////////////
		// Vector Arithmetic
		//////////////////////////////////////////////////////////////

		inline cVector2<T> operator+(const cVector2<T> &a_Vec) const
		{
			cVector2<T> vec;
			vec.x = x + a_Vec.x;
			vec.y = y + a_Vec.y;
			return vec;
		}

		inline cVector2<T> operator-(const cVector2<T> &a_Vec) const
		{
			cVector2<T> vec;
			vec.x = x - a_Vec.x;
			vec.y = y - a_Vec.y;
			return vec;
		}

		inline cVector2<T> operator*(const cVector2<T> &a_Vec) const
		{
			cVector2<T> vec;
			vec.x = x * a_Vec.x;
			vec.y = y * a_Vec.y;
			return vec;
		}

		inline cVector2<T> operator/(const cVector2<T> &a_Vec) const
		{
			cVector2<T> vec;
			vec.x = x / a_Vec.x;
			vec.y = y / a_Vec.y;
			return vec;
		}

		inline cVector2<T> &operator+=(const cVector2<T> &a_Vec)
		{
			x+=a_Vec.x;
			y+=a_Vec.y;
			return *this;
		}

		inline cVector2<T> &operator-=(const cVector2<T> &a_Vec)
		{
			x-=a_Vec.x;
			y-=a_Vec.y;
			return *this;
		}

		inline cVector2<T> &operator*=(const cVector2<T> &a_Vec)
		{
			x*=a_Vec.x;
			y*=a_Vec.y;
			return *this;
		}

		inline cVector2<T> &operator/=(const cVector2<T> &a_Vec)
		{
			x/=a_Vec.x;
			y/=a_Vec.y;
			return *this;
		}

		//////////////////////////////////////////////////////////////
		// Real Arithmetic
		//////////////////////////////////////////////////////////////

		inline cVector2<T> operator/(const T a_Val) const
		{
			cVector2<T> vec;
			vec.x = x / a_Val;
			vec.y = y / a_Val;
			return vec;
		}

		inline cVector2<T> operator*(const T a_Val) const
		{
			cVector2<T> vec;
			vec.x = x * a_Val;
			vec.y = y * a_Val;
			return vec;
		}

		inline cVector2<T> operator+(const T a_Val) const
		{
			cVector2<T> vec;
			vec.x = x + a_Val;
			vec.y = y + a_Val;
			return vec;
		}

		inline cVector2<T> operator-(const T a_Val) const
		{
			cVector2<T> vec;
			vec.x = x - a_Val;
			vec.y = y - a_Val;
			return vec;
		}

		//////////////////////////////////////////////////////////////
		// Methods
		//////////////////////////////////////////////////////////////

		inline  void FromVec(const T *a_pVec)
		{
			x = a_pVec[0];
			y = a_pVec[1];
		}

		T Normalize()
		{
			T length = sqrt(x * x + y * y);

			if (length > 1e-08)
			{
				T InvLength = 1.0f / length;
				x *= InvLength;
				y *= InvLength;
			}

			return length;
		}

		tString ToString() const
		{
			char buf[512];
			sprintf(buf,"%f : %f", x, y);
			tString str = buf;
			return str;
		}

		tString ToFileString() const
		{
			char buf[512];
			sprintf(buf,"%g %g", x, y);
			tString str = buf;
			return str;
		}
	};
};
#endif