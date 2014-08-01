#include "graphics/Skeleton.h"

#include "graphics/Bone.h"
#include "system/MemoryManager.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSkeleton::cSkeleton()
	{
		m_pRootBone = efeNew(cBone, ("__root_bone", this));
		m_pRootBone->SetTransform(cMatrixf::Identity);
	}

	//--------------------------------------------------------------

	cSkeleton::~cSkeleton()
	{
		efeDelete(m_pRootBone);
	}

	//-------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	void cSkeleton::AddBone(cBone *a_pBone)
	{
		m_vBones.push_back(a_pBone);

		m_mapBonesIdxByName.insert(tBoneIdxNameMap::value_type(a_pBone->GetName(), (int)m_vBones.size() - 1));
	}

	void cSkeleton::RemoveBone(cBone *a_pBone)
	{
		tBoneVecIt vecIt = m_vBones.begin();
		for (; vecIt != m_vBones.end(); ++vecIt)
		{
			if (*vecIt == a_pBone)
			{
				m_vBones.erase(vecIt);
				break;
			}
		}

		m_mapBonesIdxByName.clear();

		for (int i=0; i<(int)m_vBones.size(); i++)
			m_mapBonesIdxByName.insert(tBoneIdxNameMap::value_type(m_vBones[i]->GetName(), i));
	}

	//-------------------------------------------------------------

	cBone *cSkeleton::GetRootBone()
	{
		return m_pRootBone;
	}

	//-------------------------------------------------------------

	cBone *cSkeleton::GetBoneByIndex(int a_lIndex)
	{
		return m_vBones[a_lIndex];
	}

	cBone *cSkeleton::GetBoneByName(const tString &a_sName)
	{
		int a_lIdx  = GetBoneIndexByName(a_sName);
		if (a_lIdx < 0) return NULL;

		return m_vBones[a_lIdx];
	}

	int cSkeleton::GetBoneIndexByName(const tString &a_sName)
	{
		tBoneIdxNameMapIt it = m_mapBonesIdxByName.find(a_sName);
		if (it == m_mapBonesIdxByName.end()) return -1;

		return it->second;
	}
	
	int cSkeleton::GetBoneNum()
	{
		return (int)m_vBones.size();
	}
}