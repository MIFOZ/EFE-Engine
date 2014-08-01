#include "impl/LowLevelSoundOpenAL.h"
#include "system/String.h"
#include "impl/OpenALSoundData.h"
#include "impl/OpenALSoundEnvironment.h"

#include "math/Math.h"

#include "system/LowLevelSystem.h"

#include "OALWrapper/OAL_Funcs.h"
#include "OALWrapper/OAL_Effect_Reverb.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cLowLevelSoundOpenAL::cLowLevelSoundOpenAL()
	{
		m_vFormats[0] = "WAV";
		m_vFormats[1] = "OGG";
		m_vFormats[3] = "";
		m_bInitialized = false;
		m_bEnvAudioEnabled = false;
		m_bNullEffectAttached = false;
	}

	//--------------------------------------------------------------

	cLowLevelSoundOpenAL::~cLowLevelSoundOpenAL()
	{
		if (m_bInitialized)
			OAL_Close();
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cLowLevelSoundOpenAL::GetSupportedFormats(tStringList &a_lstFormats)
	{
		int lPos = 0;

		while (m_vFormats[lPos] != "")
		{
			a_lstFormats.push_back(m_vFormats[lPos]);
			lPos++;
		}
	}

	//--------------------------------------------------------------

	iSoundData *cLowLevelSoundOpenAL::LoadSoundData(const tString &a_sName, const tString &a_sFilePath,
			const tString &a_sType, bool a_bStream, bool a_bLoopStream)
	{
		cOpenALSoundData *pSoundData = efeNew(cOpenALSoundData, (a_sName, a_bStream));

		pSoundData->SetLoopStream(a_bLoopStream);

		if (pSoundData->CreateFromFile(a_sFilePath) == false)
		{
			efeDelete(pSoundData);
			return NULL;
		}

		return pSoundData;
	}

	//--------------------------------------------------------------

	void cLowLevelSoundOpenAL::UpdateSound(float a_fTimeStep)
	{
		OAL_Update();
	}

	//--------------------------------------------------------------

	void cLowLevelSoundOpenAL::SetListenerAttributes(const cVector3f &a_vPos, const cVector3f &a_vVel, 
			const cVector3f &a_vFoward, const cVector3f &a_vUp)
	{
		m_vListenerPosition = a_vPos;
		m_vListenerVelocity = a_vVel;
		m_vListenerForward = a_vFoward;
		m_vListenerUp = a_vUp;

		m_vListenerRight = cMath::Vector3Cross(m_vListenerForward,m_vListenerUp);

		m_mtxListener = cMatrixf::Identity;
		m_mtxListener.SetRight(m_vListenerRight);
		m_mtxListener.SetUp(m_vListenerUp);
		m_mtxListener.SetForward(m_vListenerForward*-1);
		m_mtxListener = cMath::MatrixInverse(m_mtxListener);
		m_mtxListener.SetTranslation(m_vListenerPosition);

		float vVel[3] = {0,0,0};
		OAL_Listener_SetAttributes(a_vPos.v, a_vVel.v, (a_vFoward*(-1)).v, a_vUp.v);
	}

	//--------------------------------------------------------------

	void cLowLevelSoundOpenAL::SetListenerPosition(const cVector3f &a_vPos)
	{
		m_vListenerPosition = a_vPos;

		OAL_Listener_SetAttributes(a_vPos.v, m_vListenerVelocity.v, (m_vListenerForward*(-1)).v, m_vListenerUp.v);
	}

	//--------------------------------------------------------------

	void cLowLevelSoundOpenAL::SetListenerAttenuation(bool a_bEnabled)
	{
		m_bListenerAttenuation = a_bEnabled;
	}

	//--------------------------------------------------------------

	void cLowLevelSoundOpenAL::SetRollOffFactor(float a_fFactor)
	{
		OAL_SetRollOffFactor(a_fFactor);
	}

	//--------------------------------------------------------------

	void cLowLevelSoundOpenAL::SetVolume(float a_fVolume)
	{
		m_fVolume = a_fVolume;

		OAL_Listener_SetMasterVolume(a_fVolume);
	}

	//--------------------------------------------------------------

	void cLowLevelSoundOpenAL::Init(bool a_bUseHardware, bool a_bForceGeneric, bool a_bUseEnvAudio, int a_lMaxChannels,
			int a_lStreamUpdateFreq, bool a_bUseThreading, bool a_bUseVoiceManagement,
			int a_lMaxMonoSourceHint, int a_lMaxStereoSourceHint, 
			int a_lStreamingBufferSize, int a_lStreamingBufferCount, bool a_bEnableLowLevelLog, tString a_sDeviceName)
	{
		cOpenALSoundEnvironment *pEnv = efeNew(cOpenALSoundEnvironment, ());

		m_bLogSounds = a_bEnableLowLevelLog;

		Log(" Initializing OpenAL\n");

		cOAL_Init_Params cInitParams;
		cInitParams.mbVoiceManagement = a_bUseVoiceManagement;
		cInitParams.mlNumSourcesHint = a_lMaxChannels;
		cInitParams.mlUpdateFreq = a_lStreamUpdateFreq;
		cInitParams.mlStreamingBufferCount = a_lStreamingBufferCount;
		cInitParams.mlStreamingBufferSize = a_lStreamingBufferSize;
		cInitParams.mlMinMonoSourcesHint = a_lMaxMonoSourceHint;
		cInitParams.mlMinStereoSourcesHint = a_lMaxStereoSourceHint;
		cInitParams.mbUseThread = a_bUseThreading;
		cInitParams.mlOutputFreq = 44100;
		cInitParams.msDeviceName = a_sDeviceName;

		if (m_bLogSounds)
			Log("  Sound logging enabled\n");

		cInitParams.mbUseEFX = a_bUseEnvAudio;

		if (a_bUseHardware)
		{
			Log("  Trying to open hardware device...%s ", a_bForceGeneric ? "(Generic forced)" : "");

			OAL_SetupLogging(m_bLogSounds, eOAL_LogOutput_File, eOAL_LogVerbose_High, "EFE_OpenAL_HWDevice");

			if (a_bForceGeneric)
				cInitParams.msDeviceName = "Generic Hardware";
		}
		else
		{
			Log("  Trying to open software device... ");

			OAL_SetupLogging(m_bLogSounds, eOAL_LogOutput_File, eOAL_LogVerbose_High, "EFE_OpenAL_SWDevice");

			if (a_bForceGeneric)
				cInitParams.msDeviceName = "Generic Software";
		}

		m_bInitialized = OAL_Init(cInitParams);

		if (!m_bInitialized)
		{
			Error("Failed to initialize audio Trying fallback driver\n");
			cInitParams.msDeviceName = "NULL";
			m_bInitialized = OAL_Init(cInitParams);
		}
		if (!m_bInitialized)
		{
			FatalError("Failed! Check your OpenAL installation\n");
			return;
		}

		const char *w = OAL_Info_GetDeviceName();
		m_bHardwareAcc = (strcmp(OAL_Info_GetDeviceName(), "Generic Software") != 0);

		if (a_bUseHardware)
		{
			if (m_bHardwareAcc)
				Log("Success!\n");
			else
				Log("Failed openning hardware device, using software\n");
		}
		else
			Log("Success!\n");

		OAL_SetDistanceModel(eOAL_DistanceModel_None);

		Log("  Device name: %s\n", OAL_Info_GetDeviceName());
		Log("  Number of mono sources: %d\n", OAL_Info_GetNumSources());
		Log("  Vendor name: %s\n", OAL_Info_GetVendorName());
		Log("  Renderer: %s\n", OAL_Info_GetRendererName());
		Log("  OpenAL Version Supported: %d.%d\n", OAL_Info_GetMajorVersion(), OAL_Info_GetMinorVersion());
		Log("  EFX: %d\n", OAL_Info_IsEFXActive());
		Log("  Output Devices: \n");
		tStringVec vDevices = OAL_Info_GetOutputDevices();
		tStringVecIt it;
		for (it = vDevices.begin(); it != vDevices.end(); ++it)
			Log("    %s\n", (*it).c_str());

		if (a_bUseEnvAudio)
		{
			m_bEnvAudioEnabled = OAL_Info_IsEFXActive();
			m_pEffect = OAL_Effect_Reverb_Create();
			if (m_bEnvAudioEnabled && (m_pEffect != NULL))
			{
				OAL_EffectSlot_AttachEffect(0, (cOAL_Effect*)m_pEffect);
				OAL_EffectSlot_SetAutoAdjust(0, false);

				SetEnvVolume(1.0f);

				Log("  Setting up Environmental Audio... Success\n");
			}
			else
				Log("  Setting up Environmental Audio... Failed\n");
		}

		float Pos[3] = {0,0,0};
		float Vel[3] = {0,0,0};

		m_vListenerForward = cVector3f(0,0,1);
		m_vListenerUp = cVector3f(0,1,0);

		OAL_Listener_SetAttributes(Pos, Vel, m_vListenerForward.v, m_vListenerUp.v);

		SetVolume(1.0f);
	}

	//--------------------------------------------------------------

	void cLowLevelSoundOpenAL::SetEnvVolume(float a_fEnvVolume)
	{
		if (!m_bEnvAudioEnabled)
			return;

		if (a_fEnvVolume < 0)
			a_fEnvVolume = 0;
		if (a_fEnvVolume > 1)
			a_fEnvVolume = 1;
		m_fEnvVolume = a_fEnvVolume;
		OAL_EffectSlot_SetGain(0, m_fEnvVolume);
	}

	//--------------------------------------------------------------

	iSoundEnvironment *cLowLevelSoundOpenAL::LoadSoundEnvironment(const tString &a_sFilePath)
	{
		if (!m_bEnvAudioEnabled)
			return NULL;

		iSoundEnvironment *pSE = GetSoundEnvironmentFromFileName(a_sFilePath);
		if (pSE) return pSE;

		cOpenALSoundEnvironment *pSoundEnv = efeNew(cOpenALSoundEnvironment, ());

		if (pSoundEnv->CreateFromFile(a_sFilePath) == false)
		{
			efeDelete(pSoundEnv);
			return NULL;
		}

		pSoundEnv->SetFileName(cString::ToLowerCase(a_sFilePath));

		m_lstSoundEnv.push_back(pSoundEnv);

		return pSoundEnv;
	}

	//--------------------------------------------------------------

	void cLowLevelSoundOpenAL::SetSoundEnvironment(iSoundEnvironment *a_pSoundEnv)
	{
		if (!m_bEnvAudioEnabled)
			return;

		if (a_pSoundEnv == NULL)
		{
			OAL_Effect_Reverb_SetDensity(m_pEffect, 0);
			OAL_Effect_Reverb_SetDiffusion(m_pEffect, 0);
			OAL_Effect_Reverb_SetEchoTime(m_pEffect, 0);
			OAL_Effect_Reverb_SetEchoDepth(m_pEffect, 0);
			OAL_Effect_Reverb_SetModulationTime(m_pEffect, 0);
			OAL_Effect_Reverb_SetModulationDepth(m_pEffect, 0);
			OAL_Effect_Reverb_SetGain(m_pEffect, 0);
			OAL_Effect_Reverb_SetGainHF(m_pEffect, 0);
			OAL_Effect_Reverb_SetGainLF(m_pEffect, 0);
			OAL_Effect_Reverb_SetReflectionsDelay(m_pEffect, 0);
			OAL_Effect_Reverb_SetReflectionsGain(m_pEffect, 0);
			OAL_Effect_Reverb_SetLateReverbGain(m_pEffect, 0);
			OAL_Effect_Reverb_SetLateReverbDelay(m_pEffect, 0);
			OAL_Effect_Reverb_SetHFReference(m_pEffect, 0);
			OAL_Effect_Reverb_SetLFReference(m_pEffect, 0);
			OAL_Effect_Reverb_SetAirAbsorptionGainHF(m_pEffect, 0);
			OAL_Effect_Reverb_SetDecayTime(m_pEffect, 0);
			OAL_Effect_Reverb_SetDecayHFRatio(m_pEffect, 0);
			OAL_Effect_Reverb_SetDecayLFRatio(m_pEffect, 0);
			OAL_Effect_Reverb_SetDecayHFLimit(m_pEffect, 0);
			OAL_Effect_Reverb_SetRoomRolloffFactor(m_pEffect, 0);

			return;
		}

		cOpenALSoundEnvironment *pEnv = static_cast<cOpenALSoundEnvironment*>(a_pSoundEnv);

		OAL_Effect_Reverb_SetDensity(m_pEffect, pEnv->GetDensity());
		OAL_Effect_Reverb_SetDiffusion(m_pEffect, pEnv->GetDiffusion());
		OAL_Effect_Reverb_SetEchoTime(m_pEffect, pEnv->GetEchoTime());
		OAL_Effect_Reverb_SetEchoDepth(m_pEffect, pEnv->GetEchoDepth());
		OAL_Effect_Reverb_SetModulationTime(m_pEffect, pEnv->GetModulationTime());
		OAL_Effect_Reverb_SetModulationDepth(m_pEffect, pEnv->GetModulationDepth());
		OAL_Effect_Reverb_SetGain(m_pEffect, pEnv->GetGain());
		OAL_Effect_Reverb_SetGainHF(m_pEffect, pEnv->GetGainHF());
		OAL_Effect_Reverb_SetGainLF(m_pEffect, pEnv->GetGainLF());
		OAL_Effect_Reverb_SetReflectionsDelay(m_pEffect, pEnv->GetReflectionsDelay());
		OAL_Effect_Reverb_SetReflectionsGain(m_pEffect, pEnv->GetReflectionsGain());
		OAL_Effect_Reverb_SetLateReverbGain(m_pEffect, pEnv->GetLateReverbGain());
		OAL_Effect_Reverb_SetLateReverbDelay(m_pEffect, pEnv->GetLateReverbDelay());
		OAL_Effect_Reverb_SetHFReference(m_pEffect, pEnv->GetHFReference());
		OAL_Effect_Reverb_SetLFReference(m_pEffect, pEnv->GetLFReference());
		OAL_Effect_Reverb_SetAirAbsorptionGainHF(m_pEffect, pEnv->GetAirAbsorptionGainHF());
		OAL_Effect_Reverb_SetDecayTime(m_pEffect, pEnv->GetDecayTime());
		OAL_Effect_Reverb_SetDecayHFRatio(m_pEffect, pEnv->GetDecayHFRatio());
		OAL_Effect_Reverb_SetDecayLFRatio(m_pEffect, pEnv->GetDecayLFRatio());
		OAL_Effect_Reverb_SetDecayHFLimit(m_pEffect, pEnv->GetDecayHFLimit());
		OAL_Effect_Reverb_SetRoomRolloffFactor(m_pEffect, pEnv->GetRoomRolloffFactor());

		OAL_EffectSlot_AttachEffect(0, (cOAL_Effect*)m_pEffect);
	}

	//--------------------------------------------------------------

	void cLowLevelSoundOpenAL::FadeSoundEnvironment(iSoundEnvironment *a_pSourceSoundEnv, iSoundEnvironment *a_pDestSoundEnv, float a_fT)
	{
		if (!m_bEnvAudioEnabled)
			return;

		if (a_fT < 0)
			a_fT = 0;
		if (a_fT > 1)
			a_fT = 1;

		float fOneMinusT = 1 - a_fT;

		if ((a_pSourceSoundEnv == NULL) && (a_pDestSoundEnv == NULL))
			return;

		cOpenALSoundEnvironment Env;

		cOpenALSoundEnvironment *pSrcEnv = static_cast<cOpenALSoundEnvironment*>(a_pSourceSoundEnv);
		cOpenALSoundEnvironment *pDstEnv = static_cast<cOpenALSoundEnvironment*>(a_pDestSoundEnv);

		if (pSrcEnv == NULL)
		{
			Env.SetDensity(pDstEnv->GetDensity() * a_fT);
			Env.SetDiffusion(pDstEnv->GetDiffusion() * a_fT);
			Env.SetGain(pDstEnv->GetGain() * a_fT);
			Env.SetGainHF(pDstEnv->GetGainHF() * a_fT);
			Env.SetGainLF(pDstEnv->GetGainLF() * a_fT);
			Env.SetDecayTime(pDstEnv->GetDecayTime() * a_fT);
			Env.SetDecayHFRatio(pDstEnv->GetDecayHFRatio() * a_fT);
			Env.SetDecayLFRatio(pDstEnv->GetDecayLFRatio() * a_fT);
			Env.SetReflectionsGain(pDstEnv->GetReflectionsGain() * a_fT);
			Env.SetReflectionsDelay(pDstEnv->GetReflectionsDelay() * a_fT);
			Env.SetLateReverbGain(pDstEnv->GetLateReverbGain() * a_fT);
			Env.SetLateReverbDelay(pDstEnv->GetLateReverbDelay() * a_fT);
			Env.SetEchoTime(pDstEnv->GetEchoTime() * a_fT);
			Env.SetEchoDepth(pDstEnv->GetEchoDepth() * a_fT);
			Env.SetModulationTime(pDstEnv->GetModulationTime() * a_fT);
			Env.SetModulationDepth(pDstEnv->GetModulationDepth() * a_fT);
			Env.SetAirAbsorptionGainHF(pDstEnv->GetAirAbsorptionGainHF() * a_fT);
			Env.SetHFReference(pDstEnv->GetHFReference() * a_fT);
			Env.SetLFReference(pDstEnv->GetLFReference() * a_fT);
			Env.SetRoomRolloffFactor(pDstEnv->GetRoomRolloffFactor() * a_fT);
			Env.SetDecayHFLimit(pDstEnv->GetDecayHFLimit() * a_fT);
		}
		else if (pDstEnv == NULL)
		{
			Env.SetDensity(pSrcEnv->GetDensity() * fOneMinusT);
			Env.SetDiffusion(pSrcEnv->GetDiffusion() * fOneMinusT);
			Env.SetGain(pSrcEnv->GetGain() * fOneMinusT);
			Env.SetGainHF(pSrcEnv->GetGainHF() * fOneMinusT);
			Env.SetGainLF(pSrcEnv->GetGainLF() * fOneMinusT);
			Env.SetDecayTime(pSrcEnv->GetDecayTime() * fOneMinusT);
			Env.SetDecayHFRatio(pSrcEnv->GetDecayHFRatio() * fOneMinusT);
			Env.SetDecayLFRatio(pSrcEnv->GetDecayLFRatio() * fOneMinusT);
			Env.SetReflectionsGain(pSrcEnv->GetReflectionsGain() * fOneMinusT);
			Env.SetReflectionsDelay(pSrcEnv->GetReflectionsDelay() * fOneMinusT);
			Env.SetLateReverbGain(pSrcEnv->GetLateReverbGain() * fOneMinusT);
			Env.SetLateReverbDelay(pSrcEnv->GetLateReverbDelay() * fOneMinusT);
			Env.SetEchoTime(pSrcEnv->GetEchoTime() * fOneMinusT);
			Env.SetEchoDepth(pSrcEnv->GetEchoDepth() * fOneMinusT);
			Env.SetModulationTime(pSrcEnv->GetModulationTime() * fOneMinusT);
			Env.SetModulationDepth(pSrcEnv->GetModulationDepth() * fOneMinusT);
			Env.SetAirAbsorptionGainHF(pSrcEnv->GetAirAbsorptionGainHF() * fOneMinusT);
			Env.SetHFReference(pSrcEnv->GetHFReference() * fOneMinusT);
			Env.SetLFReference(pSrcEnv->GetLFReference() * fOneMinusT);
			Env.SetRoomRolloffFactor(pSrcEnv->GetRoomRolloffFactor() * fOneMinusT);
			Env.SetDecayHFLimit(pSrcEnv->GetDecayHFLimit() * fOneMinusT);
		}
		else
		{
			Env.SetDensity(pSrcEnv->GetDensity() * fOneMinusT + pDstEnv->GetDensity() * a_fT);
			Env.SetDiffusion(pSrcEnv->GetDiffusion() * fOneMinusT + pDstEnv->GetDiffusion() * a_fT);
			Env.SetGain(pSrcEnv->GetGain() * fOneMinusT + pDstEnv->GetGain() * a_fT);
			Env.SetGainHF(pSrcEnv->GetGainHF() * fOneMinusT + pDstEnv->GetGainHF() * a_fT);
			Env.SetGainLF(pSrcEnv->GetGainLF() * fOneMinusT + pDstEnv->GetGainLF() * a_fT);
			Env.SetDecayTime(pSrcEnv->GetDecayTime() * fOneMinusT + pDstEnv->GetDecayTime() * a_fT);
			Env.SetDecayHFRatio(pSrcEnv->GetDecayHFRatio() * fOneMinusT + pDstEnv->GetDecayHFRatio() * a_fT);
			Env.SetDecayLFRatio(pSrcEnv->GetDecayLFRatio() * fOneMinusT + pDstEnv->GetDecayLFRatio() * a_fT);
			Env.SetReflectionsGain(pSrcEnv->GetReflectionsGain() * fOneMinusT + pDstEnv->GetReflectionsGain() * a_fT);
			Env.SetReflectionsDelay(pSrcEnv->GetReflectionsDelay() * fOneMinusT + pDstEnv->GetReflectionsDelay() * a_fT);
			Env.SetLateReverbGain(pSrcEnv->GetLateReverbGain() * fOneMinusT + pDstEnv->GetLateReverbGain() * a_fT);
			Env.SetLateReverbDelay(pSrcEnv->GetLateReverbDelay() * fOneMinusT + pDstEnv->GetLateReverbDelay() * a_fT);
			Env.SetEchoTime(pSrcEnv->GetEchoTime() * fOneMinusT + pDstEnv->GetEchoTime() * a_fT);
			Env.SetEchoDepth(pSrcEnv->GetEchoDepth() * fOneMinusT + pDstEnv->GetEchoDepth() * a_fT);
			Env.SetModulationTime(pSrcEnv->GetModulationTime() * fOneMinusT + pDstEnv->GetModulationTime() * a_fT);
			Env.SetModulationDepth(pSrcEnv->GetModulationDepth() * fOneMinusT + pDstEnv->GetModulationDepth() * a_fT);
			Env.SetAirAbsorptionGainHF(pSrcEnv->GetAirAbsorptionGainHF() * fOneMinusT + pDstEnv->GetAirAbsorptionGainHF() * a_fT);
			Env.SetHFReference(pSrcEnv->GetHFReference() * fOneMinusT + pDstEnv->GetHFReference() * a_fT);
			Env.SetLFReference(pSrcEnv->GetLFReference() * fOneMinusT + pDstEnv->GetLFReference() * a_fT);
			Env.SetRoomRolloffFactor(pSrcEnv->GetRoomRolloffFactor() * fOneMinusT + pDstEnv->GetRoomRolloffFactor() * a_fT);
			Env.SetDecayHFLimit(pSrcEnv->GetDecayHFLimit() * fOneMinusT + pDstEnv->GetDecayHFLimit() * a_fT);
		}

		SetSoundEnvironment(&Env);
	}
}