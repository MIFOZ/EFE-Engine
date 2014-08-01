#ifndef EFE_COLLIDE_SHAPE_H
#define EFE_COLLIDE_SHAPE_H

#include "math/MathTypes.h"
#include "math/BoundingVolume.h"

namespace efe
{
	enum eCollideShapeType
	{
		eCollideShapeType_Null,
		eCollideShapeType_Box,
		eCollideShapeType_Sphere,
		eCollideShapeType_Cylinder,
		eCollideShapeType_Capsule,
		eCollideShapeType_ConvexHull,
		eCollideShapeType_Mesh,
		eCollideShapeType_Compound,
		eCollideShapeType_Heightmap,
		eCollideShapeType_LastEnum
	};

	class iPhysicsWorld;

	class iCollideShape
	{
	public:
		iCollideShape(iPhysicsWorld *a_pWorld) : m_lUserCount(0), m_pWorld(a_pWorld){}
		virtual ~iCollideShape(){}

		cVector3f GetSize(){return m_vSize;}

		float GetRadius(){return m_vSize.x;}
		float GetHeight(){return m_vSize.y;}
		float GetWidth(){return m_vSize.z;}
		float GetDepth(){return m_vSize.x;}

		const cMatrixf &GetOffset(){return m_mtxOffset;}

		eCollideShapeType GetType(){return m_Type;}

		void IncUserCount(){m_lUserCount++;}
		void DecUserCount(){m_lUserCount--;}

		bool HasUsers(){return m_lUserCount>0;}
		int GetUserCount(){return m_lUserCount;}

		float GetVolume(){return m_fVolume;}

		cBoundingVolume &GetBoundingVolume(){return m_BoundingVolume;}
	protected:
		cVector3f m_vSize;
		eCollideShapeType m_Type;
		cMatrixf m_mtxOffset;

		int m_lUserCount;

		iPhysicsWorld *m_pWorld;
		float m_fVolume;

		cBoundingVolume m_BoundingVolume;
	};
};
#endif