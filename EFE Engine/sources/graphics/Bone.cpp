#include "graphics/Bone.h"

#include "graphics/Skeleton.h"
#include "math/Math.h"

#include "system/LowLevelSystem.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cBone::cBone(const tString &a_sName, cSkeleton *a_pSkeleton)
	{
		m_sName = a_sName;
		m_pSkeleton = a_pSkeleton;
		m_pParent = NULL;

		m_bNeedsUpdate = true;

		m_lValue = 0;
	}

	//--------------------------------------------------------------

	cBone::~cBone()
	{
		STLDeleteAll(m_lstChildren);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cBone *cBone::CreateChildBone(const tString &a_sName)
	{
		cBone *pBone = efeNew(cBone, (a_sName, m_pSkeleton));
		pBone->m_pParent = this;

		m_lstChildren.push_back(pBone);
		m_pSkeleton->AddBone(pBone);

		return pBone;
	}

	//--------------------------------------------------------------

	void cBone::SetTransform(const cMatrixf &a_mtxTransform)
	{
		m_mtxTransform = a_mtxTransform;

		NeedsUpdate();
	}

	//--------------------------------------------------------------

	const cMatrixf &cBone::GetLocalTransform()
	{
		return m_mtxTransform;
	}

	const cMatrixf &cBone::GetWorldTransform()
	{
		UpdateMatrix();

		return m_mtxWorldTransform;
	}

	const cMatrixf &cBone::GetInvWorldTransform()
	{
		UpdateMatrix();

		return m_mtxInvWorldTransform;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cBone::NeedsUpdate()
	{
		m_bNeedsUpdate = true;

		tBoneListIt it = m_lstChildren.begin();
		for (; it != m_lstChildren.end(); it++)
		{
			cBone *pBone = *it;

			pBone->NeedsUpdate();
		}
	}

	//--------------------------------------------------------------
	
	void cBone::UpdateMatrix()
	{
		if (m_bNeedsUpdate == false) return;

		if (m_pParent == NULL)
			m_mtxWorldTransform = m_mtxTransform;
		else
			m_mtxWorldTransform = cMath::MatrixMul(m_pParent->GetWorldTransform(), m_mtxTransform);

		m_mtxInvWorldTransform = cMath::MatrixInverse(m_mtxWorldTransform);

		m_bNeedsUpdate = false;
	}
}