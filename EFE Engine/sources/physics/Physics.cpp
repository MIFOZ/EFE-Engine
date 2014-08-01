#include "physics/Physics.h"
#include "physics/LowLevelPhysics.h"

#include "physics/PhysicsWorld.h"
#include "physics/SurfaceData.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "impl/tinyXML/tinyxml.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPhysics::cPhysics(iLowLevelPhysics *a_pLowLevelPhysics) : iUpdateable("EFE_Physics")
	{
		m_pLowLevelPhysics = a_pLowLevelPhysics;

		m_pGameWorld = NULL;

		m_lMaxImpacts = 6;
		m_fImpactDuration = 0.4f;

		m_bLog = false;
	}

	//--------------------------------------------------------------

	cPhysics::~cPhysics()
	{
		Log("Exitin' Physics Module\n");
		Log("-----------------------------------------------------\n");

		STLDeleteAll(m_lstWorlds);
		STLMapDeleteAll(m_mapSurfaceData);

		Log("-----------------------------------------------------\n\n");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cPhysics::Init(cResources *a_pResources)
	{
		m_pResources = a_pResources;
	}

	//--------------------------------------------------------------

	void cPhysics::Update(float a_fTimeStep)
	{
		UpdateImpactCounts(a_fTimeStep);
	}

	//--------------------------------------------------------------

	iPhysicsWorld *cPhysics::CreateWorld(bool a_bAddSurfaceData)
	{
		iPhysicsWorld *pWorld = m_pLowLevelPhysics->CreateWorld();
		m_lstWorlds.push_back(pWorld);

		if (a_bAddSurfaceData)
		{
			tSurfaceDataMapIt it = m_mapSurfaceData.begin();
			for (; it != m_mapSurfaceData.end(); ++it)
			{
				cSurfaceData *pData = it->second;
				pData->ToMaterial(pWorld);
			}
		}

		return pWorld;
	}

	//--------------------------------------------------------------

	void cPhysics::DestroyWorld(iPhysicsWorld *a_pWorld)
	{
		STLFindAndDelete(m_lstWorlds, a_pWorld);
	}

	//--------------------------------------------------------------

	cSurfaceData *cPhysics::CreateSurfaceData(const tString &a_sName)
	{
		cSurfaceData *pData = efeNew(cSurfaceData, (a_sName, this, m_pResources));

		m_mapSurfaceData.insert(tSurfaceDataMap::value_type(a_sName, pData));

		return pData;
	}

	cSurfaceData *cPhysics::GetSurfaceData(const tString &a_sName)
	{
		tSurfaceDataMapIt it = m_mapSurfaceData.find(a_sName);
		if (it == m_mapSurfaceData.end()) return NULL;

		return it->second;
	}

	//--------------------------------------------------------------

	bool cPhysics::LoadSurfaceData(const tString &a_sFile, cHaptic *a_pHaptic)
	{
		TiXmlDocument *pXmlDoc = efeNew(TiXmlDocument, (a_sFile.c_str()));
		if (pXmlDoc->LoadFile() == false)
		{
			Error("Couldn't load XML file '%s'!\n", a_sFile.c_str());
			efeDelete(pXmlDoc);
			return false;
		}

		TiXmlElement *pRootElem = pXmlDoc->RootElement();

		TiXmlElement *pChildElem = pRootElem->FirstChildElement("Material");
		for (; pChildElem != NULL; pChildElem = pChildElem->NextSiblingElement("Material"))
		{
			tString sName = cString::ToString(pChildElem->Attribute("Name"), "");
			if (sName == "") continue;

			cSurfaceData *pData = CreateSurfaceData(sName);

			//Get properties
			pData->SetElasticity(cString::ToFloat(pChildElem->Attribute("Elasticity"), 0.5f));
			pData->SetKineticFriction(cString::ToFloat(pChildElem->Attribute("KineticFriction"), 0.3f));
			pData->SetStaticFriction(cString::ToFloat(pChildElem->Attribute("StaticFriction"), 0.3f));

			pData->SetPriority(cString::ToInt(pChildElem->Attribute("Priority"), 0));

			pData->SetElasticityCombMode(GetCombMode(pChildElem->Attribute("ElasticityMode")));
			pData->SetFrictionCombMode(GetCombMode(pChildElem->Attribute("FrictionMode")));

			pData->SetStepType(cString::ToString(pChildElem->Attribute("StepType"), ""));

			pData->SetMinScrapeSpeed(cString::ToFloat(pChildElem->Attribute("MinScrapeSpeed"), 0.7f));
			pData->SetMinScrapeFreq(cString::ToFloat(pChildElem->Attribute("MinScrapeFreq"), 0.7f));
			pData->SetMinScrapeFreqSpeed(cString::ToFloat(pChildElem->Attribute("MinScrapeFreqSpeed"), 0.7f));
			pData->SetMaxScrapeFreq(cString::ToFloat(pChildElem->Attribute("MaxScrapeFreq"), 2));
			pData->SetMaxScrapeFreqSpeed(cString::ToFloat(pChildElem->Attribute("MaxScrapeFreqSpeed"), 3));
			pData->SetMiddleScrapeSpeed(cString::ToFloat(pChildElem->Attribute("MiddleScrapeSpeed"), 1.2f));
			pData->SetMinScrapeContacts(cString::ToInt(pChildElem->Attribute("MinScrapeContacts"), 4));
			pData->SetScrapeSoundName(cString::ToString(pChildElem->Attribute("ScrapeSoundName"), ""));

			pData->SetMinRollSpeed(cString::ToFloat(pChildElem->Attribute("MinRollSpeed"), 0.7f));
			pData->SetMinRollFreq(cString::ToFloat(pChildElem->Attribute("MinRollFreq"), 0.7f));
			pData->SetMinRollVolume(cString::ToFloat(pChildElem->Attribute("MinRollVolume"), 0.7f));
			pData->SetMinRollFreqSpeed(cString::ToFloat(pChildElem->Attribute("MinRollFreqSpeed"), 0.7f));
			pData->SetMaxRollFreq(cString::ToFloat(pChildElem->Attribute("MaxRollFreq"), 2));
			pData->SetMaxRollVolume(cString::ToFloat(pChildElem->Attribute("MaxRollVolume"), 2));
			pData->SetMaxRollFreqSpeed(cString::ToFloat(pChildElem->Attribute("MaxRollFreqSpeed"), 3));
			pData->SetMiddleRollSpeed(cString::ToFloat(pChildElem->Attribute("MiddleRollSpeed"), 1.2f));
			pData->SetRollSoundName(cString::ToString(pChildElem->Attribute("RollSoundName"), ""));

			//Axes
			tString sAxisVec = cString::ToString(pChildElem->Attribute("RollAxis"), "");
			tStringVec vAxes;
			tFlag axisFlags = 0;
			cString::GetStringVec(sAxisVec, vAxes);
			for (size_t i=0; i<vAxes.size(); ++i)
			{
				tString sAxis = cString::ToLowerCase(vAxes[i]);
				if (sAxis == "x")		axisFlags |= eRollFlagAxis_X;
				else if (sAxis == "y")		axisFlags |= eRollFlagAxis_Y;
				else if (sAxis == "z")		axisFlags |= eRollFlagAxis_Z;
			}
			pData->SetRollAxisFlags(axisFlags);

			//Get Impact data
			TiXmlElement *pImpactElem = pChildElem->FirstChildElement("Impact");
			for (; pImpactElem != NULL; pImpactElem = pImpactElem->NextSiblingElement("Impact"))
			{
				float fMinSpeed = cString::ToFloat(pImpactElem->Attribute("MinSpeed"), 1);

				cSurfaceImpactData *pImpactData = pData->CreateImpactData(fMinSpeed);

				pImpactData->SetSoundName(cString::ToString(pImpactElem->Attribute("SoundName"), ""));
				pImpactData->SetPSName(cString::ToString(pImpactElem->Attribute("PSName"), ""));
				pImpactData->SetPSPrio(cString::ToInt(pImpactElem->Attribute("PSPrio"), 10));
			}

			//Get Hit data
			TiXmlElement *pHitElem = pChildElem->FirstChildElement("Hit");
			for (; pImpactElem != NULL; pImpactElem = pImpactElem->NextSiblingElement("Hit"))
			{
				float fMinSpeed = cString::ToFloat(pHitElem->Attribute("MinSpeed"), 1);

				cSurfaceImpactData *pHitData = pData->CreateHitData(fMinSpeed);

				pHitData->SetSoundName(cString::ToString(pHitElem->Attribute("SoundName"), ""));
				pHitData->SetPSName(cString::ToString(pHitElem->Attribute("PSName"), ""));
				pHitData->SetPSPrio(cString::ToInt(pHitElem->Attribute("PSPrio"), 10));
			}
		}

		efeDelete(pXmlDoc);

		return true;
	}

	//--------------------------------------------------------------

	bool cPhysics::CanPlayImpact()
	{
		if ((int)m_lstImpactCounts.size() >= m_lMaxImpacts) return false;
		else return true;
	}

	void cPhysics::AddImpact()
	{
		m_lstImpactCounts.push_back(cPhysicsImpactCount());
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	ePhysicsMaterialCombMode cPhysics::GetCombMode(const char *a_pName)
	{
		if (a_pName == NULL) return ePhysicsMaterialCombMode_Average;

		tString sMode = cString::ToLowerCase(a_pName);

		if (sMode == "average") return ePhysicsMaterialCombMode_Average;
		if (sMode == "min") return ePhysicsMaterialCombMode_Min;
		if (sMode == "max") return ePhysicsMaterialCombMode_Max;
		if (sMode == "multiply") return ePhysicsMaterialCombMode_Multiply;

		return ePhysicsMaterialCombMode_Average;
	}

	//--------------------------------------------------------------

	void cPhysics::UpdateImpactCounts(float a_fTimeStep)
	{
		tPhysicsImpactCountListIt it = m_lstImpactCounts.begin();
		while (it != m_lstImpactCounts.end())
		{
			it->m_fCount += a_fTimeStep;
			if (it->m_fCount >  m_fImpactDuration)
				it = m_lstImpactCounts.erase(it);
			else
				++it;
		}
	}
}