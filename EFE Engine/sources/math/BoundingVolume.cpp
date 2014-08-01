#include "math/BoundingVolume.h"

#include "math/Math.h"
#include "graphics/LowLevelGraphics.h"

namespace efe
{
	static const cTriEdge kvBVEdges[12] =
	{
		cTriEdge(1,0, 0,2),
		cTriEdge(3,1, 0,5),
		cTriEdge(2,3, 0,3),
		cTriEdge(0,2, 0,4),

		cTriEdge(0,4, 4,2),
		cTriEdge(4,6, 4,1),
		cTriEdge(6,2, 4,3),
		cTriEdge(4,5, 1,2),

		cTriEdge(5,7, 1,5),
		cTriEdge(7,6, 1,3),
		cTriEdge(1,5, 2,5),
		cTriEdge(3,7, 5,3)
	};

	static cTriangleData g_vFaces[6] =
	{
		cVector3f(1,0,0),
		cVector3f(-1,0,0),

		cVector3f(0,1,0),
		cVector3f(0,-1,0),

		cVector3f(0,0,1),
		cVector3f(0,0,-1)
	};

	static const int kvFacePoints[6] = {0,5,5,6,4,7};

	bool cShadowVolumeBV::CollideBoundingVolume(cBoundingVolume *a_BV)
	{
		if (CollideBVSphere(a_BV)==false) return false;

		return CollideBVAABB(a_BV);
	}

	bool cShadowVolumeBV::CollideBVSphere(cBoundingVolume *a_BV)
	{
		for (int i=0; i<m_lPlaneCount;i++)
		{
			float fDist = cMath::PlaneToPointDist(m_vPlanes[i], a_BV->GetWorldCenter());

			if (fDist < -a_BV->GetRadius())
				return false;
		}
		return true;
	}

	bool cShadowVolumeBV::CollideBVAABB(cBoundingVolume *a_BV)
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

		for (int i=0; i<m_lPlaneCount; i++)
		{
			int lInCount = 0;
		}
		return true;
	}

	cBoundingVolume::cBoundingVolume()
	{
		m_mtxTransform = cMatrixf::Identity;

		m_vLocalMax = 0;
		m_vLocalMin = 0;

		m_vPosition = 0;
		m_vPivot = 0;
		m_vSize = 0;
		m_fRadius = 0;

		m_bPositionUpdated = true;
		m_bSizeUpdated = true;

		m_ShadowVolume.m_vPoints.reserve(8*4);
		m_bShadowPlanesNeedUpdate = true;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cVector3f cBoundingVolume::GetMax()
	{
		UpdateSize();
		return m_vWorldMax;
	}

	cVector3f cBoundingVolume::GetMin()
	{
		UpdateSize();
		return m_vWorldMin;
	}

	//--------------------------------------------------------------

	cVector3f cBoundingVolume::GetLocalMax()
	{
		return m_vLocalMax;
	}

	cVector3f cBoundingVolume::GetLocalMin()
	{
		return m_vLocalMin;
	}

	//--------------------------------------------------------------

	void cBoundingVolume::SetLocalMinMax(const cVector3f &m_vMin, const cVector3f &m_vMax)
	{
		m_vLocalMin = m_vMin;
		m_vLocalMax = m_vMax;

		m_bSizeUpdated = true;
	}

	cVector3f cBoundingVolume::GetLocalCenter()
	{
		return m_vPivot;
	}

	cVector3f cBoundingVolume::GetWorldCenter()
	{
		UpdateSize();
	
		return m_mtxTransform.GetTranslation() + m_vPivot;
	}

	void cBoundingVolume::SetPosition(const cVector3f &a_vPos)
	{
		m_bPositionUpdated = true;
	}

	cVector3f cBoundingVolume::GetPosition()
	{
		return m_vPosition;
	}

	//--------------------------------------------------------------

	void cBoundingVolume::SetTransform(const cMatrixf &a_mtxTransform)
	{
		m_mtxTransform = a_mtxTransform;

		m_bSizeUpdated = true;
	}

	const cMatrixf &cBoundingVolume::GetTransform()
	{
		return m_mtxTransform;
	}

	//--------------------------------------------------------------

	void cBoundingVolume::SetSize(const cVector3f &a_vSize)
	{
		m_vLocalMax = a_vSize * 0.5f;
		m_vLocalMin = a_vSize * -0.5f;

		m_bSizeUpdated = true;
	}

	//--------------------------------------------------------------
	
	cVector3f cBoundingVolume::GetSize()
	{
		UpdateSize();
		
		return m_vSize;
	}

	//--------------------------------------------------------------

	float cBoundingVolume::GetRadius()
	{
		UpdateSize();
		return m_fRadius;
	}

	//--------------------------------------------------------------

	void cBoundingVolume::AddArrayPoints(const float *a_pArray, int a_lNumOfVectors)
	{
		cBVTempArray temp;
		temp.m_pArray = a_pArray;
		temp.m_lSize = a_lNumOfVectors;

		m_lstArrays.push_back(temp);
	}

	//--------------------------------------------------------------

	void cBoundingVolume::CreateFromPoints(int a_lStride)
	{
		m_vLocalMax = cVector3f(-10000, -10000, -10000);
		m_vLocalMin = cVector3f(10000, 10000, 10000);

		for (tBVTempArrayListIt it = m_lstArrays.begin(); it != m_lstArrays.end(); it++)
		{
			const float *a_pVec = it->m_pArray;
			int lNumOfVectors = it->m_lSize;
			while (lNumOfVectors)
			{
				if (a_pVec[0] < m_vLocalMin.x) m_vLocalMin.x = a_pVec[0];
				if (a_pVec[0] > m_vLocalMax.x) m_vLocalMax.x = a_pVec[0];

				if (a_pVec[1] < m_vLocalMin.y) m_vLocalMin.y = a_pVec[1];
				if (a_pVec[1] > m_vLocalMax.y) m_vLocalMax.y = a_pVec[1];

				if (a_pVec[2] < m_vLocalMin.z) m_vLocalMin.z = a_pVec[2];
				if (a_pVec[2] > m_vLocalMax.z) m_vLocalMax.z = a_pVec[2];

				a_pVec += a_lStride;
				lNumOfVectors--;
			}
		}
		m_lstArrays.clear();

		m_bPositionUpdated = true;
		m_bSizeUpdated = true;
	}

	//--------------------------------------------------------------

	cShadowVolumeBV *cBoundingVolume::GetShadowVolume(const cVector3f &a_vLightPos, float a_fLightRange, bool a_bForceUpdate)
	{
		if (cMath::PointBVCollision(a_vLightPos,*this)) return NULL;

		if (!a_bForceUpdate && !m_bShadowPlanesNeedUpdate) return &m_ShadowVolume;

		m_ShadowVolume.m_vPoints.resize(0);

		cVector3f vMax = GetMax();
		cVector3f vMin = GetMin();
		cVector3f vCorners[8];
		vCorners[0] = cVector3f(vMax.x,vMax.y,vMax.z);
		vCorners[1] = cVector3f(vMax.x,vMax.y,vMin.z);
		vCorners[2] = cVector3f(vMax.x,vMin.y,vMax.z);
		vCorners[3] = cVector3f(vMax.x,vMin.y,vMin.z);

		vCorners[4] = cVector3f(vMin.x,vMax.y,vMax.z);
		vCorners[5] = cVector3f(vMin.x,vMax.y,vMin.z);
		vCorners[6] = cVector3f(vMin.x,vMin.y,vMax.z);
		vCorners[7] = cVector3f(vMin.x,vMin.y,vMin.z);

		int lNearPoint = -1;
		m_ShadowVolume.m_lPlaneCount=0;
		for (int face=0;face<6;face++)
		{
			g_vFaces[face].facingLight = cMath::Vector3Dot(g_vFaces[face].normal,
														vCorners[kvFacePoints[face]] - a_vLightPos)<0;

			if (g_vFaces[face].facingLight)
			{
				m_ShadowVolume.m_vPlanes[m_ShadowVolume.m_lPlaneCount] = cPlanef(
								g_vFaces[face].normal*-1.0f,vCorners[kvFacePoints[face]]);
				m_ShadowVolume.m_lPlaneCount++;
			}
		}

		m_ShadowVolume.m_lCapPlanes = m_ShadowVolume.m_lPlaneCount;

		cVector3f vDir;

		float fPushLength = a_fLightRange*kSqrt2f;

		for (int edge=0;edge<12;edge++)
		{
			const cTriEdge &Edge = kvBVEdges[edge];

			cTriangleData &Face1 = g_vFaces[Edge.tri1];
			cTriangleData &Face2 = g_vFaces[Edge.tri2];

			if ((Face1.facingLight && !Face2.facingLight) || (Face2.facingLight && !Face1.facingLight))
			{
				if (Face1.facingLight)
				{
					m_ShadowVolume.m_vPoints.push_back(vCorners[Edge.point1]);
					m_ShadowVolume.m_vPoints.push_back(vCorners[Edge.point2]);

					vDir = (vCorners[Edge.point2]-a_vLightPos); vDir.Normalize();
					m_ShadowVolume.m_vPoints.push_back(vCorners[Edge.point2] + vDir*fPushLength);

					vDir = (vCorners[Edge.point1]-a_vLightPos); vDir.Normalize();
					m_ShadowVolume.m_vPoints.push_back(vCorners[Edge.point1] + vDir*fPushLength);
				}
				else
				{
					m_ShadowVolume.m_vPoints.push_back(vCorners[Edge.point2]);
					m_ShadowVolume.m_vPoints.push_back(vCorners[Edge.point1]);

					vDir = (vCorners[Edge.point1]-a_vLightPos); vDir.Normalize();
					m_ShadowVolume.m_vPoints.push_back(vCorners[Edge.point1]);

					vDir = (vCorners[Edge.point2]-a_vLightPos); vDir.Normalize();
					m_ShadowVolume.m_vPoints.push_back(vCorners[Edge.point2] + vDir*fPushLength);
				}
			}
		}

		for (int i=0;i<(int)m_ShadowVolume.m_vPoints.size();i++)
		{
			cVector3f vNormal = cMath::Vector3Cross(
									m_ShadowVolume.m_vPoints[i+1] - m_ShadowVolume.m_vPoints[i],
									m_ShadowVolume.m_vPoints[i+2] - m_ShadowVolume.m_vPoints[i]);
			m_ShadowVolume.m_vPlanes[m_ShadowVolume.m_lPlaneCount].FromNormalPoint(vNormal,
																m_ShadowVolume.m_vPoints[i]);
			m_ShadowVolume.m_vPlanes[m_ShadowVolume.m_lPlaneCount].Normalize();

			m_ShadowVolume.m_lPlaneCount++;
		}

		return &m_ShadowVolume;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cBoundingVolume::UpdateSize()
	{
		if (m_bSizeUpdated)
		{
			cMatrixf mtxRot = m_mtxTransform.GetRotation();

			cVector3f vCorners[8];
			vCorners[0] = cMath::MatrixMul(mtxRot, cVector3f(m_vLocalMax.x,m_vLocalMax.y,m_vLocalMax.z));
			vCorners[1] = cMath::MatrixMul(mtxRot, cVector3f(m_vLocalMax.x,m_vLocalMax.y,m_vLocalMin.z));
			vCorners[2] = cMath::MatrixMul(mtxRot, cVector3f(m_vLocalMax.x,m_vLocalMin.y,m_vLocalMax.z));
			vCorners[3] = cMath::MatrixMul(mtxRot, cVector3f(m_vLocalMax.x,m_vLocalMin.y,m_vLocalMin.z));

			vCorners[4] = cMath::MatrixMul(mtxRot, cVector3f(m_vLocalMin.x,m_vLocalMax.y,m_vLocalMax.z));
			vCorners[5] = cMath::MatrixMul(mtxRot, cVector3f(m_vLocalMin.x,m_vLocalMax.y,m_vLocalMin.z));
			vCorners[6] = cMath::MatrixMul(mtxRot, cVector3f(m_vLocalMin.x,m_vLocalMin.y,m_vLocalMax.z));
			vCorners[7] = cMath::MatrixMul(mtxRot, cVector3f(m_vLocalMin.x,m_vLocalMin.y,m_vLocalMin.z));

			m_vMax = vCorners[0];
			m_vMin = vCorners[0];

			for (int i=1; i<8; i++)
			{
				if (vCorners[i].x < m_vMin.x) m_vMin.x = vCorners[i].x;
				else if (vCorners[i].x > m_vMax.x) m_vMax.x = vCorners[i].x;

				if (vCorners[i].y < m_vMin.y) m_vMin.y = vCorners[i].y;
				else if (vCorners[i].y > m_vMax.y) m_vMax.y = vCorners[i].y;

				if (vCorners[i].z < m_vMin.z) m_vMin.z = vCorners[i].z;
				else if (vCorners[i].z > m_vMax.z) m_vMax.z = vCorners[i].z;
			}

			m_vSize = m_vMax - m_vMin;

			m_vPivot = m_vMax - (m_vSize*0.5);

			m_fRadius = cMath::Vector3DDist(m_vPivot, m_vMax);

			m_bSizeUpdated = false;
			m_bPositionUpdated = true;
		}

		if (m_bPositionUpdated)
		{
			m_vWorldMax = m_mtxTransform.GetTranslation() + m_vMax;
			m_vWorldMin = m_mtxTransform.GetTranslation() + m_vMin;

			m_bPositionUpdated = false;

			m_bShadowPlanesNeedUpdate = true;
		}
	}
}