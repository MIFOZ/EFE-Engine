#ifndef EFE_VECTOR4_H
#define EFE_VECTOR4_H

#include "math/Vector3.h"

namespace efe
{
	class cVector4f
	{
	public:
		union
		{
			struct
			{
				float x,y,z,w;
			};
			float v[4];
		};

		cVector4f()
		{
			x=0;y=0;z=0;w=0;
		}
		cVector4f(float a_Val)
		{
			x=a_Val;y=a_Val;z=a_Val;w=a_Val;
		}
		cVector4f(float a_X, float a_Y, float a_Z, float a_W)
		{
			x=a_X;y=a_Y;z=a_Z;w=a_W;
		}
		cVector4f(float a_XYZ, float a_W)
		{
			x=a_XYZ;y=a_XYZ;z=a_XYZ;w=a_W;
		}
		cVector4f(cVector3<float> a_vXYZ, float a_W)
		{
			x=a_vXYZ.x;y=a_vXYZ.y;z=a_vXYZ.z;w=a_W;
		}

		//////////////////////////////////////////////////////////////
		// Copy
		//////////////////////////////////////////////////////////////

		inline cVector4f &operator=(const cVector4f &a_Vec)
		{
			x = a_Vec.x;
			y = a_Vec.y;
			z = a_Vec.z;
			w = a_Vec.w;
			return *this;
		}

		inline cVector4f &operator=(const float a_Val)
		{
			x = a_Val;
			y = a_Val;
			z = a_Val;
			w = a_Val;
			return *this;
		}

		//////////////////////////////////////////////////////////////
		// Boolean
		//////////////////////////////////////////////////////////////

		inline bool operator==(const cVector4f &a_Vec) const
		{
			if (x == a_Vec.x && y == a_Vec.y && z == a_Vec.z && w == a_Vec.w) return true;
			else return false;
		}

		inline bool operator!=(const cVector4f &a_Vec) const
		{
			if (x == a_Vec.x && y == a_Vec.y && z == a_Vec.z && w == a_Vec.w) return false;
			else return true;
		}

		inline bool operator<(const cVector4f &a_Vec) const
		{
			if (x != a_Vec.x) return x < a_Vec.x;
			if (y != a_Vec.y) return y < a_Vec.y;
			if (z != a_Vec.z) return z < a_Vec.z;
			return w < a_Vec.w;
		}

		inline bool operator>(const cVector4f &a_Vec) const
		{
			if (x != a_Vec.x) return x > a_Vec.x;
			if (y != a_Vec.y) return y > a_Vec.y;
			if (z != a_Vec.z) return z > a_Vec.z;
			return w > a_Vec.w;
		}

		//////////////////////////////////////////////////////////////
		// Vector4 Arithmetic
		//////////////////////////////////////////////////////////////
	};
};

#endif