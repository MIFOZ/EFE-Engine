#include "sound/SoundEntityData.h"

#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "system/String.h"

#include "tinyxml.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSoundEntityData::cSoundEntityData(tString a_sName) : iResourceBase(a_sName, 0)
	{
		m_sMainSound = "";
		m_sStartSound = "";
		m_sStopSound = "";

		m_bFadeStart = false;
		m_bFadeStop = false;

		m_fVolume = 1;
		m_fMaxDistance = 0;
		m_fMinDistance = 0;

		m_bStream = false;
		m_bLoop = false;
		m_bUse3D = true;

		m_fRandom = 1;
		m_fInterval = 0;
	}

	//--------------------------------------------------------------

	cSoundEntityData::~cSoundEntityData()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cSoundEntityData::CreateFromFile(const tString &a_sFile)
	{
		tString sType;
		TiXmlDocument *pDoc = efeNew(TiXmlDocument, ());
		if (pDoc->LoadFile(a_sFile.c_str()) == false)
		{
			Error("Couldn't load '%s'!\n", a_sFile.c_str());
			efeDelete(pDoc);
			return false;
		}

		TiXmlElement *pRootElem = pDoc->FirstChildElement();

		TiXmlElement *pMainElem = pRootElem->FirstChildElement("MAIN");
		if (pMainElem == NULL)
		{
			Error("Couldn't find MAIN element in '%s'!\n", a_sFile.c_str());
			efeDelete(pDoc);
			return false;
		}

		m_sMainSound = cString::ToString(pMainElem->Attribute("MainSound"),"");
		m_sStartSound = cString::ToString(pMainElem->Attribute("StartSound"),"");
		m_sStopSound = cString::ToString(pMainElem->Attribute("StopSound"),"");

		TiXmlElement *pPropElem = pRootElem->FirstChildElement("PROPERTIES");
		if (pPropElem == NULL)
		{
			Error("Couldn't find PROPERTIES element in '%s'!\n", a_sFile.c_str());
			efeDelete(pDoc);
			return false;
		}
		m_bUse3D = cString::ToBool(pPropElem->Attribute("Use3D"),true);
		m_bLoop = cString::ToBool(pPropElem->Attribute("Loop"),true);
		m_bStream = cString::ToBool(pPropElem->Attribute("Stream"),true);

		m_bBlockable = cString::ToBool(pPropElem->Attribute("Blockable"),false);
		m_fBlockVolumeMul = cString::ToFloat(pPropElem->Attribute("BlockVolumeMul"),0.6f);

		m_fVolume = cString::ToFloat(pPropElem->Attribute("Volume"),1);
		m_fMaxDistance = cString::ToFloat(pPropElem->Attribute("MaxDistance"),1);
		m_fMinDistance = cString::ToFloat(pPropElem->Attribute("MinDistance"),1);

		m_bFadeStart = cString::ToBool(pPropElem->Attribute("FadeStart"),true);
		m_bFadeStop = cString::ToBool(pPropElem->Attribute("FadeStop"),true);

		m_fRandom = cString::ToFloat(pPropElem->Attribute("Random"),1);
		m_fInterval = cString::ToFloat(pPropElem->Attribute("Interval"),0);

		m_lPriority = cString::ToInt(pPropElem->Attribute("Priority"),0);
		
		efeDelete(pDoc);

		return true;
	}
}