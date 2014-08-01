#include "scene/Camera2D.h"

#include  "graphics/LowLevelGraphics.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	cCamera2D::cCamera2D(unsigned int a_lW, unsigned int a_lH)
	{
		m_vPosition = cVector3f(0,0,0);
		m_fAngle = 0;
		m_vClipArea.x = a_lH;
		m_vClipArea.y = a_lW;

		m_fZMin = -100;
		m_fZMax = -100;
		m_fFOV = 60;
	}

	//--------------------------------------------------------------

	cCamera2D::~cCamera2D()
	{
	
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cVector3f cCamera2D::GetEyePostion()
	{
		cVector3f vEyePos = GetPosition();
		vEyePos.z = 100;
		return vEyePos;
	}

	//--------------------------------------------------------------
	
	void cCamera2D::GetClipRect(cRectf &a_Rect)
	{
		float fDiv = 1;
		if (m_vPosition.z < 0)
		{
			fDiv = 1 - std::abs(m_vPosition.z/m_fZMin);
			if (fDiv<=0) fDiv = 0.0001f;
		}
		else
			fDiv = 1 + std::abs(m_vPosition.z/m_fZMax);

		float lW = ((float)m_vClipArea.x)*fDiv;
		float lH = ((float)m_vClipArea.y)*fDiv;

		float lX = m_vPosition.x - lW/2;
		float lY = m_vPosition.y - lH/2;

		a_Rect.x = lX;
		a_Rect.y = lY;
		a_Rect.w = lW;
		a_Rect.h = lH;
	}

	//--------------------------------------------------------------

	void cCamera2D::SetModelViewMatrix(iLowLevelGraphics *a_pLowLevel)
	{
		cRectf ClipRect;
		GetClipRect(ClipRect);
		cVector2f vVirtSize = a_pLowLevel->GetVirtualSize();
		cVector3f vPos = (-(ClipRect.x), -(ClipRect.y),0.0f);
		cVector3f vScale(vVirtSize.x/ClipRect.w, vVirtSize.y/ClipRect.h, 1.0f);
	}

	//--------------------------------------------------------------

	void cCamera2D::SetProjectionMatrix(iLowLevelGraphics *a_pLowLevel)
	{
		
	}
}