#ifndef EFE_LOWLEVELSOUND_H
#define EFE_LOWLEVELSOUND_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"

#include "system/LowLevelSystem.h"

namespace efe
{
	class iSoundData;
	class iSoundEnvironment;

	typedef std::list<iSoundEnvironment*> tSoundEnvList;
	typedef tSoundEnvList::iterator  tSoundEnvListIt;

	class iLowLevelSound
	{
	public:
		iLowLevelSound();
		virtual ~iLowLevelSound();

		virtual void GetSupportedFormats(tStringList &a_lstFormats)=0;

		virtual iSoundData *LoadSoundData(const tString &a_sName, const tString &a_sFilePath,
			const tString &a_sType, bool a_bStream, bool a_bLoopStream)=0;

		virtual void UpdateSound(float a_fTimeStep)=0;

		virtual void SetListenerAttributes(const cVector3f &a_vPos, const cVector3f &a_vVel, 
			const cVector3f &a_vFoward, const cVector3f &a_vUp)=0;

		virtual void SetListenerPosition(const cVector3f &a_vPos)=0;
		const cMatrixf &GetListenerMatrix(){return m_mtxListener;}

		virtual void SetListenerAttenuation(bool a_bEnabled)=0;

		virtual void SetRollOffFactor(float a_fFactor)=0;

		virtual void SetVolume(float a_fVolume)=0;

		cVector3f &GetListenerPosition(){return m_vListenerPosition;}
		cVector3f &GetListenerVelocity(){return m_vListenerVelocity;}
		cVector3f &GetListenerForward(){return m_vListenerForward;}
		cVector3f &GetListenerUp(){return m_vListenerUp;}
		virtual bool GetListenerAttenuation(){return m_bListenerAttenuation;}

		float GetVolume(){return m_fVolume;}

		virtual void Init(bool a_bUseHardware, bool a_bForceGeneric, bool a_bUseEnvAudio, int a_lMaxChannels,
			int a_lStreamUpdateFreq, bool a_bUseThreading, bool a_bUseVoiceManagement,
			int a_lMaxMonoSourceHint, int a_lMaxStereoSourceHint, 
			int a_lStreamingBufferSize, int a_lStreamingBufferCount, bool a_bEnableLowLevelLog, tString a_sDeviceName)=0;

		bool IsHardwareAccelerated(){return m_bHardwareAcc;}
		bool IsEnvAudioEnabled() {return m_bEnvAudioEnabled;}

		virtual void SetEnvVolume(float a_fEnvVolume)=0;
		float GetEnvVolume(){return m_fEnvVolume;}

		virtual iSoundEnvironment *LoadSoundEnvironment(const tString &a_sFilePath)=0;

		virtual void SetSoundEnvironment(iSoundEnvironment *a_pSoundEnv)=0;
		virtual void FadeSoundEnvironment(iSoundEnvironment *a_pSourceSoundEnv, iSoundEnvironment *a_pDestSoundEnv, float a_fT)=0;

		virtual iSoundEnvironment *GetSoundEnvironmentFromFileName(const tString &a_sName);

	protected:
		float m_fVolume;
		float m_fEnvVolume;
		bool m_bListenerAttenuation;

		bool m_bHardwareAcc;
		bool m_bEnvAudioEnabled;

		cVector3f m_vListenerUp;
		cVector3f m_vListenerForward;
		cVector3f m_vListenerRight;
		cVector3f m_vListenerPosition;
		cVector3f m_vListenerVelocity;

		cMatrixf m_mtxListener;

		tSoundEnvList m_lstSoundEnv;
	};
};
#endif