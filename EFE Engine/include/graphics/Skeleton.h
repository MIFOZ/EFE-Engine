#ifndef EFE_SKELETON_H
#define EFE_SKELETON_H

#include <map>

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"

namespace efe
{
	class cBone;

	typedef std::vector<cBone*> tBoneVec;
	typedef tBoneVec::iterator tBoneVecIt;

	typedef std::map<tString, int> tBoneIdxNameMap;
	typedef tBoneIdxNameMap::iterator tBoneIdxNameMapIt;

	class cSkeleton
	{
	public:
		cSkeleton();
		~cSkeleton();

		void AddBone(cBone *a_pBone);
		void RemoveBone(cBone *a_pBone);

		cBone *GetRootBone();

		cBone *GetBoneByIndex(int a_lIndex);
		cBone *GetBoneByName(const tString &a_sName);
		int GetBoneIndexByName(const tString &a_sName);
		int GetBoneNum();

	private:
		cBone *m_pRootBone;

		tBoneVec m_vBones;
		tBoneIdxNameMap m_mapBonesIdxByName;
	};
};
#endif