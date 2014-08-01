#include "ai/AINodeGenerator.h"

#include "scene/World3D.h"

#include "system/LowLevelSystem.h"
#include "system/System.h"
#include "system/String.h"

#include "resources/Resources.h"
#include "resources/FileSearcher.h"

#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"

#include "impl/tinyXML/tinyxml.h"

namespace efe
{
	cAINodeGeneratorParams::cAINodeGeneratorParams()
	{
		m_sNodeType = "node";

		m_fHeightFromGround = 0.1f;
		m_fMinWallDist = 0.4f;

		m_vMinPos = cVector3f(-10000,-10000,-10000);
		m_vMaxPos = cVector3f(10000,10000,10000);

		m_fGridSize = 0.4f;
	}

	class cCollideRayCallback : public iPhysicsRayCallback
	{
	public:
		bool OnIntersect(iPhysicsBody *a_pBody, cPhysicsRayParams *a_pParams)
		{
			if (a_pBody->GetMass()!=0) return true;

			m_bIntersected = true;
			m_vPos = a_pParams->m_vPoint;
			m_fDist = a_pParams->m_fDist;

			return false;
		}

		bool m_bIntersected;
		cVector3f m_vPos;
		float m_fDist;
	};

	cAINodeGenerator::cAINodeGenerator()
	{
	}

	cAINodeGenerator::~cAINodeGenerator()
	{
	}

	static cCollideRayCallback g_CollideRayCallback;

	void cAINodeGenerator::Generate(cWorld3D *a_pWorld, cAINodeGeneratorParams *a_pParams)
	{
		m_pWorld = a_pWorld;
		m_pParams = a_pParams;

		iPhysicsWorld *pPhysicsWorld = a_pWorld->GetPhysicsWorld();

		bool m_bLoadFromFile = false;

		cSystem *pSystem = a_pWorld->GetSystem();
		cResources *pResources = a_pWorld->GetResources();
		cFileSearcher *pFileSearcher = pResources->GetFileSearcher();

		m_pNodeList = a_pWorld->GetAINodeList(m_pParams->m_sNodeType);

		if (m_pWorld->GetFileName() != "")
		{
			tString sPath = pFileSearcher->GetFilePath(m_pWorld->GetFileName());
			tWString sSaveFile = cString::To16Char(cString::SetFileExt(sPath, "ainodes"));

			if (sPath != "" && FileExists(sSaveFile))
			{
				cDate mapDate = FileModifiedDate(cString::To16Char(sPath));
				cDate saveDate = FileModifiedDate(sSaveFile);

				if (saveDate > mapDate)
				{
					LoadFromFile();
					return;
				}
			}
		}

		cPhysicsBodyIterator it = pPhysicsWorld->GetBodyIterator();
		cVector3f vWorldMax(-10000,-10000,-10000);
		cVector3f vWorldMin(10000,10000,10000);
		
		while(it.HasNext())
		{
			iPhysicsBody *pBody = it.Next();

			cVector3f vMin = pBody->GetBV()->GetMin();
			cVector3f vMax = pBody->GetBV()->GetMax();

			//X
			if (vWorldMin.x > vMin.x) vWorldMin.x = vMin.x;
			if (vWorldMax.x < vMax.x) vWorldMax.x = vMax.x;

			//Y
			if (vWorldMin.y > vMin.y) vWorldMin.y = vMin.y;
			if (vWorldMax.y < vMax.y) vWorldMax.y = vMax.y;


			//Z
			if (vWorldMin.z > vMin.z) vWorldMin.z = vMin.z;
			if (vWorldMax.z < vMax.z) vWorldMax.z = vMax.z;
		}

		vWorldMin.x += m_pParams->m_fGridSize;
		vWorldMin.z += m_pParams->m_fGridSize;
		vWorldMax.x -= m_pParams->m_fGridSize;
		vWorldMax.z -= m_pParams->m_fGridSize;

		if (vWorldMin.x < m_pParams->m_vMinPos.x) vWorldMin.x = m_pParams->m_vMinPos.x;
		if (vWorldMax.x > m_pParams->m_vMaxPos.x) vWorldMax.x = m_pParams->m_vMaxPos.x;

		if (vWorldMin.y < m_pParams->m_vMinPos.y) vWorldMin.y = m_pParams->m_vMinPos.y;
		if (vWorldMax.y > m_pParams->m_vMaxPos.y) vWorldMax.y = m_pParams->m_vMaxPos.y;

		if (vWorldMin.z < m_pParams->m_vMinPos.z) vWorldMin.z = m_pParams->m_vMinPos.z;
		if (vWorldMax.z > m_pParams->m_vMaxPos.z) vWorldMax.z = m_pParams->m_vMaxPos.z;

		cVector3f vPos(vWorldMin.x,0,vWorldMin.z);

		while (vPos.z <= vWorldMax.z)
		{
			cVector3f vStart(vPos.x, vWorldMax.y, vPos.z);
			cVector3f vEnd(vPos.x, vWorldMin.y, vPos.z);

			pPhysicsWorld->CastRay(this, vStart, vEnd, false, false, true);

			vPos.x += a_pParams->m_fGridSize;
			if (vPos.x > vWorldMax.x)
			{
				vPos.x = vWorldMin.x;
				vPos.z =+ a_pParams->m_fGridSize;
			}
		}

		cVector3f vEnds[4] = {
			cVector3f(m_pParams->m_fMinWallDist,0,0),
			cVector3f(-m_pParams->m_fMinWallDist,0,0),
			cVector3f(0,0,m_pParams->m_fMinWallDist),
			cVector3f(0,0,-m_pParams->m_fMinWallDist)
		};

		cVector3f vPushBackDirs[4] = 
		{
			cVector3f(-1,0,0),
			cVector3f(1,0,0),
			cVector3f(0,0,-1),
			cVector3f(0,0,1)
		};

		tTempAiNodeListIt nodeIt = m_pNodeList->begin();
		for (; nodeIt != m_pNodeList->end(); ++nodeIt)
		{
			cTempAiNode &Node = *nodeIt;

			for (int i=0;i<4;i++)
			{
				g_CollideRayCallback.m_bIntersected = false;
				pPhysicsWorld->CastRay(&g_CollideRayCallback,Node.m_vPos,Node.m_vPos + vEnds[i],true,false,true);

				if (g_CollideRayCallback.m_bIntersected)
				{
					if (g_CollideRayCallback.m_fDist < a_pParams->m_fMinWallDist)
						Node.m_vPos += vPushBackDirs[i] * (m_pParams->m_fMinWallDist - g_CollideRayCallback.m_fDist);
				}
			}
		}

		SaveToFile();
	}

	bool cAINodeGenerator::OnIntersect(iPhysicsBody *a_pBody, cPhysicsRayParams *a_pParams)
	{
		if (a_pBody->GetMass() != 0) return true;

		iPhysicsWorld *pPhysicsWorld = m_pWorld->GetPhysicsWorld();

		cVector3f vPosition = a_pParams->m_vPoint + cVector3f(0,m_pParams->m_fHeightFromGround,0);

		m_pNodeList->push_back(cTempAiNode(vPosition, ""));

		return true;
	}

	void cAINodeGenerator::SaveToFile()
	{
		if (m_pWorld->GetFileName()=="") return;

		cSystem *pSystem = m_pWorld->GetSystem();
		cResources *pResources = m_pWorld->GetResources();
		cFileSearcher *pFileSearcher = pResources->GetFileSearcher();

		tString sMapPath = pFileSearcher->GetFilePath(m_pWorld->GetFileName());
		tString sSaveFile = cString::SetFileExt(sMapPath, "ainodes");

		TiXmlDocument *pXmlDoc = efeNew(TiXmlDocument, (sSaveFile.c_str()));

		TiXmlElement *pRootElem = static_cast<TiXmlElement*>(pXmlDoc->InsertEndChild(TiXmlElement("AiNodes")));

		tTempAiNodeListIt nodeIt = m_pNodeList->begin();
		for (; nodeIt != m_pNodeList->end(); ++nodeIt)
		{
			cTempAiNode &Node = *nodeIt;
			TiXmlElement *pNodeElem = static_cast<TiXmlElement*>(pRootElem->InsertEndChild(TiXmlElement("Node")));

			tString sPos =	cString::ToString(Node.m_vPos.x)+" "+
							cString::ToString(Node.m_vPos.y)+" "+
							cString::ToString(Node.m_vPos.z);
			pNodeElem->SetAttribute("Pos", sPos.c_str());
			pNodeElem->SetAttribute("Name", Node.m_sName.c_str());
		}

		if (pXmlDoc->SaveFile() == false)
			Error("Couldn't save XML file %s\n", sSaveFile.c_str());

		efeDelete(pXmlDoc);
	}

	void cAINodeGenerator::LoadFromFile()
	{
		if (m_pWorld->GetFileName()=="") return;

		cSystem *pSystem = m_pWorld->GetSystem();
		cResources *pResources = m_pWorld->GetResources();
		cFileSearcher *pFileSearcher = pResources->GetFileSearcher();

		tString sMapPath = pFileSearcher->GetFilePath(m_pWorld->GetFileName());
		tString sSaveFile = cString::SetFileExt(sMapPath, "ainodes");

		TiXmlDocument *pXmlDoc = efeNew(TiXmlDocument, (sSaveFile.c_str()));
		if (pXmlDoc->LoadFile() == false)
		{
			Warning("Couldn't open XML file %s\n", sSaveFile.c_str());
			efeDelete(pXmlDoc);
			return;
		}

		TiXmlElement *pRootElem = pXmlDoc->RootElement();

		TiXmlElement *pNodeElem = pRootElem->FirstChildElement("Node");
		for (; pNodeElem != NULL; pNodeElem = pNodeElem->NextSiblingElement("Node"))
		{
			cVector3f vPos = cString::ToVector3f(pNodeElem->Attribute("Pos"),0);
			tString sName = cString::ToString(pNodeElem->Attribute("Name"),"");

			m_pNodeList->push_back(cTempAiNode(vPos, sName));
		}

		efeDelete(pXmlDoc);
	}
}