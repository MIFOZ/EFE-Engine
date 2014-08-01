/*#include "impl/MouseSDL.h"

#include "SDL.h"

#include "graphics/LowLevelGraphics.h"
#include "impl/LowLevelInputSDL.h"

namespace efe
{
	cMouseSDL::cMouseSDL(cLowLevelInputSDL *a_pLowLevelInputSDL, iLowLevelGraphics *a_pLowLevelGraphics)
	{
		m_fMaxPercent = 0.7f;
		m_fMinPercent = 0.1f;
		m_lBufferSize = 6;

		m_pLowLevelInputSDL = a_pLowLevelInputSDL;
		m_pLowLevelGraphics = a_pLowLevelGraphics;

		m_vMouseRelPos = D3DXVECTOR2(0,0);
		m_vMouseAbsPos = D3DXVECTOR2(0,0);

		m_bWheelUpMoved = false;
		m_bWheelDownMoved = false;
	}

	void cMouseSDL::Update()
	{
		D3DXVECTOR2 vScreenSize = m_pLowLevelGraphics->GetScreenSize();
		D3DXVECTOR2 vVirtualSize = m_pLowLevelGraphics->GetVirtualSize();

		m_bWheelUpMoved = false;
		m_bWheelDownMoved = false;

		//std::list<SDL_Event>::iterator it = m_pLowLevelInputSDL->
	}
}*/