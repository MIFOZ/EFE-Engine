#ifndef EFE_POINT_H
#define EFE_POINT_H

#include <math.h>
#include <stdio.h>
#include "system/systemTypes.h"

namespace efe
{
	class Point
	{
	public:
		union
		{
			struct{
				int x,y;
			};
			int v[2];
		};
		Point()
		{
			x=0, y=0;
		}
		Point(int a_Val)
		{
			x = a_Val; y = a_Val;
		}
		Point(int a_X, int a_Y)
		{
			x = a_X; y = a_Y;
		}

		Point(const Point &a_Pnt)
		{
			x = a_Pnt.x; y = a_Pnt.y;
		}

		__forceinline Point &operator=(const Point &a_Vec)
		{
			x = a_Vec.x; y = a_Vec.y;
			return *this;
		}

		__forceinline Point &operator=(const int a_Val)
		{
			x = a_Val; y = a_Val;
			return *this;
		}
	};
};
#endif