#ifndef EFE_MOUSE_RAW_H
#define EFE_MOUSE_RAW_H

#include "input/Mouse.h"

namespace efe{
	class cLowLevelInputRaw;
	class iLowLevelGraphics;

	class cMouseDI : public iMouse
	{
	public:
		cMouseDI(cLowLevelInputRaw *a_pLowLevelInputImpl, iLowLevelGraphics *a_pLowLevelGraphics);

		bool ButtonIsDown(eMButton);

		void Update();

		cVector2f GetAbsPosition();

		cVector2f GetRelPosition();

		void Reset();

		void SetSmoothProperties(float a_fMinPercent, float a_fMaxPercent, unsigned int a_lBufferSize);
	private:
		cVector2f m_vMouseAbsPos;
		cVector2f m_vMouseRelPos;

		std::vector<bool> m_vMButtonArray;

		tVector2fList m_lstMouseCoord;

		float m_fMaxPercent;
		float m_fMinPercent;
		int m_lBufferSize;

		cLowLevelInputRaw *m_pLowLevelInputRaw;
		iLowLevelGraphics *m_pLowLevelGraphics;

		bool m_bWheelUpMoved;
		bool m_bWheelDownMoved;
	};
};
#endif