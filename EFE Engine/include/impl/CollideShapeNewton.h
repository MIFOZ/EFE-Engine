#ifndef EFE_COLLIDE_SHAPE_NEWTON_H
#define EFE_COLLIDE_SHAPE_NEWTON_H

#include <Newton.h>
#include "physics/CollideShape.h"

#include "math/MathTypes.h"

namespace efe
{
	class iVertexBuffer;
	class iCollideShape;
	class iBitmap2D;

	typedef std::vector<iCollideShape*> tCollideShapeVec;
	typedef tCollideShapeVec::iterator tCollideShapeVecIt;

	class cCollideShapeNewton : public iCollideShape
	{
	public:
		cCollideShapeNewton(eCollideShapeType a_Type, const cVector3f &a_vSize,
			cMatrixf *a_pOffsetMtx, const NewtonWorld *a_pNewtonWorld,
			iPhysicsWorld *a_pWorld);
		~cCollideShapeNewton();

		iCollideShape *GetSubShape(int a_lIdx);
		int GetSubShapeNum();

		cVector3f GetInertia(float a_fMass);

		void CreateFromShapeVec(tCollideShapeVec &a_vShapes);
		void CreateFromVertices(const unsigned int *a_pIndexArray, int a_lIndexNum,
			const float *a_pVertexArray, int a_lVtxStride, int a_lVtxNum);
		void CreateFromBitmap(iBitmap2D *a_pHeightmap);

		const NewtonCollision *GetNewtonCollision(){return m_pNewtonCollision;}

	private:
		const NewtonCollision *m_pNewtonCollision;
		const NewtonWorld *m_pNewtonWorld;

		tCollideShapeVec m_vSubShapes;
	};
};

#endif