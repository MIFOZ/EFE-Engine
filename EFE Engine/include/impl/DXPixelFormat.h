#ifndef EFE_DX_PIXELFORMAT_H
#define EFE_DX_PIXELFORMAT_H

#include "graphics/PixelFormat.h"
#include <DXGI.h>

namespace efe
{
	class cDXPixelFormat : public iPixelFormat
	{
	public:
		cDXPixelFormat() : iPixelFormat("DX")
		{
			*m_pDXPixelFmt32 = DXGI_FORMAT_R8G8B8A8_UNORM;
		}

		DXGI_FORMAT *GetDXPixelFormat32(){return m_pDXPixelFmt32;}
	private:
		DXGI_FORMAT *m_pDXPixelFmt32;
	};
};
#endif