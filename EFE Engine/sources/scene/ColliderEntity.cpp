#include "scene/ColliderEntity.h"

#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "physics/CollideShape.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTERS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cColliderEntity::cColliderEntity(const tString &a_sName, iPhysicsBody *a_pBody, iPhysicsWorld *a_pWorld)
		: iRenderable(a_sName)
	{
		m_pPhysicsWorld = a_pWorld;
		m_pBody = a_pBody;

		m_BoundingVolume = m_pBody->GetShape()->GetBoundingVolume();
	}

	//--------------------------------------------------------------

	cColliderEntity::~cColliderEntity()
	{
		if (m_pBody && m_pPhysicsWorld)
			m_pPhysicsWorld->DestroyBody(m_pBody);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cBoundingVolume *cColliderEntity::GetBoundingVolume()
	{
		return &m_BoundingVolume;
	}

	//--------------------------------------------------------------

	cMatrixf *cColliderEntity::GetModelMatrix(cCamera3D *a_pCamera)
	{
		return &GetWorldMatrix();
	}

	//--------------------------------------------------------------

	int cColliderEntity::GetMatrixUpdateCount()
	{
		return GetTransformUpdateCount();
	}
}