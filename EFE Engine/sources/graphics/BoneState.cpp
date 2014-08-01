#include "graphics/BoneState.h"

#include "physics/PhysicsBody.h"

#include "math/Math.h"

#include "system/LowLevelSystem.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cBoneState::cBoneState(const tString &a_sName, bool a_bAutoDeleteChildren) : cNode3D(a_sName, a_bAutoDeleteChildren)
	{
		m_pBody = NULL;
		m_pColliderBody = NULL;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cBoneState::SetBody(iPhysicsBody *a_pBody)
	{
		m_pBody = a_pBody;
	}

	iPhysicsBody *cBoneState::GetBody()
	{
		return m_pBody;
	}

	//--------------------------------------------------------------

	void cBoneState::SetColliderBody(iPhysicsBody *a_pBody)
	{
		m_pBody = m_pColliderBody;
	}

	iPhysicsBody *cBoneState::GetColliderBody()
	{
		return m_pColliderBody;
	}
}