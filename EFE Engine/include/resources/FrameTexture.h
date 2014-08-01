#ifndef EFE_FRAME_TEXTURE_H
#define EFE_FRAME_TEXTURE_H

#include "resources/FrameBase.h"

namespace efe
{
	class iTexture;

	class cFrameTexture : public iFrameBase
	{
	public:
		cFrameTexture(iTexture *pTex, int a_lHandle);
		~cFrameTexture();

		iTexture *GetTexture();
		int GetHandle(){return m_lHandle;}
	private:
		iTexture *m_pTexture;
		int m_lHandle;
	};
};
#endif