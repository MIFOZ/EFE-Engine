#include "resources/FrameTexture.h"
#include "graphics/Texture.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cFrameTexture::cFrameTexture(iTexture *pTex, int a_lHandle)
	{
		m_pTexture = pTex;
		m_lHandle = a_lHandle;
	}

	cFrameTexture::~cFrameTexture()
	{
		if (m_pTexture) efeDelete(m_pTexture);
		m_pTexture = NULL;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iTexture *cFrameTexture::GetTexture()
	{
		return m_pTexture;
	}
}