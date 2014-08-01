#ifndef EFE_COLLIDE_DATA_H
#define EFE_COLLIDE_DATA_H

#include "math/MathTypes.h"
#include "system/SystemTypes.h"

namespace efe
{
	class cCollidePoint
	{
	public:
		cVector3f m_vPoint;
		cVector3f m_vNormal;
		float m_fDepth;
	};

	typedef std::vector<cCollidePoint> tCollidePointVec;
	typedef tCollidePointVec::iterator tCollidePointVecIt;

	class cCollideData
	{
	public:
		tCollidePointVec m_vContactPoints;
		int m_lNumOfPoints;

		void SetMaxSize(int a_lSize)
		{
			m_vContactPoints.resize(a_lSize);
		}
	};
};
#endif