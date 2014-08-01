#ifndef EFE_PHYSICS_JOINT_NEWTON_H
#define EFE_PHYSICS_JOINT_NEWTON_H

#include <Newton.h>
#include "impl/PhysicsWorldNewton.h"
#include "impl/PhysicsBodyNewton.h"

#include "system/LowLevelSystem.h"

namespace efe
{
	template <typename T>
	class iPhysicsJointNewton : public T
	{
	public:
		iPhysicsJointNewton(const tString &a_sName,
			iPhysicsBody *a_pParentBody, iPhysicsBody *a_pChildBody, 
			iPhysicsWorld *a_pWorld, const cVector3f &a_vPivotPoint)
			: T(a_sName, a_pParentBody, a_pChildBody, a_pWorld, a_vPivotPoint)
		{
			cPhysicsWorldNewton *pWorld = static_cast<cPhysicsWorldNewton*>(a_pWorld);

			m_pNewtonWorld = pWorld->GetNewtonWorld();

			cPhysicsBodyNewton *pNParent = static_cast<cPhysicsBodyNewton*>(a_pParentBody);
			cPhysicsBodyNewton *pNChild = static_cast<cPhysicsBodyNewton*>(a_pChildBody);

			if (a_pParentBody == NULL)
				m_pNewtonParentBody = NULL;
			else
				m_pNewtonParentBody = pNParent->GetNewtonBody();

			m_pNewtonChildBody = pNChild->GetNewtonBody();
		}

		virtual ~iPhysicsJointNewton()
		{
			if (this->m_pChildBody || this->m_pParentBody)
				NewtonDestroyJoint(m_pNewtonWorld, m_pNewtonJoint);
		}

		void SetCollideBodies(bool a_bX)
		{
			NewtonJointSetCollisionState(m_pNewtonJoint, a_bX ? 1 : 0);
		}

		bool GetCollideBodies()
		{
			return NewtonJointGetCollisionState(m_pNewtonJoint) == 0 ? false : true;
		}

		void SetStiffness(float a_fX)
		{
			NewtonJointSetStiffness(m_pNewtonJoint, a_fX);
		}

		float GetStiffness()
		{
			return NewtonJointGetStiffness(m_pNewtonJoint);
		}

	protected:
		NewtonJoint *m_pNewtonJoint;
		const NewtonWorld *m_pNewtonWorld;
		NewtonBody *m_pNewtonParentBody;
		NewtonBody *m_pNewtonChildBody;
	};
};
#endif