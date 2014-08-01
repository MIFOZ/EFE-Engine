#ifndef EFE_HALF_H
#define EFE_HALF_H

namespace efe
{
	class cHalf
	{
		unsigned short sh;

		cHalf(){};
		cHalf(const float a_fX);
		operator float () const;
	};
};
#endif