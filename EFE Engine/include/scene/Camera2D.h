#ifndef EFE_CAMERA2D_H
#define EFE_CAMERA2D_H

#include "math/MathTypes.h"
#include "Scene/Camera.h"

namespace efe
{
	class iLowLevelGraphics;

	class cCamera2D : public iCamera
	{
	public:
		cCamera2D(unsigned int a_lW, unsigned int a_lH);
		~cCamera2D();

		const cVector3f &GetPosition()const{return m_vPosition;}
		cVector3f GetEyePostion();
		void SetPosition(cVector3f a_vPos) {m_vPosition = a_vPos;}

		void SetXY(cVector2f a_vPos){m_vPosition.x = a_vPos.x;m_vPosition.y = a_vPos.y;}
		void SetZ(float a_fZ){m_vPosition = a_fZ;}
		void MoveXY(cVector2f a_vAdd){m_vPosition.x += a_vAdd.x;m_vPosition.y += a_vAdd.y;}
		void MoveZ(float a_fZ){m_vPosition.z += a_fZ;}

		void GetClipRect(cRectf &a_Rect);

		void SetModelViewMatrix(iLowLevelGraphics *a_pLowLevel);
		void SetProjectionMatrix(iLowLevelGraphics *a_pLowLevel);

		eCameraType GetType(){return eCameraType_2D;}
	private:
		cVector3f m_vPosition;
		float m_fAngle;
		float m_fZMin;
		float m_fZMax;
		float m_fFOV;

		cVector2l m_vClipArea;
	};
};
#endif