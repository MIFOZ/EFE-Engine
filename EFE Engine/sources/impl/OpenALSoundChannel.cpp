#include "impl/OpenALSoundChannel.h"
#include "impl/OpenALSoundData.h"
#include "resources/SoundManager.h"

#include "math/Math.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cOpenALSoundChannel::cOpenALSoundChannel(iSoundData *a_pData, int a_lChannel, cSoundManager *a_pSoundManager)
		: iSoundChannel(a_pData, a_pSoundManager)
	{
		m_lChannel = a_lChannel;

		for (int i=0;i<3;i++)
		{
			m_fPosition[i] = 0;
			m_fVelocity[i] = 0;
		}

		OAL_Source_SetAttributes(m_lChannel, m_fPosition, m_fVelocity);
		OAL_Source_SetFilterType(m_lChannel, eOALFilterType_LowPass);
	}

	//--------------------------------------------------------------

	cOpenALSoundChannel::~cOpenALSoundChannel()
	{
		if (m_lChannel >= 0)
			OAL_Source_Stop(m_lChannel);

		if (m_pSoundManager) m_pSoundManager->Destroy(m_pData);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cOpenALSoundChannel::Play()
	{
		SetPaused(false);

		m_bStopUsed = false;
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::Stop()
	{
		OAL_Source_Stop(m_lChannel);
		m_lChannel = -1;

		m_bStopUsed = true;
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetPaused(bool a_bX)
	{
		m_bPaused = a_bX;
		OAL_Source_SetPaused(m_lChannel, a_bX);
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetSpeed(float a_fSpeed)
	{
		m_fSpeed = a_fSpeed;

		OAL_Source_SetPitch(m_lChannel, a_fSpeed);
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetVolume(float a_fVolume)
	{
		m_fVolume = a_fVolume;

		OAL_Source_SetVolume(m_lChannel, m_fVolume);
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetLooping(bool a_bLoop)
	{
		m_bLooping = a_bLoop;

		OAL_Source_SetLoop(m_lChannel, a_bLoop);
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetPan(float a_fPan)
	{
		
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::Set3D(bool a_b3D)
	{
		m_b3D = a_b3D;
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetPriority(int a_lX)
	{
		int lPrio = a_lX + m_lPriorityModifier;
		if (lPrio>255) lPrio = 255;

		OAL_Source_SetPriority(m_lChannel, lPrio);
	}

	//--------------------------------------------------------------

	int cOpenALSoundChannel::GetPriority()
	{
		return OAL_Source_GetPriority(m_lChannel);
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetPositionRelative(bool a_bRelative)
	{
		m_bPositionRelative = a_bRelative;
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetPosition(const cVector3f &a_vPos)
	{
		m_vPosition = a_vPos;

		OAL_Source_SetAttributes(m_lChannel, m_vPosition.v, m_vVelocity.v);
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetVelocity(const cVector3f &a_vVel)
	{
		m_vVelocity = a_vVel;

		OAL_Source_SetAttributes(m_lChannel, m_vPosition.v, m_vVelocity.v);
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetMinDistance(float a_fMin)
	{
		m_fMinDistance = a_fMin;
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetMaxDistance(float a_fMax)
	{
		m_fMaxDistance = a_fMax;
	}

	//--------------------------------------------------------------

	bool cOpenALSoundChannel::IsPlaying()
	{
		return OAL_Source_IsPlaying(m_lChannel);
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetAffectedByEnv(bool a_bX)
	{
		iSoundChannel::SetAffectedByEnv(a_bX);

		if (m_bAffectedByEnv)
			OAL_Source_SetAuxSendSlot(m_lChannel, 0, 0);
		else
			OAL_Source_SetAuxSendSlot(m_lChannel, 0, -1);
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetFiltering(bool a_bEnabled, int a_lFlags)
	{
		OAL_Source_SetFiltering(m_lChannel, a_bEnabled, a_lFlags);
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetFilterGain(float a_fGain)
	{
		OAL_Source_SetFilterGain(m_lChannel, a_fGain);
	}

	//--------------------------------------------------------------

	void cOpenALSoundChannel::SetFilterGainHF(float a_fGainHF)
	{
		OAL_Source_SetFilterGainHF(m_lChannel, a_fGainHF);
	}
}