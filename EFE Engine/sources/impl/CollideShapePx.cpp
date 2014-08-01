#include "impl/CollideShapePx.h"

#include "impl/PhysicsWorldPx.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// WRITE BUFFER
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPxWriteBuffer::cPxWriteBuffer()
	{
		m_lCurrentSize = 0;
		m_lMaxSize = 0;
		m_pData = NULL;
	}

	//--------------------------------------------------------------

	cPxWriteBuffer::~cPxWriteBuffer()
	{
		efeDelete(m_pData);
	}

	//--------------------------------------------------------------

	PxU8 cPxWriteBuffer::readByte()const
	{
		PxU8 b;
		memcpy(&b, m_pBuffer, sizeof(PxU8));
        m_pBuffer += sizeof(PxU8);
        return b; 
	}
	PxU16 cPxWriteBuffer::readWord()const
	{
		PxU16 w;
		memcpy(&w, m_pBuffer, sizeof(PxU16));
        m_pBuffer += sizeof(PxU16);
        return w;
	}
	PxU32 cPxWriteBuffer::readDword()const
	{
		PxU32 d;
		memcpy(&d, m_pBuffer, sizeof(PxU32));
		m_pBuffer += sizeof(PxU32);
		return d;
	}
	float cPxWriteBuffer::readFloat()const
	{
		float f;
		memcpy(&f, m_pBuffer, sizeof(float));
		m_pBuffer += sizeof(float);
		return f;
	}
	double cPxWriteBuffer::readDouble()const
	{
		double f;
		memcpy(&f, m_pBuffer, sizeof(double));
		m_pBuffer += sizeof(double);
		return f;
	}
	void cPxWriteBuffer::readBuffer(void* buffer, PxU32 size)const
	{
		memcpy(buffer, m_pBuffer, size);
        m_pBuffer += size;
	}

	//--------------------------------------------------------------
		
	PxStream &cPxWriteBuffer::storeByte(PxU8 b)
	{
		storeBuffer(&b, sizeof(PxU8));
		return *this;
	}
	PxStream &cPxWriteBuffer::storeWord(PxU16 w)
	{
		storeBuffer(&w, sizeof(PxU16));
		return *this;
	}
	PxStream &cPxWriteBuffer::storeDword(PxU32 d)
	{
		storeBuffer(&d, sizeof(PxU16));
		return *this;
	}
	PxStream &cPxWriteBuffer::storeFloat(PxReal f)
	{
		storeBuffer(&f, sizeof(PxReal));
		return *this;
	}
	PxStream &cPxWriteBuffer::storeDouble(PxF64 f)
	{
		storeBuffer(&f, sizeof(PxF64));
		return *this;
	}
	PxStream &cPxWriteBuffer::storeBuffer(const void* buffer, PxU32 size)
	{
		unsigned int newSize = m_lCurrentSize + size;
		if(newSize > m_lMaxSize)
		{       //resize
			m_lMaxSize = newSize + BUFFER_RESIZE_STEP;

			PxU8* newData = new PxU8[m_lMaxSize];
			PX_ASSERT(newData != nullptr);

			if(m_pData)
			{
				memcpy(newData, m_pData, m_lCurrentSize);
				delete[] m_pData;
			}
			m_pData = newData;
		}

		memcpy(m_pData + m_lCurrentSize, buffer, size);
		m_lCurrentSize += size;

		m_pBuffer = m_pData;
		return *this;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cCollideShapePx::cCollideShapePx(eCollideShapeType a_Type, const cVector3f &a_vSize,
			cMatrixf *a_pOffsetMtx, PxScene *a_pPxScene,
			iPhysicsWorld *a_pWorld)
			: iCollideShape(a_pWorld)
	{
		m_pPxGeometry = NULL;
		m_pPxScene = NULL;
		m_vSize = a_vSize;
		m_Type = a_Type;

		m_fVolume = 0;

		float *pMtx = NULL;
		cMatrixf mtxTranspose;
		if (a_pOffsetMtx)
		{
			m_mtxOffset = *a_pOffsetMtx;
			/*mtxTranspose = m_mtxOffset.GetTranspose();

			pMtx = &(mtxTranspose.m[0][0]);*/
		}
		else
			m_mtxOffset = cMatrixf::Identity;

		switch (a_Type)
		{
		//case efe::eCollideShapeType_Null:			m_pPxShape = m_pPxScene->
		//	break;
		case efe::eCollideShapeType_Box:			m_pPxGeometry = efeNew(PxBoxGeometry, (m_vSize.x/2, m_vSize.y/2, m_vSize.z/2));
			break;
		case efe::eCollideShapeType_Sphere:			m_pPxGeometry = efeNew(PxSphereGeometry, (m_vSize.x));
			break;
		case efe::eCollideShapeType_Capsule:		m_pPxGeometry = efeNew(PxCapsuleGeometry, (m_vSize.x, m_vSize.y));
			break;
		}

		if (m_Type == eCollideShapeType_Box)
		{
			m_BoundingVolume.SetSize(m_vSize);

			m_fVolume = m_vSize.x * m_vSize.y * m_vSize.z;
		}
		else if (m_Type == eCollideShapeType_Sphere)
		{
			m_BoundingVolume.SetSize(m_vSize*2);

			m_fVolume = (4.0f / 3.0f) * kPif * (m_vSize.x * m_vSize.x * m_vSize.x);
		}
		else if (m_Type == eCollideShapeType_Capsule)
		{
			m_BoundingVolume.SetSize(cVector3f(0.5 * m_vSize.y,m_vSize.x, m_vSize.x));

			float fHeight = m_vSize.y * 2;
			m_fVolume = 0;

			if (fHeight>0)
				m_fVolume += k2Pif * m_vSize.x * m_vSize.x * fHeight;

			m_fVolume += (4.0f / 3.0f) * kPif * (m_vSize.x * m_vSize.x * m_vSize.x);
		}

		m_BoundingVolume.SetTransform(m_mtxOffset);
	}

	//-------------------------------------------------------------

	cCollideShapePx::~cCollideShapePx()
	{
		/*if (m_pPxShape)
			m_pPxShape->release();*/

		for (int i=0; i<(int)m_vSubShapes.size(); i++)
			m_pWorld->DestroyShape(m_vSubShapes[i]);
	}

	//-------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	void cCollideShapePx::CreateFromVertices(const unsigned int *a_pIndexArray, int a_lIndexNum,
			const float *a_pVertexArray, int a_lVtxStride, int a_lVtxNum)
	{
		float vTriVec[9];

		bool bOptimize = false;
		bool bCreatedPlane = false;
		cPlanef plane;

		PxTriangleMeshDesc desc;

		desc.points.count = a_lVtxNum;
		desc.triangles.count = a_lIndexNum/3;
		desc.points.stride = 4*4;
		desc.triangles.stride = 4*3;
		desc.points.data = a_pVertexArray;
		desc.triangles.data = a_pIndexArray;

		cPhysicsWorldPx *pPxWorld = static_cast<cPhysicsWorldPx*>(m_pWorld);
		PxCooking *pCooking = PxCreateCooking(PX_PHYSICS_VERSION, &pPxWorld->GetPxSDK()->getFoundation(), PxCookingParams());
		
		cPxWriteBuffer wt;
		if (pCooking->cookTriangleMesh(desc, wt)==false)
		{
			Error("Couldn't cook mesh!\n");
			pCooking->release();
			return;
		};

		PxTriangleMesh *pColMesh = pPxWorld->GetPxSDK()->createTriangleMesh(wt);
		pCooking->release();
		
		m_pPxGeometry = efeNew(PxTriangleMeshGeometry, (pColMesh));
	}
}