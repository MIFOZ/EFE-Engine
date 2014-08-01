#ifndef EFE_CAMERA3D_H
#define EFE_CAMERA3D_H

#include "math/MathTypes.h"
#include "scene/Camera.h"
#include "math/Frustum.h"
#include "scene/Node3D.h"

namespace efe
{
	enum eCameraMoveMode
	{
		eCameraMoveMode_Walk,
		eCameraMoveMode_Fly,
		eCameraMoveMode_LastEnum
	};

	enum eCameraRotateMode
	{
		eCameraRotateMode_EulerAngles,
		eCameraRotateMode_Matrix,
		eCameraRotateMode_LastEnum
	};

	class iLowLevelGraphics;
	class iEntity3D;

	class cCamera3D : public iCamera
	{
	public:
		cCamera3D();
		~cCamera3D();

		const cVector3f &GetPosition()const{return m_vPosition;}
		void SetPosition(const cVector3f &a_vPos);

		void MoveForward(float a_fDist);

		void MoveRight(float a_fDist);

		void MoveUp(float a_fDist);

		void SetFOV(float a_fAngle){m_fFOV = a_fAngle, m_bProjectionUpdated = true;}
		float GetFOV(){return m_fFOV;}

		void SetAspect(float a_fAngle){m_fAspect = a_fAngle, m_bProjectionUpdated = true;}
		float GetAspect(){return m_fAspect;}

		void SetFarClipPlane(float a_fX){m_fFarClipPlane = a_fX, m_bProjectionUpdated = true;}
		float GetFarClipPlane(){return m_fFarClipPlane;}

		void SetNearClipPlane(float a_fX){m_fNearClipPlane = a_fX, m_bProjectionUpdated = true;}
		float GetNearClipPlane(){return m_fNearClipPlane;}

		void SetInfinitiveFarPlane(bool a_bX){m_bInfFarPlane = a_bX, m_bProjectionUpdated = true;}
		bool GetInfinitiveFarPlane(){return m_bInfFarPlane;}

		cFrustum *GetFrustum();

		eCameraMoveMode GetMoveMode(){return m_MoveMode;}

		void SetMoveMode(eCameraMoveMode a_Mode);

		void ResetRotation();

		cVector3f UnProject(const cVector2f &a_vScreenPos, iLowLevelGraphics *a_pLowLevel);

		void SetPitch(float a_fAngle);
		void SetYaw(float a_fAngle);
		void SetRoll(float a_fAngle);

		void AddPitch(float a_fAngle);
		void AddYaw(float a_fAngle);
		void AddRoll(float a_fAngle);

		float GetPitch(){return m_fPitch;}
		float GetYaw(){return m_fYaw;}
		float GetRoll(){return m_fRoll;}

		void SetPitchLimits(cVector2f a_vLimits){m_vPitchLimits = a_vLimits;}
		const cVector2f &GetPitchLimits(){return m_vPitchLimits;}

		void SetYawLimits(cVector2f a_vLimits){m_vYawLimits = a_vLimits;}
		const cVector2f &GetYawLimits(){return m_vYawLimits;}

		const cMatrixf &GetViewMatrix();
		const cMatrixf &GetProjectionMatrix();

		const cMatrixf &GetMoveMatrix();

		//iCamera stuff
		void SetModelViewMatrix(iLowLevelGraphics *a_pLowLevel);
		void SetProjectionMatrix(iLowLevelGraphics *a_pLowLevel);
		cVector3f GetEyePosition();

		eCameraType GetType(){return eCameraType_3D;}

		cVector3f GetForward();
		cVector3f GetRight();
		cVector3f GetUp();

		void SetPrevView(const cMatrixf &a_mtxA){m_mtxPrevView = a_mtxA;}
		void SetPrevProjection(const cMatrixf &a_mtxA){m_mtxPrevProjection = a_mtxA;}

		cMatrixf &GetPrevView(){return m_mtxPrevView;}
		cMatrixf &GetPrevProjection(){return m_mtxPrevProjection;}

	private:
		void UpdateMoveMatrix();
		cVector3f m_vPosition;

		//cVector3f *TransformVector(D3DXQUATERNION *a_quatOrientation, cVector3f *a_vAxis);

		//D3DXQUATERNION m_qOrientation;

		float m_fFOV;
		float m_fAspect;
		float m_fFarClipPlane;
		float m_fNearClipPlane;

		float m_fPitch;
		float m_fYaw;
		float m_fRoll;

		cVector2f m_vPitchLimits;
		cVector2f m_vYawLimits;

		eCameraRotateMode m_RotateMode;
		eCameraMoveMode m_MoveMode;

		cMatrixf m_mtxView;
		cMatrixf m_mtxProjection;
		cMatrixf m_mtxMove;

		cMatrixf m_mtxPrevView;
		cMatrixf m_mtxPrevProjection;

		cNode3D m_Node;

		cFrustum m_Frustum;

		bool m_bInfFarPlane;

		bool m_bViewUpdated;
		bool m_bProjectionUpdated;
		bool m_bMoveUpdated;
	};
};
#endif