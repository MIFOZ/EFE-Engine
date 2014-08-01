#ifndef EFE_PIXELFORMAT_H
#define EFE_PIXELFORMAT_H

#include "system/SystemTypes.h"

namespace efe
{
	class iPixelFormat
	{
	public:
		iPixelFormat(tString a_sType) : m_sType(a_sType) {}
		~iPixelFormat(){};
	private:
		tString m_sType;
	};
};
#endif