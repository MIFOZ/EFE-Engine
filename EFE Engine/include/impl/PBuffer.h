#ifndef EFE_P_BUFFER_H
#define EFE_P_BUFFER_H

#include "graphics/GraphicsTypes.h"
#include <D3D11.h>

namespace efe
{
	class iLowLevelGraphics;

	class cPBuffer
	{
	public:
		cPBuffer(iLowLevelGraphics *a_pLowLevelGraphics, bool a_bShareObjects, bool a_bUseMipMaps=false, bool a_bUseDepth=true, bool a_bUseStencil=true);
		~cPBuffer();

		bool Init(unsigned int a_lWidth, unsigned int a_lHeight, cColor a_Col);

		int MakeCurrentContext();

		void Bind();
		void UnBind();
	private:
		ID3D11ShaderResourceView *m_pBuffer;

		int m_lWidth;
		int m_lHeight;

		bool m_bShareObjects;
		iLowLevelGraphics *m_pLowLevelGraphics;

		std::vector<int> m_vAttribBuffer;
		std::vector<int> m_vAttribFormat;
	};
};
#endif