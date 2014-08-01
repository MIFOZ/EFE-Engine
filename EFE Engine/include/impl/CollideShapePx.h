#ifndef EFE_COLLIDE_SHAPE_PX_H
#define EFE_COLLIDE_SHAPE_PX_H

#include <PxPhysicsAPI.h>
#include "physics/CollideShape.h"

#include "math/MathTypes.h"

namespace efe
{
	using namespace physx;
	class iVertexBuffer;
	//class iCollideShape;

	typedef std::vector<iCollideShape*> tCollideShapeVec;
	typedef tCollideShapeVec::iterator tCollideShapeVecIt;

	class cPxWriteBuffer : public PxStream
	{
	public:
		cPxWriteBuffer();
		virtual ~cPxWriteBuffer();

		PxU8 *m_pData;
		mutable const PxU8 *m_pBuffer;

		static const int BUFFER_RESIZE_STEP = 4096;

		virtual PxU8 readByte()const;
		virtual PxU16 readWord()const;
		virtual PxU32 readDword()const;
		virtual float readFloat()const;
		virtual double readDouble()const;
		virtual void readBuffer(void* buffer, PxU32 size)const;

		virtual PxStream &storeByte(PxU8 b);
		virtual PxStream &storeWord(PxU16 w);
		virtual PxStream &storeDword(PxU32 d);
		virtual PxStream &storeFloat(PxReal f);
		virtual PxStream &storeDouble(PxF64 f);
		virtual PxStream &storeBuffer(const void* buffer, PxU32 size);

	private:
		PxU32 m_lCurrentSize;
		PxU32 m_lMaxSize;
	};

	class cCollideShapePx : public iCollideShape
	{
	public:
		cCollideShapePx(eCollideShapeType a_Type, const cVector3f &a_vSize,
			cMatrixf *a_pOffsetMtx, PxScene *a_pPxScene,
			iPhysicsWorld *a_pWorld);
		~cCollideShapePx();

		iCollideShape *GetSubShape(int a_lIdx);
		int GetSubShapeNum();

		cVector3f GetInertia(float a_fMass);

		void CreateFromShapeVec(tCollideShapeVec &a_vShapes);
		void CreateFromVertices(const unsigned int *a_pIndexArray, int a_lIndexNum,
			const float *a_pVertexArray, int a_lVtxStride, int a_lVtxNum);

		PxGeometry *GetPxGeometry(){return m_pPxGeometry;}

	private:
		//PxShape *m_pPxShape;
		PxScene *m_pPxScene;
		PxGeometry *m_pPxGeometry;

		tCollideShapeVec m_vSubShapes;
	};	
};
#endif