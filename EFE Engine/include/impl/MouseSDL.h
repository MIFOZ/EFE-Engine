#ifndef EFE_MOUSE_SDL_H
#define EFE_MOUSE_SDL_H

#include <vector>
#include "input/Mouse.h"



namespace efe
{
	class iLowLevelGraphics;
	class cLowLevelInputSDL;

	class cMouseSDL : public iMouse
	{
	public:
		cMouseSDL(cLowLevelInputSDL *a_pLowLevelInputSDL, iLowLevelGraphics *a_pLowLevelGraphics);

		bool m_bButtonIsDown(eMButton);

		void Update();

		D3DXVECTOR2 GetRelativePosition();

		void Reset();

		//void SetSmoothProperties()
	private:
		D3DXVECTOR2 m_vMouseAbsPos;
		D3DXVECTOR2 m_vMouseRelPos;

		LPDIRECTINPUTDEVICE8 m_Mouse;
		DIMOUSESTATE2 m_MouseState;
		DIMOUSESTATE2 m_PrevMouseState;

		std::vector<bool> m_vButtonArray;

		tD3DXVECTOR2List m_lstMouseCoord;

		float m_fMaxPercent;
		float m_fMinPercent;
		int m_lBufferSize;

		cLowLevelInputSDL *m_pLowLevelInputSDL;
		iLowLevelGraphics *m_pLowLevelGraphics;

		bool m_bWheelUpMoved;
		bool m_bWheelDownMoved;
	};
};


#endif