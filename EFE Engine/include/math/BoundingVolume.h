#ifndef EFE_BOUNDING_VOLUME_H
#define EFE_BOUNDING_VOLUME_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "system/SerializeClass.h"

namespace efe
{
	class iLowLevelGraphics;
	class cBoundingVolume;

	class cBVTempArray
	{
	public:
		const float *m_pArray;
		int m_lSize;
	};

	enum eBVCollision
	{
		eBVCollision_Inside,
		eBVCollision_Outside,
		eBVCollision_Intersect,
		eBVCollision_LastEnum
	};

	typedef std::list<cBVTempArray> tBVTempArrayList;
	typedef tBVTempArrayList::iterator tBVTempArrayListIt;

	class cShadowVolumeBV
	{
	public:
		cPlanef m_vPlanes[12];
		int m_lPlaneCount;
		tVector3fVec m_vPoints;
		int m_lCapPlanes;

		bool CollideBoundingVolume(cBoundingVolume *a_BV);
	private:
		bool CollideBVSphere(cBoundingVolume *a_BV);
		bool CollideBVAABB(cBoundingVolume *a_BV);
	};

	class cBoundingVolume : public iSerializable
	{
		friend class cMath;
		kSerializableClassInit(cBoundingVolume)
	public:
		cBoundingVolume();

		cVector3f GetMax();
		cVector3f GetMin();

		cVector3f GetLocalMax();
		cVector3f GetLocalMin();

		cVector3f m_vLocalMax;
		cVector3f m_vLocalMin;

		void SetLocalMinMax(const cVector3f &m_vMin, const cVector3f &m_vMax);

		void SetPosition(const cVector3f &a_vPos);
		cVector3f GetPosition();

		void SetSize(const cVector3f &a_vSize);
		cVector3f GetSize();

		void SetTransform(const cMatrixf &a_mtxTransform);
		const cMatrixf &GetTransform();

		cVector3f GetLocalCenter();
		cVector3f GetWorldCenter();

		float GetRadius();

		void AddArrayPoints(const float *a_pArray, int a_lNumOfVectors);
		void CreateFromPoints(int a_lStride);

		cShadowVolumeBV *GetShadowVolume(const cVector3f &a_vLightPos, float a_fLightRange, bool a_bForceUpdate);

		//Debug:
		//void DrawLines()

		void UpdateSize();

		cMatrixf m_mtxTransform;

		cVector3f m_vMax;
		cVector3f m_vMin;

		cVector3f m_vPivot;

		cVector3f m_vWorldMax;
		cVector3f m_vWorldMin;

		cVector3f m_vPosition;
		cVector3f m_vSize;
		float m_fRadius;
	private:
		bool m_bPositionUpdated;
		bool m_bSizeUpdated;

		tBVTempArrayList m_lstArrays;

		cShadowVolumeBV m_ShadowVolume;
		bool m_bShadowPlanesNeedUpdate;
	};
};
#endif