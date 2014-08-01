#include "math/Frustum.h"

#include "math/Math.h"
#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"

namespace efe
{
	cFrustum::cFrustum()
	{
	}

	void cFrustum::SetViewProjMatrix(const cMatrixf &a_mtxProj, const cMatrixf &a_mtxView,
			float a_fFarPlane, float a_fNearPlane, float a_fFOV, float a_fAspect,
			const cVector3f &a_vOrigin, bool a_bInfFarPlane)
	{
		m_mtxViewProj = cMath::MatrixMul(a_mtxProj, a_mtxView);
		m_mtxModelView = a_mtxView;

		m_fFarPlane = a_fFarPlane;
		m_fNearPlane = a_fNearPlane;
		m_fFOV = a_fFOV;
		m_fAspect = a_fAspect;

		m_vOrigin = a_vOrigin;

		m_bInfFarPlane = a_bInfFarPlane;

		m_OriginBV.SetSize(a_fNearPlane*2);
		m_OriginBV.SetPosition(m_vOrigin);

		UpdatePlanes();
		UpdateSphere();
		UpdateEndPoints();
		UpdateBV();
	}

	//--------------------------------------------------------------

	eFrustumCollision cFrustum::CollideBoundingVolume(cBoundingVolume *a_BV)
	{
		if (CollideFrustumSphere(a_BV) == eFrustumCollision_Outside)
			return eFrustumCollision_Outside;

		eFrustumCollision ret = CollideBVSphere(a_BV);

		if (ret == eFrustumCollision_Intersect)
			return CollideBVAABB(a_BV);

		return ret;
	}

	//--------------------------------------------------------------

	eFrustumCollision cFrustum::CollideBVSphere(cBoundingVolume *a_BV)
	{
		int lPlanes = 6;
		if (m_bInfFarPlane) lPlanes = 5;

		for (int i=0; i<lPlanes; i++)
		{
			float fDist = cMath::PlaneToPointDist(m_Plane[i],a_BV->GetWorldCenter());

			if (fDist < -a_BV->GetRadius())
				return eFrustumCollision_Outside;

			if (std::abs(fDist) < a_BV->GetRadius())
				return eFrustumCollision_Intersect;
		}

		return eFrustumCollision_Inside;
	}

	//--------------------------------------------------------------

	eFrustumCollision cFrustum::CollideBVAABB(cBoundingVolume *a_BV)
	{
		cVector3f vMax = a_BV->GetMax();
		cVector3f vMin = a_BV->GetMin();

		cVector3f vCorners[9] = 
		{
			cVector3f(vMax.x, vMax.y, vMax.z),
			cVector3f(vMax.x, vMax.y, vMin.z),
			cVector3f(vMax.x, vMin.y, vMax.z),
			cVector3f(vMax.x, vMin.y, vMin.z),

			cVector3f(vMin.x, vMax.y, vMax.z),
			cVector3f(vMin.x, vMax.y, vMin.z),
			cVector3f(vMin.x, vMin.y, vMax.z),
			cVector3f(vMin.x, vMin.y, vMin.z),

			a_BV->GetPosition()
		};

		int lTotalIn = 0;

		int lPlanes = 6;
		if (m_bInfFarPlane) lPlanes = 5;

		for (int i=0; i<lPlanes; i++)
		{
			int lInCount = 9;
			bool bIsIn = true;

			for (int j=0; j<9; j++)
			{
				float fDist = cMath::PlaneToPointDist(m_Plane[i], vCorners[j]);
				if (fDist < 0)
				{
					lInCount--;
					bIsIn = false;
				}
			}

			if (lInCount == 0) return eFrustumCollision_Outside;
			if (bIsIn) lTotalIn++;
		}

		if (lTotalIn == lPlanes) return eFrustumCollision_Inside;

		return eFrustumCollision_Intersect;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cFrustum::UpdatePlanes()
	{
		m_Plane[eFrustumPlane_Left] = cPlanef(	m_mtxViewProj.m[3][0] + m_mtxViewProj.m[0][0],
												m_mtxViewProj.m[3][1] + m_mtxViewProj.m[0][1],
												m_mtxViewProj.m[3][2] + m_mtxViewProj.m[0][2],
												m_mtxViewProj.m[3][3] + m_mtxViewProj.m[0][3]);

		m_Plane[eFrustumPlane_Right] = cPlanef(	m_mtxViewProj.m[3][0] - m_mtxViewProj.m[0][0],
												m_mtxViewProj.m[3][1] - m_mtxViewProj.m[0][1],
												m_mtxViewProj.m[3][2] - m_mtxViewProj.m[0][2],
												m_mtxViewProj.m[3][3] - m_mtxViewProj.m[0][3]);

		m_Plane[eFrustumPlane_Bottom] = cPlanef(m_mtxViewProj.m[3][0] + m_mtxViewProj.m[1][0],
												m_mtxViewProj.m[3][1] + m_mtxViewProj.m[1][1],
												m_mtxViewProj.m[3][2] + m_mtxViewProj.m[1][2],
												m_mtxViewProj.m[3][3] + m_mtxViewProj.m[1][3]);

		m_Plane[eFrustumPlane_Top] = cPlanef(	m_mtxViewProj.m[3][0] - m_mtxViewProj.m[1][0],
												m_mtxViewProj.m[3][1] - m_mtxViewProj.m[1][1],
												m_mtxViewProj.m[3][2] - m_mtxViewProj.m[1][2],
												m_mtxViewProj.m[3][3] - m_mtxViewProj.m[1][3]);

		m_Plane[eFrustumPlane_Near] = cPlanef(	m_mtxViewProj.m[3][0] + m_mtxViewProj.m[2][0],
												m_mtxViewProj.m[3][1] + m_mtxViewProj.m[2][1],
												m_mtxViewProj.m[3][2] + m_mtxViewProj.m[2][2],
												m_mtxViewProj.m[3][3] + m_mtxViewProj.m[2][3]);

		m_Plane[eFrustumPlane_Far] = cPlanef(	m_mtxViewProj.m[3][0] - m_mtxViewProj.m[2][0],
												m_mtxViewProj.m[3][1] - m_mtxViewProj.m[2][1],
												m_mtxViewProj.m[3][2] - m_mtxViewProj.m[2][2],
												m_mtxViewProj.m[3][3] - m_mtxViewProj.m[2][3]);

		for (int i=0;i<6;i++)
		{
			m_Plane[i].Normalize();
			m_Plane[i].CalcNormal();
		}
	}

	//--------------------------------------------------------------

	void cFrustum::UpdateSphere()
	{
		float fViewLen = m_fFarPlane - m_fNearPlane;

		float fHeight = fViewLen * tan(m_fFOV * 0.5f);
		float fWidth = fHeight * m_fAspect;

		cVector3f P(0.0f, 0.0f, m_fNearPlane + fViewLen * 0.5f);

		cVector3f Q(fWidth, fHeight, fViewLen);

		cVector3f vDiff = P - Q;

		float fRadius = vDiff.Length();

		cVector3f vLookVector = m_mtxModelView.GetForward() * -1;

		cVector3f vCenter = (m_vOrigin) + (vLookVector * (fViewLen * 0.5f + m_fNearPlane));

		m_BoundingSphere = cSpheref(vCenter, fRadius);
	}

	//--------------------------------------------------------------

	void cFrustum::UpdateEndPoints()
	{
		float fXAngle = m_fFOV*0.5f;
		float fYAngle = m_fFOV*0.5f*m_fAspect;

		cVector3f vForward = m_mtxModelView.GetForward();
		cVector3f vUp = m_mtxModelView.GetUp();
		cVector3f vRight = m_mtxModelView.GetRight();

		cVector3f vUpDir = cMath::MatrixMul(cMath::MatrixQuaternion(cQuaternion(fXAngle, vRight)), vForward);
		cVector3f vDownDir = cMath::MatrixMul(cMath::MatrixQuaternion(cQuaternion(-fXAngle, vRight)), vForward);

		vForward = vForward * -1;

		float fRightAdd = sin(fYAngle);

		cVector3f vVec0 = vUpDir + vRight * fRightAdd;
		cVector3f vVec2 = vDownDir + vRight * fRightAdd;
		cVector3f vVec3 = vDownDir + vRight * -fRightAdd;
		cVector3f vVec1 = vUpDir + vRight * -fRightAdd;

		float fAngle0 = cMath::Vector3Angle(vVec0, vForward);
		float fAngle1 = cMath::Vector3Angle(vVec1, vForward);
		float fAngle2 = cMath::Vector3Angle(vVec2, vForward);
		float fAngle3 = cMath::Vector3Angle(vVec3, vForward);

		m_vEndPoints[0] = m_vOrigin + vVec0 * (m_fFarPlane/cos(fAngle0));
		m_vEndPoints[1] = m_vOrigin + vVec1 * (m_fFarPlane/cos(fAngle1));
		m_vEndPoints[2] = m_vOrigin + vVec2 * (m_fFarPlane/cos(fAngle2));
		m_vEndPoints[3] = m_vOrigin + vVec3 * (m_fFarPlane/cos(fAngle3));
	}

	//--------------------------------------------------------------

	void cFrustum::UpdateBV()
	{
		cVector3f vMin = m_vOrigin;
		cVector3f vMax = m_vOrigin;

		for (int i=0;i<4;i++)
		{
			if (vMax.x < m_vEndPoints[i].x) vMax.x = m_vEndPoints[i].x;
			else if(vMin.x > m_vEndPoints[i].x) vMin.x = m_vEndPoints[i].x;

			if (vMax.y < m_vEndPoints[i].y) vMax.y = m_vEndPoints[i].y;
			else if(vMin.y > m_vEndPoints[i].y) vMin.y = m_vEndPoints[i].y;

			if (vMax.z < m_vEndPoints[i].z) vMax.z = m_vEndPoints[i].z;
			else if(vMin.z > m_vEndPoints[i].z) vMin.z = m_vEndPoints[i].z;
		}

		m_BoundingVolume.SetLocalMinMax(vMin, vMax);
	}

	//--------------------------------------------------------------

	eFrustumCollision cFrustum::CollideFrustumSphere(cBoundingVolume *a_BV)
	{
		float fRaduisSum = m_BoundingSphere.r + a_BV->GetRadius();
		cVector3f vSepAxis = m_BoundingSphere.center - a_BV->GetWorldCenter();
		if (vSepAxis.SqrLegnth() > (fRaduisSum * fRaduisSum))
			return eFrustumCollision_Outside;

		return eFrustumCollision_Intersect;
	}

	//--------------------------------------------------------------

	const cVector3f &cFrustum::GetOrigin()
	{
		return m_vOrigin;
	}

	//--------------------------------------------------------------

	cBoundingVolume *cFrustum::GetOriginBV()
	{
		return &m_OriginBV;
	}

	//--------------------------------------------------------------

	cVector3f cFrustum::GetForward()
	{
		return m_mtxModelView.GetForward();
	}

	//--------------------------------------------------------------

	void cFrustum::Draw(iLowLevelGraphics *a_pLowLevelGraphics)
	{
		a_pLowLevelGraphics->DrawLine(m_vOrigin, m_vEndPoints[0], cColor(1,1,1,1));
		a_pLowLevelGraphics->DrawLine(m_vOrigin, m_vEndPoints[1], cColor(1,1,1,1));
		a_pLowLevelGraphics->DrawLine(m_vOrigin, m_vEndPoints[2], cColor(1,1,1,1));
		a_pLowLevelGraphics->DrawLine(m_vOrigin, m_vEndPoints[3], cColor(1,1,1,1));

		a_pLowLevelGraphics->DrawLine(m_vEndPoints[0], m_vEndPoints[1], cColor(1,0,0,1));
		a_pLowLevelGraphics->DrawLine(m_vEndPoints[1], m_vEndPoints[2], cColor(1,0,0,1));
		a_pLowLevelGraphics->DrawLine(m_vEndPoints[2], m_vEndPoints[3], cColor(1,0,0,1));
		a_pLowLevelGraphics->DrawLine(m_vEndPoints[3], m_vEndPoints[0], cColor(1,0,0,1));

		a_pLowLevelGraphics->DrawLine(m_vEndPoints[0], m_vEndPoints[2], cColor(0,1,0,1));
		a_pLowLevelGraphics->DrawLine(m_vEndPoints[1], m_vEndPoints[3], cColor(0,1,0,1));
	}
}