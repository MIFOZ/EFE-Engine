#include "graphics/Animation.h"

#include "math/Math.h"
#include "graphics/AnimationTrack.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cAnimation::cAnimation(const tString &a_sName, const tString &a_sFile) : iResourceBase(a_sName, 0)
	{
		m_sAnimName = "";
		m_sFileName = a_sFile;
	}


	//--------------------------------------------------------------

	cAnimation::~cAnimation()
	{
		STLDeleteAll(m_vTracks);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	float cAnimation::GetLength()
	{
		return m_fLength;
	}

	//--------------------------------------------------------------

	void cAnimation::SetLength(float a_fTime)
	{
		m_fLength = a_fTime;
	}

	//--------------------------------------------------------------

	cAnimationTrack *cAnimation::CreateTrack(const tString &a_sName, tAnimTransformFlag a_Flags)
	{
		cAnimationTrack *pTrack = efeNew(cAnimationTrack, (a_sName, a_Flags, this));

		m_vTracks.push_back(pTrack);

		return pTrack;
	}

	//--------------------------------------------------------------

	cAnimationTrack *cAnimation::GetTrack(int a_lIndex)
	{
		return m_vTracks[a_lIndex];
	}

	//--------------------------------------------------------------

	cAnimationTrack *cAnimation::GetTrackByName(const tString &a_sName)
	{
		for (rsize_t i=0; i<m_vTracks.size(); ++i)
		{
			if (a_sName == tString(m_vTracks[i]->GetName()))
				return m_vTracks[i];
		}

		return NULL;
	}

	//--------------------------------------------------------------

	void cAnimation::ResizeTracks(int a_lNum)
	{
		m_vTracks.reserve(a_lNum);
	}

	//--------------------------------------------------------------

	int cAnimation::GetTrackNum()
	{
		return m_vTracks.size();
	}
}