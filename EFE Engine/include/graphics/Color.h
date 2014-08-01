#ifndef EFE_COLOR_H
#define EFE_COLOR_H

#include <list>
#include <vector>

#include "system/SystemTypes.h"

namespace efe
{
	class cColor
	{
	public:
		float r, g, b, a;

		cColor(float a_fR, float a_fG, float a_fB, float a_fA);
		cColor(float a_fR, float a_fG, float a_fB);
		cColor();
		cColor(float a_fVal);
		cColor(float a_fVal, float a_fA);

		cColor operator*(float a_fVal) const;
		cColor operator/(float a_fVal) const;

		cColor operator+(const cColor &a_Col) const;
		cColor operator-(const cColor &a_Col) const;
		cColor operator*(const cColor &a_Col) const;
		cColor operator/(const cColor &a_Col) const;

		bool operator==(cColor a_Col) const;

		tString ToString() const;

		void FromVec(float *a_pV);
	};

	typedef std::list<cColor> tColorList;
	typedef tColorList::iterator tColorListIt;

	typedef std::vector<cColor> tColorVec;
	typedef tColorVec::iterator tColorVecIt;
};
#endif