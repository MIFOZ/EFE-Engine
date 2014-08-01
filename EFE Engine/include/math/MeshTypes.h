#ifndef EFE_MESH_TYPES_H
#define EFE_MESH_TYPES_H

#include "math/MathTypes.h"
#include "math/BoundingVolume.h"

namespace efe
{
	class cTriangleData
	{
	public:
		cVector3f normal;
		bool facingLight;

		cTriangleData(){}
		cTriangleData(const cVector3f &a_vNormal)
		{
			normal = a_vNormal;
		}
	};

	typedef std::vector<cTriangleData> tTriangleDataVec;
	typedef tTriangleDataVec::iterator tTriangleDataVecIt;

	class cTriEdge
	{
	public:
		int point1, point2;
		mutable int tri1, tri2;
		bool invert_tri2;

		cTriEdge(){}
		cTriEdge(int a_lPoint1, int a_lPoint2, int a_lTri1, int a_lTri2)
		{
			point1 = a_lPoint1;
			point2 = a_lPoint2;
			tri1 = a_lTri1;
			tri2 = a_lTri2;
		}
	};

	typedef std::vector<cTriEdge> tTriEdgeVec;
	typedef tTriEdgeVec::iterator tTriEdgeVecIt;
};
#endif