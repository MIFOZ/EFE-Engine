#include "impl/CollideShapeNewton.h"

#include "impl/PhysicsWorldNewton.h"
#include "system/LowLevelSystem.h"

#include "graphics/Bitmap2D.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cCollideShapeNewton::cCollideShapeNewton(eCollideShapeType a_Type, const cVector3f &a_vSize,
			cMatrixf *a_pOffsetMtx, const NewtonWorld *a_pNewtonWorld,
			iPhysicsWorld *a_pWorld)
			: iCollideShape(a_pWorld)
	{
		m_pNewtonCollision = NULL;
		m_pNewtonWorld = a_pNewtonWorld;
		m_vSize = a_vSize;
		m_Type = a_Type;

		m_fVolume = 0;

		float *pMtx = NULL;
		cMatrixf mtxTranspose;
		if (a_pOffsetMtx)
		{
			m_mtxOffset = *a_pOffsetMtx;
			mtxTranspose = m_mtxOffset.GetTranspose();

			pMtx = &(mtxTranspose.m[0][0]);
		}
		else
			m_mtxOffset = cMatrixf::Identity;

		switch (m_Type)
		{
		case efe::eCollideShapeType_Null:			m_pNewtonCollision = NewtonCreateNull(a_pNewtonWorld); break;

		case efe::eCollideShapeType_Box:			m_pNewtonCollision = NewtonCreateBox(a_pNewtonWorld,
														m_vSize.x, m_vSize.y, m_vSize.z,
														0, pMtx); break;

		case efe::eCollideShapeType_Sphere:			m_pNewtonCollision = NewtonCreateSphere(m_pNewtonWorld,
														m_vSize.x,
														0, pMtx); break;

		case efe::eCollideShapeType_Cylinder:		m_pNewtonCollision = NewtonCreateCylinder(m_pNewtonWorld,
														m_vSize.x, m_vSize.y,
														0, pMtx); break;

		case efe::eCollideShapeType_Capsule:		m_pNewtonCollision = NewtonCreateCapsule(m_pNewtonWorld,
														m_vSize.x, m_vSize.y,
														0, pMtx); break;
		default: break;
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
		else if (m_Type == eCollideShapeType_Cylinder ||
			m_Type == eCollideShapeType_Capsule)
		{
			m_BoundingVolume.SetSize(cVector3f(m_vSize.y,m_vSize.x*2, m_vSize.x*2));

			if (m_Type == eCollideShapeType_Cylinder)
				m_fVolume = kPif * (m_vSize.x*m_vSize.x)*m_vSize.y;
			else
			{
				float fCylHeight = m_vSize.y - m_vSize.x*2;
				m_fVolume = 0;

				if (fCylHeight>0)
					m_fVolume += k2Pif * (m_vSize.x * m_vSize.x) * fCylHeight;

				m_fVolume += (4.0f / 3.0f) * kPif * (m_vSize.x * m_vSize.x * m_vSize.x);
			}
		}

		m_BoundingVolume.SetTransform(m_mtxOffset);
	}

	//-------------------------------------------------------------

	cCollideShapeNewton::~cCollideShapeNewton()
	{
		if (m_pNewtonCollision)
			NewtonDestroyCollision(m_pNewtonCollision);

		for (int i=0; i<(int)m_vSubShapes.size(); i++)
			m_pWorld->DestroyShape(m_vSubShapes[i]);
	}

	//-------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	iCollideShape *cCollideShapeNewton::GetSubShape(int a_lIdx)
	{
		if (m_Type == eCollideShapeType_Compound)
			return m_vSubShapes[a_lIdx];
		else
			return this;
	}

	int cCollideShapeNewton::GetSubShapeNum()
	{
		if (m_Type == eCollideShapeType_Compound)
			return (int)m_vSubShapes.size();
		else
			return 1;
	}

	//-------------------------------------------------------------

	cVector3f cCollideShapeNewton::GetInertia(float a_fMass)
	{
		_ASSERT(0);
		//NewtonBodyGetInertiaMatrix(
		return 0;
	}

	//-------------------------------------------------------------

	void cCollideShapeNewton::CreateFromShapeVec(tCollideShapeVec &a_vShapes)
	{
		m_vSubShapes.reserve(a_vShapes.size());

		m_fVolume = 0;

		m_pNewtonCollision = NewtonCreateCompoundCollision(m_pNewtonWorld, 0);

		NewtonCompoundCollisionBeginAddRemove(const_cast<NewtonCollision*>(m_pNewtonCollision));
		for (size_t i=0; i<a_vShapes.size(); i++)
		{
			m_vSubShapes.push_back(a_vShapes[i]);

			cCollideShapeNewton *pNewtonShape = static_cast<cCollideShapeNewton*>(a_vShapes[i]);
			NewtonCompoundCollisionAddSubCollision(const_cast<NewtonCollision*>(m_pNewtonCollision), 
				const_cast<NewtonCollision*>(pNewtonShape->GetNewtonCollision()));

			m_fVolume += pNewtonShape->GetVolume();
		}
		NewtonCompoundCollisionEndAddRemove(const_cast<NewtonCollision*>(m_pNewtonCollision));
		
		// Create bounding volume
		cVector3f vFinalMax = a_vShapes[0]->GetBoundingVolume().GetMax();
		cVector3f vFinalMin = a_vShapes[0]->GetBoundingVolume().GetMin();

		for (size_t i=1; i<a_vShapes.size(); i++)
		{
			cVector3f vMax = a_vShapes[i]->GetBoundingVolume().GetMax();
			cVector3f vMin = a_vShapes[i]->GetBoundingVolume().GetMin();

			if (vFinalMax.x < vMax.x) vFinalMax.x = vMax.x;
			if (vFinalMin.x > vMin.x) vFinalMin.x = vMin.x;

			if (vFinalMax.y < vMax.y) vFinalMax.y = vMax.y;
			if (vFinalMin.y > vMin.y) vFinalMin.y = vMin.y;

			if (vFinalMax.z < vMax.z) vFinalMax.z = vMax.z;
			if (vFinalMin.z > vMin.z) vFinalMin.z = vMin.z;
		}

		m_BoundingVolume.SetLocalMinMax(vFinalMin, vFinalMax);
	}

	//-------------------------------------------------------------

	void cCollideShapeNewton::CreateFromVertices(const unsigned int *a_pIndexArray, int a_lIndexNum,
			const float *a_pVertexArray, int a_lVtxStride, int a_lVtxNum)
	{
		float vTriVec[9];

		bool bOptimize = false;
		bool bCreatedPlane = false;
		cPlanef plane;

		m_pNewtonCollision = NewtonCreateTreeCollision(m_pNewtonWorld, 0);

		NewtonTreeCollisionBeginBuild(m_pNewtonCollision);
		for (int tri=0; tri<a_lIndexNum; tri+=3)
		{
			for (int idx=0; idx<3; idx++)
			{
				int lVtx = a_pIndexArray[tri + 2-idx]*a_lVtxStride;

				vTriVec[idx*3 + 0] = a_pVertexArray[lVtx + 0];
				vTriVec[idx*3 + 1] = a_pVertexArray[lVtx + 1];
				vTriVec[idx*3 + 2] = a_pVertexArray[lVtx + 2];
			}

			if (bOptimize == false)
			{
				cPlanef tempPlane;
				cVector3f vP1(vTriVec[0+0], vTriVec[0+1], vTriVec[0+2]);
				cVector3f vP2(vTriVec[1*3+0], vTriVec[1*3+1], vTriVec[1*3+2]);
				cVector3f vP3(vTriVec[2*3+0], vTriVec[2*3+1], vTriVec[2*3+2]);

				tempPlane.FromPoints(vP1, vP2, vP3);

				if (bCreatedPlane == false)
				{
					plane = tempPlane;
					bCreatedPlane = true;
				}
				else
				{
					if (std::abs(plane.a - tempPlane.a) > kEpsilonf ||
						std::abs(plane.b - tempPlane.b) > kEpsilonf ||
						std::abs(plane.c - tempPlane.c) > kEpsilonf ||
						std::abs(plane.d - tempPlane.d) > kEpsilonf)
						bOptimize = true;
				}
			}

			NewtonTreeCollisionAddFace(m_pNewtonCollision, 3, vTriVec, sizeof(float)*3, 1);
		}

		NewtonTreeCollisionEndBuild(m_pNewtonCollision, bOptimize ? 1 : 0);
		
		m_BoundingVolume.AddArrayPoints(a_pVertexArray, a_lVtxNum);
		m_BoundingVolume.CreateFromPoints(a_lVtxStride);
	}

	void cCollideShapeNewton::CreateFromBitmap(iBitmap2D *a_pHeightmap)
	{
		//m_pNewtonCollision = NewtonCreateHeightFieldCollision(m_pNewtonWorld, a_pHeightmap->GetWidth(), a_pHeightmap->GetHeight(), 5);
	}
}