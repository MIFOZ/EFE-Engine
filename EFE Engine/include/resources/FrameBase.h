#ifndef EFE_FRAMEBASE_H
#define EFE_FRAMEBASE_H

#include "math/MathTypes.h"

namespace efe
{
	class iFrameBase
	{
	public:
		iFrameBase()
		{
			m_lPicCount = 0;
		}

		void SetPicCount(int a_lPicCount){m_lPicCount = a_lPicCount;}
		void DecPicCount(){if (m_lPicCount>0) m_lPicCount--;}
		int GetPicCount(){return m_lPicCount;}
		bool IsEmpty(){return m_lPicCount <= 0;}
	protected:
		int m_lPicCount;
	};
};
#endif