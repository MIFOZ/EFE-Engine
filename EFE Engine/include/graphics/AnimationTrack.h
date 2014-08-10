#ifndef EFE_ANIMATION_TRACK_H
#define EFE_ANIMATION_TRACK_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"

namespace efe
{
	class cAnimation;
	class cNode3D;

	class cAnimationTrack
	{
	public:
		cAnimationTrack(const tString &a_sName, tAnimTransformFlag a_TransformFlags, cAnimation *a_pParent);
		~cAnimationTrack();

		void ResizeKeyFrames(int a_lSize);

		cKeyFrame *CreateKeyFrame(float a_fTime);

		inline cKeyFrame *GetKeyFrame(int a_lIndex) {return m_vKeyFrames[a_lIndex];}
		inline int GetKeyFrameNum(){return (int)m_vKeyFrames.size();}

		inline tAnimTransformFlag GetTransformFlags() {return m_TransformFlags;}

		void ApplyToNode(cNode3D *a_pNode, float a_fTime, float a_fWeight);

		cKeyFrame GetInterpolatedKeyFrame(float a_fTime);

		float GetKeyFramesAtTime(float a_fTime, cKeyFrame **a_pKeyFrameA, cKeyFrame **a_pKeyFrameB);

		const char *GetName() {return m_sName.c_str();}

		void SetNodeIndex(int a_lIndex) {m_lNodeIdx = a_lIndex;}
		int GetNodeIndex() {return m_lNodeIdx;}

	private:
		float CalculateT(float a_fX0, float a_fX1, float a_fX2, float a_fX3, float a_fX);

		tString m_sName;

		int m_lNodeIdx;

		tKeyFramePtrVec m_vKeyFrames;
		tAnimTransformFlag m_TransformFlags;

		float m_fMaxFrameTime;

		cAnimation *m_pParent;
	};
};

#endif