#include "graphics/AnimationTrack.h"

#include "math/Math.h"
#include "graphics/Animation.h"
#include "system/LowLevelSystem.h"
#include "scene/Node3D.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cAnimationTrack::cAnimationTrack(const tString &a_sName, tAnimTransformFlag a_TransformFlags, cAnimation *a_pParent)
	{
		m_sName = a_sName;
		m_TransformFlags = a_TransformFlags;
		m_pParent = a_pParent;

		m_fMaxFrameTime = 0;

		m_lNodeIdx = -1;
	}

	//--------------------------------------------------------------

	cAnimationTrack::~cAnimationTrack()
	{
		STLDeleteAll(m_vKeyFrames);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cAnimationTrack::ResizeKeyFrames(int a_lSize)
	{
		m_vKeyFrames.resize(a_lSize);
	}

	//--------------------------------------------------------------

	cKeyFrame *cAnimationTrack::CreateKeyFrame(float a_fTime)
	{
		cKeyFrame *pFrame = efeNew(cKeyFrame, ());
		pFrame->time = a_fTime;

		if (a_fTime > m_fMaxFrameTime || m_vKeyFrames.empty())
		{
			m_vKeyFrames.push_back(pFrame);
			m_fMaxFrameTime = a_fTime;
		}
		else
		{
			tKeyFramePtrVecIt it = m_vKeyFrames.begin();
			for (; it != m_vKeyFrames.end(); ++it)
			{
				if (a_fTime < (*it)->time)
					break;
			}
			m_vKeyFrames.insert(it,pFrame);
		}

		return pFrame;
	}

	//--------------------------------------------------------------

	void cAnimationTrack::ApplyToNode(cNode3D *a_pNode, float a_fTime, float a_fWeight)
	{
		cKeyFrame Frame = GetInterpolatedKeyFrame(a_fTime);

		//Rotation
		cQuaternion qRot = cMath::QuaternionSlerp(a_fWeight, cQuaternion::Identity, Frame.rotation, true);
		a_pNode->AddRotation(qRot);

		//Translation
		cVector3f vTrans = Frame.trans * a_fWeight;
		a_pNode->AddTranslation(vTrans);
	}

	//--------------------------------------------------------------

	cKeyFrame cAnimationTrack::GetInterpolatedKeyFrame(float a_fTime)
	{
		cKeyFrame ResultKeyFrame;
		ResultKeyFrame.time = a_fTime;

		cKeyFrame *pKeyFrameA = NULL;
		cKeyFrame *pKeyFrameB = NULL;

		float fT = GetKeyFramesAtTime(a_fTime, &pKeyFrameA, &pKeyFrameB);

		if (fT == 0.0f)
		{
			ResultKeyFrame.rotation = pKeyFrameA->rotation;
			ResultKeyFrame.scale = pKeyFrameA->scale;
			ResultKeyFrame.trans = pKeyFrameA->trans;
		}
		else
		{
			ResultKeyFrame.rotation = cMath::QuaternionSlerp(fT, pKeyFrameA->rotation, pKeyFrameB->rotation, true);

			ResultKeyFrame.scale = pKeyFrameA->scale * (1 - fT) + pKeyFrameB->scale * fT;
			ResultKeyFrame.trans = pKeyFrameA->trans * (1 - fT) + pKeyFrameB->trans * fT;
		}

		return ResultKeyFrame;
	}

	//--------------------------------------------------------------

	float cAnimationTrack::GetKeyFramesAtTime(float a_fTime, cKeyFrame **a_pKeyFrameA, cKeyFrame **a_pKeyFrameB)
	{
		float fTotalAnimLength = m_pParent->GetLength();

		a_fTime = cMath::Clamp(a_fTime, 0, fTotalAnimLength);

		if (a_fTime >= m_fMaxFrameTime)
		{
			*a_pKeyFrameA = m_vKeyFrames[m_vKeyFrames.size()-1];
			*a_pKeyFrameB = m_vKeyFrames[0];

			float fDeltaT = fTotalAnimLength - (*a_pKeyFrameA)->time;

			return 0.0f;
		}

		const int lSize = (int)m_vKeyFrames.size();

		int lIdxB = -1;
		for (int i=0; i<lSize; i++)
		{
			if (a_fTime <= m_vKeyFrames[i]->time)
			{
				lIdxB = i;
				break;
			}
		}

		if (lIdxB == 0)
		{
			*a_pKeyFrameA = m_vKeyFrames[0];
			*a_pKeyFrameB = m_vKeyFrames[0];
			return 0.0f;
		}

		*a_pKeyFrameA = m_vKeyFrames[lIdxB-1];
		*a_pKeyFrameB = m_vKeyFrames[lIdxB];

		float fDeltaT = (*a_pKeyFrameB)->time - (*a_pKeyFrameA)->time;

		return (a_fTime - (*a_pKeyFrameA)->time) / fDeltaT;
	}
}