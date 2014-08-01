#include "scene/Camera3D.h"

#include "math/Math.h"
#include "graphics/LowLevelGraphics.h"
#include "scene/Entity3D.h"

namespace efe
{
	cCamera3D::cCamera3D()
	{
		m_vPosition = cVector3f(0);

		m_fFOV = cMath::ToRad(70.0f);
		m_fAspect = 4.0f/3.0f;
		m_fFarClipPlane = 1000.0f;
		m_fNearClipPlane = 0.05f;

		m_fPitch = 0;
		m_fYaw = 0;
		m_fRoll = 0;

		m_RotateMode = eCameraRotateMode_EulerAngles;
		m_MoveMode = eCameraMoveMode_Fly;

		m_mtxView = cMatrixf::Identity;
		m_mtxProjection = cMatrixf::Identity;
		m_mtxMove = cMatrixf::Identity;

		m_bViewUpdated = true;
		m_bProjectionUpdated = true;
		m_bMoveUpdated = true;

		m_bInfFarPlane = true;

		m_vPitchLimits = cVector2f(kPif/2.0f, -kPif/2.0f);
		m_vYawLimits = cVector2f(0,0);
	}

	cCamera3D::~cCamera3D()
	{
	}

	void cCamera3D::SetPosition(const cVector3f &a_vPos)
	{
		m_vPosition = a_vPos;
		m_bViewUpdated = true;

		m_Node.SetPosition(m_vPosition);
	}

	void cCamera3D::SetPitch(float a_fAngle)
	{
		m_fPitch = a_fAngle;

		if (m_vPitchLimits.x!=0 || m_vPitchLimits.y!=0)
		{
			if (m_fPitch > m_vPitchLimits.x) m_fPitch = m_vPitchLimits.x;
			if (m_fPitch < m_vPitchLimits.y) m_fPitch = m_vPitchLimits.y;
		}

		m_bMoveUpdated = true; m_bViewUpdated = true;
	}

	void cCamera3D::SetYaw(float a_fAngle)
	{
		m_fYaw = a_fAngle;

		if (m_vYawLimits.x!=0 || m_vYawLimits.y!=0)
		{
			if (m_fYaw > m_vYawLimits.x) m_fYaw = m_vYawLimits.x;
			if (m_fYaw < m_vYawLimits.y) m_fYaw = m_vYawLimits.y;
		}

		m_bMoveUpdated = true; m_bViewUpdated = true;
	}

	void cCamera3D::SetRoll(float a_fAngle)
	{
		m_fRoll = a_fAngle;
		m_bMoveUpdated = true; m_bViewUpdated = true;
	}

	void cCamera3D::AddPitch(float a_fAngle)
	{
		m_fPitch += a_fAngle;

		if (m_vPitchLimits.x!=0 || m_vPitchLimits.y!=0)
		{
			if (m_fPitch > m_vPitchLimits.x) m_fPitch = m_vPitchLimits.x;
			if (m_fPitch < m_vPitchLimits.y) m_fPitch = m_vPitchLimits.y;
		}

		m_bMoveUpdated = true; m_bViewUpdated = true;
	}

	void cCamera3D::AddYaw(float a_fAngle)
	{
		m_fYaw += a_fAngle;

		if (m_vYawLimits.x!=0 || m_vYawLimits.y!=0)
		{
			if (m_fYaw > m_vYawLimits.x) m_fYaw = m_vYawLimits.x;
			if (m_fYaw < m_vYawLimits.y) m_fYaw = m_vYawLimits.y;
		}

		m_bMoveUpdated = true; m_bViewUpdated = true;
	}

	void cCamera3D::AddRoll(float a_fAngle)
	{
		m_fRoll += a_fAngle;
		m_bMoveUpdated = true; m_bViewUpdated = true;
	}

	void cCamera3D::MoveForward(float a_fDist)
	{
		UpdateMoveMatrix();

		m_vPosition += m_mtxMove.GetForward()*-a_fDist;

		m_bViewUpdated = true;

		m_Node.SetPosition(m_vPosition);
	}

	void cCamera3D::MoveRight(float a_fDist)
	{
		UpdateMoveMatrix();

		m_vPosition += m_mtxMove.GetRight()*a_fDist;

		m_bViewUpdated = true;

		m_Node.SetPosition(m_vPosition);
	}

	void cCamera3D::MoveUp(float a_fDist)
	{
		UpdateMoveMatrix();

		m_vPosition += m_mtxMove.GetUp()*a_fDist;

		m_bViewUpdated = true;

		m_Node.SetPosition(m_vPosition);
	}

	void cCamera3D::SetMoveMode(eCameraMoveMode a_Mode)
	{
		m_MoveMode = a_Mode;
		m_bMoveUpdated = true;
	}

	void cCamera3D::ResetRotation()
	{
		m_bViewUpdated = false;
		m_bMoveUpdated = false;

		m_mtxMove = cMatrixf::Identity;
		m_mtxView = cMatrixf::Identity;

		m_fRoll = 0;
		m_fPitch = 0;
		m_fYaw = 0;
	}

	cFrustum *cCamera3D::GetFrustum()
	{
		bool bWasInf = false;
		if (m_bInfFarPlane)
		{
			SetInfinitiveFarPlane(false);
			bWasInf = true;
		}
		m_Frustum.SetViewProjMatrix(GetProjectionMatrix(), GetViewMatrix(),
									GetFarClipPlane(), GetNearClipPlane(),
									GetFOV(), GetAspect(), GetPosition(), m_bInfFarPlane);
		if(bWasInf)
			SetInfinitiveFarPlane(true);

		return &m_Frustum;
	}

	const cMatrixf &cCamera3D::GetViewMatrix()
	{
		if (m_bViewUpdated)
		{
			if (m_RotateMode == eCameraRotateMode_EulerAngles)
			{
				m_mtxView = cMatrixf::Identity;

				m_mtxView = cMath::MatrixMul(cMath::MatrixTranslate(m_vPosition * -1), m_mtxView);
				m_mtxView = cMath::MatrixMul(cMath::MatrixRotateY(-m_fYaw), m_mtxView);
				m_mtxView = cMath::MatrixMul(cMath::MatrixRotateX(-m_fPitch), m_mtxView);
				m_mtxView = cMath::MatrixMul(cMath::MatrixRotateZ(-m_fRoll), m_mtxView);
			}

			/*cMatrixf mtxPos = cMath::MatrixTranslate(m_vPosition*-1);

			cQuaternion qRot;
			qRot.FromEulerAngles(cVector3f(-m_fYaw, -m_fPitch, -m_fRoll));

			cMatrixf mtxRot = cMath::MatrixQuaternion(qRot);
			
			m_mtxView = cMath::MatrixMul(mtxPos, mtxRot);*/

			m_bViewUpdated = false;
		}
		return m_mtxView;
	}

	const cMatrixf &cCamera3D::GetProjectionMatrix()
	{
		if (m_bProjectionUpdated)
		{
			float fFar = m_fFarClipPlane;
			float fNear = m_fNearClipPlane;
			float fTop = tan(m_fFOV*0.5f) * fNear;
			float fBottom = -fTop;
			float fRight = m_fAspect * fTop;
			float fLeft = m_fAspect * fBottom;

			float A = (2.0f*fNear) / (fRight-fLeft);
			float B = (2.0f*fNear) / (fTop-fBottom);
			float D = -1.0f;
			float C,Z;
			if (m_bInfFarPlane)
			{
				C = -2.0f * fNear;
				Z = -1.0f;
			}
			else
			{
				C = -(2.0f*fFar*fNear) / (fFar - fNear);
				Z = -(fFar + fNear)/(fFar - fNear);
			}

			float X = 0;
			float Y = 0;

			m_mtxProjection = cMatrixf(	A,0,X,0,
										0,B,Y,0,
										0,0,Z,C,
										0,0,D,0);

			m_bProjectionUpdated = false;
		}

		return m_mtxProjection;
	}

	const cMatrixf &cCamera3D::GetMoveMatrix()
	{
		UpdateMoveMatrix();

		return m_mtxMove;
	}

	cVector3f cCamera3D::GetEyePosition()
	{
		return m_vPosition;
	}

	void cCamera3D::SetModelViewMatrix(iLowLevelGraphics *a_pLowLevel)
	{
		
	}

	void cCamera3D::SetProjectionMatrix(iLowLevelGraphics *a_pLowLevel)
	{
		
	}

	/*__forceinline cVector3f *TransformVector(D3DXQUATERNION *a_quatOrientation, cVector3f *a_vAxis)
	{
		D3DXVECTOR3 vNewAxis;
		D3DXMATRIX mtxRotation;

		D3DXMatrixRotationQuaternion(&mtxRotation, a_quatOrientation);

		vNewAxis.x = a_vAxis->x*mtxRotation._11+a_vAxis->y*mtxRotation._21+a_vAxis->z*mtxRotation._31+mtxRotation._41;
		vNewAxis.y = a_vAxis->x*mtxRotation._11+a_vAxis->y*mtxRotation._22+a_vAxis->z*mtxRotation._32+mtxRotation._42;
		vNewAxis.z = a_vAxis->x*mtxRotation._13+a_vAxis->y*mtxRotation._23+a_vAxis->z*mtxRotation._33+mtxRotation._43;

		return &vNewAxis;
	}*/

	/*D3DXVECTOR3 cCamera3D::UnProject(const D3DXVECTOR2 &a_vScreenPos, iLowLevelGraphics *a_pLowLevel)
	{
		
	}*/

	//--------------------------------------------------------------

	cVector3f cCamera3D::GetForward()
	{
		return GetViewMatrix().GetForward()*-1.0f;
	}

	cVector3f cCamera3D::GetRight()
	{
		return GetViewMatrix().GetRight();
	}

	cVector3f cCamera3D::GetUp()
	{
		return GetViewMatrix().GetUp();
	}
	//--------------------------------------------------------------


	void cCamera3D::UpdateMoveMatrix()
	{
		if (m_bMoveUpdated)
		{
			if (m_RotateMode == eCameraRotateMode_EulerAngles)
			{
				m_mtxMove = cMath::MatrixRotateY(-m_fYaw);
				if(m_MoveMode==eCameraMoveMode_Fly)
					m_mtxMove = cMath::MatrixMul(cMath::MatrixRotateX(-m_fPitch),m_mtxMove);
			}

			m_bMoveUpdated  = false;
		}
	}
}