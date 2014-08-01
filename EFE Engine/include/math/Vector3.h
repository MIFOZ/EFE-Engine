#ifndef EFE_VECTOR3_H
#define EFE_VECTOR3_H

#include <math.h>
#include "math/Vector2.h"

namespace efe
{
	template <class T> class cVector3
	{
	public:
		union
		{
			struct
			{
				T x,y,z;
			};
			T v[3];
		};

		cVector3()
		{
			x=0;y=0;z=0;
		}
		cVector3(T a_Val)
		{
			x=a_Val;y=a_Val;z=a_Val;
		}
		cVector3(T a_X, T a_Y, T a_Z)
		{
			x=a_X;y=a_Y;z=a_Z;
		}

		cVector3(cVector3<T> const &a_Vec)
		{
			x = a_Vec.x; y = a_Vec.y; z = a_Vec.z;
		}

		cVector3(cVector2<T> const &a_Vec)
		{
			x = a_Vec.x; y = a_Vec.y; z = 0;
		}

		static const cVector3<T> Left;
		static const cVector3<T> Right;
		static const cVector3<T> Up;
		static const cVector3<T> Down;
		static const cVector3<T> Forward;
		static const cVector3<T> Back;

		//////////////////////////////////////////////////////////////
		// Copy
		//////////////////////////////////////////////////////////////

		inline cVector3<T> &operator=(const cVector3<T> &a_Vec)
		{
			x = a_Vec.x;
			y = a_Vec.y;
			z = a_Vec.z;
			return *this;
		}

		inline cVector3<T> &operator=(const cVector2<T> &a_Vec)
		{
			x = a_Vec.x;
			y = a_Vec.y;
			return *this;
		}

		inline cVector3<T> &operator=(const T a_Val)
		{
			x = a_Val;
			y = a_Val;
			z = a_Val;
			return *this;
		}

		//////////////////////////////////////////////////////////////
		// Boolean
		//////////////////////////////////////////////////////////////

		inline bool operator==(const cVector3<T> &a_Vec) const
		{
			if (x == a_Vec.x && y == a_Vec.y && z == a_Vec.z) return true;
			else return false;
		}

		inline bool operator!=(const cVector3<T> &a_Vec) const
		{
			if (x == a_Vec.x && y == a_Vec.y && z == a_Vec.z) return false;
			else return true;
		}

		inline bool operator<(const cVector3<T> &a_Vec) const
		{
			if (x != a_Vec.x) return x < a_Vec.x;
			if (y != a_Vec.y) return y < a_Vec.y;
			return z < a_Vec.z;
		}

		inline bool operator>(const cVector3<T> &a_Vec) const
		{
			if (x != a_Vec.x) return x > a_Vec.x;
			if (y != a_Vec.y) return y > a_Vec.y;
			return z > a_Vec.z;
		}

		//////////////////////////////////////////////////////////////
		// Vector3 Arithmetic
		//////////////////////////////////////////////////////////////

		inline cVector3<T> operator-() const
		{
			cVector3<T> vec;
			vec.x = -x;
			vec.y = -y;
			vec.z = -z;
			return vec;
		}

		inline cVector3<T> operator+(const cVector3<T> &a_Vec) const
		{
			cVector3<T> vec;
			vec.x = x + a_Vec.x;
			vec.y = y + a_Vec.y;
			vec.z = z + a_Vec.z;
			return vec;
		}

		inline cVector3<T> operator-(const cVector3<T> &a_Vec) const
		{
			cVector3<T> vec;
			vec.x = x - a_Vec.x;
			vec.y = y - a_Vec.y;
			vec.z = z - a_Vec.z;
			return vec;
		}

		inline cVector3<T> operator*(const cVector3<T> &a_Vec) const
		{
			cVector3<T> vec;
			vec.x = x * a_Vec.x;
			vec.y = y * a_Vec.y;
			vec.z = z * a_Vec.z;
			return vec;
		}

		inline cVector3<T> operator/(const cVector3<T> &a_Vec) const
		{
			cVector3<T> vec;
			vec.x = x / a_Vec.x;
			vec.y = y / a_Vec.y;
			vec.z = z / a_Vec.z;
			return vec;
		}

		inline cVector3<T> &operator+=(const cVector3<T> &a_Vec)
		{
			x+=a_Vec.x;
			y+=a_Vec.y;
			z+=a_Vec.z;
			return *this;
		}

		inline cVector3<T> &operator-=(const cVector3<T> &a_Vec)
		{
			x-=a_Vec.x;
			y-=a_Vec.y;
			z-=a_Vec.z;
			return *this;
		}

		inline cVector3<T> &operator*=(const cVector3<T> &a_Vec)
		{
			x*=a_Vec.x;
			y*=a_Vec.y;
			z*=a_Vec.z;
			return *this;
		}

		inline cVector3<T> &operator/=(const cVector3<T> &a_Vec)
		{
			x/=a_Vec.x;
			y/=a_Vec.y;
			z/=a_Vec.z;
			return *this;
		}

		//////////////////////////////////////////////////////////////
		// Vector2 Arithmetic
		//////////////////////////////////////////////////////////////

		inline cVector3<T> operator+(const cVector2<T> &a_Vec) const
		{
			cVector3<T> vec;
			vec.x = x + a_Vec.x;
			vec.y = y + a_Vec.y;
			vec.z = z;
			return vec;
		}

		inline cVector3<T> operator-(const cVector2<T> &a_Vec) const
		{
			cVector3<T> vec;
			vec.x = x - a_Vec.x;
			vec.y = y - a_Vec.y;
			vec.z = z;
			return vec;
		}

		inline cVector3<T> operator*(const cVector2<T> &a_Vec) const
		{
			cVector3<T> vec;
			vec.x = x * a_Vec.x;
			vec.y = y * a_Vec.y;
			vec.z = z;
			return vec;
		}

		inline cVector3<T> operator/(const cVector2<T> &a_Vec) const
		{
			cVector3<T> vec;
			vec.x = x / a_Vec.x;
			vec.y = y / a_Vec.y;
			vec.z = z;
			return vec;
		}

		inline cVector3<T> &operator+=(const cVector2<T> &a_Vec)
		{
			x+=a_Vec.x;
			y+=a_Vec.y;
			return *this;
		}

		inline cVector3<T> &operator-=(const cVector2<T> &a_Vec)
		{
			x-=a_Vec.x;
			y-=a_Vec.y;
			return *this;
		}

		inline cVector3<T> &operator*=(const cVector2<T> &a_Vec)
		{
			x*=a_Vec.x;
			y*=a_Vec.y;
			return *this;
		}

		inline cVector3<T> &operator/=(const cVector2<T> &a_Vec)
		{
			x/=a_Vec.x;
			y/=a_Vec.y;
			return *this;
		}

		//////////////////////////////////////////////////////////////
		// Single Float Arithmetic
		//////////////////////////////////////////////////////////////

		inline cVector3<T> operator/(const T a_Val) const
		{
			cVector3<T> vec;
			vec.x = x / a_Val;
			vec.y = y / a_Val;
			vec.z = z / a_Val;
			return vec;
		}

		inline cVector3<T> operator*(const T a_Val) const
		{
			cVector3<T> vec;
			vec.x = x * a_Val;
			vec.y = y * a_Val;
			vec.z = z * a_Val;
			return vec;
		}

		inline cVector3<T> operator+(const T a_Val) const
		{
			cVector3<T> vec;
			vec.x = x + a_Val;
			vec.y = y + a_Val;
			vec.z = z + a_Val;
			return vec;
		}

		inline cVector3<T> operator-(const T a_Val) const
		{
			cVector3<T> vec;
			vec.x = x - a_Val;
			vec.y = y - a_Val;
			vec.z = z - a_Val;
			return vec;
		}

		inline cVector3<T> operator/=(const T a_Val) const
		{
			cVector3<T> vec;
			vec.x = x / a_Val;
			vec.y = y / a_Val;
			vec.z = z / a_Val;
			return vec;
		}

		//////////////////////////////////////////////////////////////
		// Methods
		//////////////////////////////////////////////////////////////

		inline  void FromVec(const T *a_pVec)
		{
			x = a_pVec[0];
			y = a_pVec[1];
			z = a_pVec[2];
		}

		inline T Length()
		{
			return sqrt(x * x + y * y + z * z);
		}

		inline T SqrLegnth()
		{
			return x * x + y * y + z * z;
		}

		T Normalize()
		{
			T length = sqrt(x * x + y * y + z * z);

			if (length > 1e-08)
			{
				T InvLength = 1.0f / length;
				x *= InvLength;
				y *= InvLength;
				z *= InvLength;
			}

			return length;
		}

		tString ToString() const
		{
			char buf[512];
			sprintf(buf,"%.10f : %.10f : %.10f", x, y, z);
			tString str = buf;
			return str;
		}

		tString ToFileString() const
		{
			char buf[512];
			sprintf(buf,"%g %g %g", x, y, z);
			tString str = buf;
			return str;
		}
	};
};
#endif