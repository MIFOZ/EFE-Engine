#ifndef EFE_OPENALSOUNDENVIRONMENT_H
#define EFE_OPENALSOUNDENVIRONMENT_H

#include "sound/SoundEnvironment.h"

namespace efe
{
	class cSoundManager;

	class cOpenALSoundEnvironment : public iSoundEnvironment
	{
	public:
		cOpenALSoundEnvironment();

		~cOpenALSoundEnvironment();

		bool CreateFromFile(const tString &a_sFile);

		inline float GetDensity() {return m_fDensity;}
		inline float GetDiffusion() {return m_fDiffusion;}
		inline float GetGain() {return m_fGain;}
		inline float GetGainHF() {return m_fGainHF;}
		inline float GetGainLF() {return m_fGainLF;}
		inline float GetDecayTime() {return m_fDecayTime;}
		inline float GetDecayHFRatio() {return m_fDecayHFRatio;}
		inline float GetDecayLFRatio() {return m_fDecayHFRatio;}
		inline float GetReflectionsGain() {return m_fReflectionsGain;}
		inline float GetReflectionsDelay() {return m_fReflectionsDelay;}
		inline float *GetReflectionsPan() {return m_vReflectionsPan;}
		inline float GetLateReverbGain() {return m_fLateReverbGain;}
		inline float GetLateReverbDelay() {return m_fLateReverbDelay;}
		inline float *GetLateReverbPan() {return m_vLateReverbPan;}
		inline float GetEchoTime() {return m_fEchoTime;}
		inline float GetEchoDepth() {return m_fEchoDepth;}
		inline float GetModulationTime() {return m_fModulationTime;}
		inline float GetModulationDepth() {return m_fModulationDepth;}
		inline float GetAirAbsorptionGainHF() {return m_fAirAbsorptionGainHF;}
		inline float GetHFReference() {return m_fHFReference;}
		inline float GetLFReference() {return m_fLFReference;}
		inline float GetRoomRolloffFactor() {return m_fRoomRolloffFactor;}
		inline int GetDecayHFLimit() {return m_fDecayHFRatio;}

		inline void SetDensity(float a_fDensity) {m_fDensity = a_fDensity;}
		inline void SetDiffusion(float a_fDiffusion) {m_fDiffusion = a_fDiffusion;}
		inline void SetGain(float a_fGain) {m_fGain = a_fGain;}
		inline void SetGainHF(float a_fGainHF) {m_fGainHF = a_fGainHF;}
		inline void SetGainLF(float a_fGainLF) {m_fGainLF = a_fGainLF;}
		inline void SetDecayTime(float a_fDecayTime) {m_fDecayTime = a_fDecayTime;}
		inline void SetDecayHFRatio(float a_fDecayHFRatio) {m_fDecayHFRatio = a_fDecayHFRatio;}
		inline void SetDecayLFRatio(float a_fDecayHFRatio) {m_fDecayHFRatio = a_fDecayHFRatio;}
		inline void SetReflectionsGain(float a_fReflectionsGain) {m_fReflectionsGain = a_fReflectionsGain;}
		inline void SetReflectionsDelay(float a_fReflectionsDelay) {m_fReflectionsDelay = a_fReflectionsDelay;}
		inline void SetReflectionsPan(float a_vReflectionsPan[3]) {m_vReflectionsPan[0] = a_vReflectionsPan[0];m_vReflectionsPan[1] = a_vReflectionsPan[1];m_vReflectionsPan[2] = a_vReflectionsPan[2];}
		inline void SetLateReverbGain(float a_fLateReverbGain) {m_fLateReverbGain = a_fLateReverbGain;}
		inline void SetLateReverbDelay(float a_fLateReverbDelay) {m_fLateReverbDelay = a_fLateReverbDelay;}
		inline void SetLateReverbPan(float a_vLateReverbPan[3]) {m_vLateReverbPan[0] = a_vLateReverbPan[0];m_vLateReverbPan[1] = a_vLateReverbPan[1];m_vLateReverbPan[2] = a_vLateReverbPan[2];}
		inline void SetEchoTime(float a_fEchoTime) {m_fEchoTime = a_fEchoTime;}
		inline void SetEchoDepth(float a_fEchoDepth) {m_fEchoDepth = a_fEchoDepth;}
		inline void SetModulationTime(float a_fModulationTime) {m_fModulationTime = a_fModulationTime;}
		inline void SetModulationDepth(float a_fModulationDepth) {m_fModulationDepth = a_fModulationDepth;}
		inline void SetAirAbsorptionGainHF(float a_fAirAbsorptionGainHF) {m_fAirAbsorptionGainHF = a_fAirAbsorptionGainHF;}
		inline void SetHFReference(float a_fHFReference) {m_fHFReference = a_fHFReference;}
		inline void SetLFReference(float a_fLFReference) {m_fLFReference = a_fLFReference;}
		inline void SetRoomRolloffFactor(float a_fRoomRolloffFactor) {m_fRoomRolloffFactor = a_fRoomRolloffFactor;}
		inline void SetDecayHFLimit(int a_fDecayHFRatio) {m_fDecayHFRatio = a_fDecayHFRatio;}

	protected:
		float m_fDensity;
		float m_fDiffusion;
		float m_fGain;
		float m_fGainHF;
		float m_fGainLF;
		float m_fDecayTime;
		float m_fDecayHFRatio;
		float m_fDecayLFRatio;
		float m_fReflectionsGain;
		float m_fReflectionsDelay;
		float m_vReflectionsPan[3];
		float m_fLateReverbGain;
		float m_fLateReverbDelay;
		float m_vLateReverbPan[3];
		float m_fEchoTime;
		float m_fEchoDepth;
		float m_fModulationTime;
		float m_fModulationDepth;
		float m_fAirAbsorptionGainHF;
		float m_fHFReference;
		float m_fLFReference;
		float m_fRoomRolloffFactor;
		int m_lDecayHFLimit;
	};
};

#endif