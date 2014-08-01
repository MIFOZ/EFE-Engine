#include "impl/OpenALSoundEnvironment.h"
#include "tinyxml.h"

namespace efe
{
	cOpenALSoundEnvironment::cOpenALSoundEnvironment()
	{
		
	}

	cOpenALSoundEnvironment::~cOpenALSoundEnvironment()
	{
		
	}

	bool cOpenALSoundEnvironment::CreateFromFile(const tString &a_sFile)
	{
		tString sType;
		TiXmlDocument doc;
		if (!doc.LoadFile(a_sFile.c_str()))
			return false;

		TiXmlElement *pMain = doc.FirstChildElement("SoundEnvironment")->FirstChildElement("Main");
		if (pMain)
		{
			sType = pMain->Attribute("Type");
			m_sName = pMain->Attribute("Name");
		}

		float *pfTemp;

		TiXmlElement *pParams = doc.FirstChildElement("SoundEnvironment")->FirstChildElement("Parameters");

		if ((pParams == NULL) || (sType.compare("OpenAL")!=0))
		{
			doc.Clear();
			return false;
		}

		m_fDensity = cString::ToFloat(pParams->Attribute("Density"),0);
		m_fDiffusion = cString::ToFloat(pParams->Attribute("Diffusion"),0);
		m_fGain = cString::ToFloat(pParams->Attribute("Gain"),0);
		m_fGainHF = cString::ToFloat(pParams->Attribute("GainHF"),0);
		m_fGainLF = cString::ToFloat(pParams->Attribute("GainLF"),0);
		m_fDecayTime = cString::ToFloat(pParams->Attribute("DecayTime"),0);
		m_fDecayHFRatio = cString::ToFloat(pParams->Attribute("DecayHFRatio"),0);
		m_fDecayLFRatio = cString::ToFloat(pParams->Attribute("DecayLFRatio"),0);
		m_fReflectionsGain = cString::ToFloat(pParams->Attribute("ReflectionsGain"),0);
		m_fReflectionsDelay = cString::ToFloat(pParams->Attribute("ReflectionsDelay"),0);
		pfTemp = cString::ToVector3f(pParams->Attribute("ReflectionsPan"),cVector3f(0)).v;
		m_vReflectionsPan[0] = pfTemp[0];
		m_vReflectionsPan[1] = pfTemp[1];
		m_vReflectionsPan[2] = pfTemp[2];
		m_fLateReverbGain = cString::ToFloat(pParams->Attribute("LateReverbGain"),0);
		m_fLateReverbDelay = cString::ToFloat(pParams->Attribute("LateReverbDelay"),0);
		pfTemp = cString::ToVector3f(pParams->Attribute("LateReverbPan"),cVector3f(0)).v;
		m_vLateReverbPan[0] = pfTemp[0];
		m_vLateReverbPan[1] = pfTemp[1];
		m_vLateReverbPan[2] = pfTemp[2];
		m_fEchoTime = cString::ToFloat(pParams->Attribute("EchoTime"),0);
		m_fEchoDepth = cString::ToFloat(pParams->Attribute("EchoDepth"),0);
		m_fModulationTime = cString::ToFloat(pParams->Attribute("ModulationTime"),0);
		m_fModulationDepth = cString::ToFloat(pParams->Attribute("ModulationDepth"),0);
		m_fAirAbsorptionGainHF = cString::ToFloat(pParams->Attribute("AirAbsorptionGainHF"),0);
		m_fHFReference = cString::ToFloat(pParams->Attribute("HFReference"),0);
		m_fLFReference = cString::ToFloat(pParams->Attribute("LFReference"),0);
		m_fRoomRolloffFactor = cString::ToFloat(pParams->Attribute("RoomRolloffFactor"),0);
		m_lDecayHFLimit = cString::ToInt(pParams->Attribute("DecayHFLimit"),0);

		doc.Clear();
		pParams = NULL;

		return true;
	}
}