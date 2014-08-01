#ifndef EFE_BONE_H
#define EFE_BONE_H

#include <map>

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"

namespace efe
{
	class cSkeleton;
	class cBone;

	typedef std::list<cBone*> tBoneList;
	typedef tBoneList::iterator tBoneListIt;

	typedef cSTLIterator<cBone*, tBoneList, tBoneListIt> cBoneIterator;

	class cBone
	{
		friend class cSkeleton;
	public:
		cBone(const tString &a_sName, cSkeleton *a_pSkeleton);
		~cBone();

		cBone *CreateChildBone(const tString &a_sName);

		void SetTransform(const cMatrixf &a_mtxTransform);
		const cMatrixf &GetLocalTransform();
		const cMatrixf &GetWorldTransform();
		const cMatrixf &GetInvWorldTransform();

		const tString &GetName() {return m_sName;}

	private:
		void NeedsUpdate();

		void UpdateMatrix();

		tString m_sName;

		cMatrixf m_mtxTransform;

		cMatrixf m_mtxWorldTransform;
		cMatrixf m_mtxInvWorldTransform;

		//cVector3f m_vPosition;
		//cQuaternion m_qOrientation;

		cBone *m_pParent;
		tBoneList m_lstChildren;

		cSkeleton *m_pSkeleton;

		bool m_bNeedsUpdate;

		int m_lValue;
	};
};

#endif