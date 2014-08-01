#ifndef EFE_BONE_STATE_H
#define EFE_BONESATE_H

#include "scene/Node3D.h"

namespace efe
{
	class iPhysicsBody;

	class cBoneState : public cNode3D
	{
	public:
		cBoneState(const tString &a_sName, bool a_bAutoDeleteChildren);

		void SetBody(iPhysicsBody *a_pBody);
		iPhysicsBody *GetBody();

		void SetColliderBody(iPhysicsBody *a_pBody);
		iPhysicsBody *GetColliderBody();

	private:
		iPhysicsBody *m_pBody;
		cMatrixf m_mtxBody;
		cMatrixf m_mtxInvBody;

		iPhysicsBody *m_pColliderBody;
	};
};
#endif