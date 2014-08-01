#ifndef EFE_FRUSTUM_H
#define EFE_FRUSTUM_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "math/BoundingVolume.h"

namespace efe
{
	class iLowLevelGraphics;

	enum eFrustumPlane
	{
		eFrustumPlane_Left=		0,
		eFrustumPlane_Right=	1,
		eFrustumPlane_Bottom=	2,
		eFrustumPlane_Top=		3,
		eFrustumPlane_Near=		4,
		eFrustumPlane_Far=		5,
		eFrustumPlane_LastEnum=	7
	};

	enum eFrustumCollision
	{
		eFrustumCollision_Inside,
		eFrustumCollision_Outside,
		eFrustumCollision_Intersect,
		eFrustumCollision_LastEnum
	};

	class cFrustum
	{
	public:
		cFrustum();

		void SetViewProjMatrix(const cMatrixf &a_mtxProj, const cMatrixf &a_mtxView,
			float a_fFarPlane, float a_fNearPlane, float a_fFOV, float a_fAspect,
			const cVector3f &a_vOrigin, bool a_bInfFarPlane = false);

		cPlanef GetPlane(eFrustumPlane a_Type);

		eFrustumCollision CollideFrustumSphere(cBoundingVolume *a_BV);

		eFrustumCollision CollideBoundingVolume(cBoundingVolume *a_BV);

		eFrustumCollision CollideBVSphere(cBoundingVolume *a_BV);
		eFrustumCollision CollideBVAABB(cBoundingVolume *a_BV);

		const cBoundingVolume &GetBoundingVolume(){return m_BoundingVolume;}

		const cVector3f &GetOrigin();
		cBoundingVolume *GetOriginBV();

		cVector3f GetForward();

		void Draw(iLowLevelGraphics *a_pLowLevelGraphics);

	private:
		void UpdatePlanes();
		void UpdateSphere();
		void UpdateEndPoints();
		void UpdateBV();

		float m_fFarPlane;
		float m_fNearPlane;
		float m_fAspect;
		float m_fFOV;

		bool m_bInfFarPlane;

		cVector3f m_vOrigin;
		cBoundingVolume m_OriginBV;

		cMatrixf m_mtxViewProj;
		cMatrixf m_mtxModelView;
		cPlanef m_Plane[6];
		cSpheref m_BoundingSphere;
		cBoundingVolume m_BoundingVolume;

		cVector3f m_vEndPoints[4];
		tVector3fVec m_vPoints;
	};
};
#endif