#include "scene/World3D.h"

#include "tinyxml.h"

#include "system/Script.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"

#include "math/Math.h"
#include "math/MathTypes.h"

#include "game/Game.h"

#include "graphics/Graphics.h"
#include "graphics/Renderer3D.h"

#include "resources/ScriptManager.h"
#include "resources/TextureManager.h"
#include "resources/FileSearcher.h"
#include "resources/Resources.h"
#include "resources/SoundEntityManager.h"

#include "scene/Scene.h"
#include "scene/Camera.h"
#include "scene/Node3D.h"
#include "scene/Light3DPoint.h"
#include "scene/Light3DSpot.h"
#include "scene/PortalContainer.h"
#include "scene/MeshEntity.h"
#include "scene/Terrain.h"
#include "scene/ColliderEntity.h"
#include "scene/SoundEntity.h"

#include "system/System.h"

#include "sound/SoundEntityData.h"
#include "sound/Sound.h"
#include "sound/SoundHandler.h"

#include "physics/Physics.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"

namespace efe
{
	cWorld3D::cWorld3D(tString a_sName, cGraphics *a_pGraphics, cResources *a_pResources, cSound *a_pSound,
					cPhysics *a_pPhysics, cScene *a_pScene, cSystem *a_pSystem, cAI *a_pAI,
					cHaptic *a_pHaptic)
	{
		m_pGraphics = a_pGraphics;
		m_pResources = a_pResources;
		m_pSound=a_pSound;
		m_pPhysics = a_pPhysics;
		m_pScene = a_pScene;

		m_pRootNode = efeNew(cNode3D,());

		m_pTerrain = NULL;

		m_pScript = NULL;

		m_sName = a_sName;

		m_AmbientColor = cColor(0,0);

		m_pPortalContainer = efeNew(cPortalContainer, ());

		m_pPhysicsWorld = NULL;
		m_bAutoDeletePhysicsWorld = false;

		m_sFileName = "";
	}

	cWorld3D::~cWorld3D()
	{
		STLDeleteAll(m_lstMeshEntities);
		STLDeleteAll(m_lstLights);

		STLDeleteAll(m_lstColliders);

		STLDeleteAll(m_lstStartPosEntities);
		STLMapDeleteAll(m_mapAreaEntities);

		if (m_pScript)
			m_pResources->GetScriptManager()->Destroy(m_pScript);

		if (m_pPhysicsWorld)
			m_pPhysics->DestroyWorld(m_pPhysicsWorld);

		STLDeleteAll(m_lstSoundEntities);

		efeDelete(m_pPortalContainer);

		efeDelete(m_pRootNode);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWorld3D::Update(float a_fTimeStep)
	{
		START_TIMING(Physics);
		if (m_pPhysicsWorld) m_pPhysicsWorld->Update(a_fTimeStep);
		STOP_TIMING(Physics);

		START_TIMING(Entities);
		UpdateEntities(a_fTimeStep);
		STOP_TIMING(Entities);

		START_TIMING(Bodies);
		UpdateBodies(a_fTimeStep);
		STOP_TIMING(Bodies);

		START_TIMING(SoundEntities);
		//UpdateICMRegKey(a_fTimeStep);
		STOP_TIMING(SoundEntities);
	}

	//--------------------------------------------------------------

	void cWorld3D::PreUpdate(float a_fTotalTime, float a_fTimeStep)
	{
		m_pSound->GetSoundHandler()->SetSilent(true);

		while (a_fTotalTime > 0)
		{
			if (m_pPhysicsWorld) m_pPhysicsWorld->Update(a_fTimeStep);
			//UpdateParticles(a_fTimeStep);

			a_fTotalTime -= a_fTimeStep;
		}

		m_pSound->GetSoundHandler()->SetSilent(false);
	}

	//--------------------------------------------------------------

	iRenderableContainer *cWorld3D::GetRenderContainer()
	{
		return m_pPortalContainer;
	}

	//--------------------------------------------------------------

	iEntity3D *cWorld3D::CreateEntity(const tString &a_sName, const cMatrixf &a_mtxTransform,
			const tString &a_sFile, bool a_bLoadReference)
	{
		iEntity3D *pEntity = NULL;
		tString sFileName = cString::SetFileExt(a_sFile, "ent");

		tString sPath = m_pResources->GetFileSearcher()->GetFilePath(sFileName);

		if (sPath != "")
		{
			TiXmlDocument *pEntityDoc = efeNew(TiXmlDocument, ());
			if (pEntityDoc->LoadFile(sPath.c_str()) == false)
				Error("Couldn't load '%s'\n", sPath.c_str());
			else
			{
				TiXmlElement *pRootElem = pEntityDoc->FirstChildElement();
				TiXmlElement *pMainElem = pRootElem->FirstChildElement("MAIN");

				tString sType = cString::ToString(pMainElem->Attribute("Type"), "");

				iEntity3DLoader *pLoader = m_pResources->GetEntity3DLoader(sType);

				if (pLoader)
				{
					pEntity = pLoader->Load(a_sName, pRootElem, a_mtxTransform, this, sFileName, a_bLoadReference);
					pEntity->SetSourceFile(sFileName);
				}
				else
					Error("Couldn't find loader for type '%s' in file '%s'\n", sType.c_str(), sFileName.c_str());
			}
			efeDelete(pEntityDoc);
		}
		else
			Error("Entity file '%s' was not dound!\n", sFileName.c_str());

		return pEntity;
	}

	//--------------------------------------------------------------

	static void CheckMinMaxUpdate(cVector3f &a_vMin, cVector3f &a_vMax,
		const cVector3f &a_vLocalMin, const cVector3f &a_vLocalMax)
	{
		if (a_vMin.x > a_vLocalMin.x) a_vMin.x = a_vLocalMin.x;
		if (a_vMax.x < a_vLocalMax.x) a_vMax.x = a_vLocalMax.x;

		if (a_vMin.y > a_vLocalMin.y) a_vMin.y = a_vLocalMin.y;
		if (a_vMax.y < a_vLocalMax.y) a_vMax.y = a_vLocalMax.y;

		if (a_vMin.z > a_vLocalMin.z) a_vMin.z = a_vLocalMin.z;
		if (a_vMax.z < a_vLocalMax.z) a_vMax.z = a_vLocalMax.z;
	}

	void cWorld3D::SetUpData()
	{
		m_pPortalContainer->Compile();

		tSectorMap *pSectorMap = m_pPortalContainer->GetSectorMap();

		if (pSectorMap->size() > 0)
		{
			tSectorMapIt SectorIt =  pSectorMap->begin();
			cSector *pSector = SectorIt->second;

			cVector3f vMin = pSector->GetBV()->GetMin();
			cVector3f vMax = pSector->GetBV()->GetMax();
			++SectorIt;
			for (; SectorIt != pSectorMap->end(); ++SectorIt)
			{
				pSector = SectorIt->second;

				cVector3f vLocalMin = pSector->GetBV()->GetMin();
				cVector3f vLocalMax = pSector->GetBV()->GetMax();

				CheckMinMaxUpdate(vMin, vMax, vLocalMin, vLocalMax);
			}

		}
	}

	//--------------------------------------------------------------

	void cWorld3D::SetPhysicsWorld(iPhysicsWorld *a_pWorld, bool a_bAutoDelete)
	{
		m_pPhysicsWorld = a_pWorld;
		m_bAutoDeletePhysicsWorld = a_bAutoDelete;
		if (m_pPhysicsWorld)
			m_pPhysicsWorld->SetWorld3D(this);
	}

	iPhysicsWorld *cWorld3D::GetPhysicsWorld()
	{
		return m_pPhysicsWorld;
	}

	//--------------------------------------------------------------

	cAreaEntity *cWorld3D::CreateAreaEntity(const tString &a_sName)
	{
		cAreaEntity *pArea = efeNew(cAreaEntity, ());
		pArea->m_sName = a_sName;
		m_mapAreaEntities.insert(tAreaEntityMap::value_type(a_sName, pArea));
		return pArea;
	}

	cAreaEntity *cWorld3D::GetAreaEntity(const tString &a_sName)
	{
		tAreaEntityMapIt it = m_mapAreaEntities.find(a_sName);
		if (it == m_mapAreaEntities.end()) return NULL;

		return it->second;
	}

	//--------------------------------------------------------------

	cMeshEntity *cWorld3D::CreateMeshEntity(const tString &a_sName, cMesh *a_pMesh, bool a_bAddToContainer)
	{
		cMeshEntity *pMesh = efeNew(cMeshEntity, (a_sName, a_pMesh, m_pResources->GetMaterialManager(),
			m_pResources->GetMeshManager(), m_pResources->GetAnimationManager()));
		m_lstMeshEntities.push_back(pMesh);

		if (a_bAddToContainer)
			m_pPortalContainer->Add(pMesh,false);

		pMesh->SetWorld(this);

		return pMesh;
	}

	//--------------------------------------------------------------

	void cWorld3D::DrawMeshBoundingBoxes(const cColor &a_Color, bool a_bStatic)
	{
		tMeshEntityListIt it = m_lstMeshEntities.begin();
		for (;it != m_lstMeshEntities.end();++it)
		{
			cMeshEntity *pEntity = *it;

			if (a_bStatic && pEntity->IsStatic()) continue;

			cBoundingVolume *pBV = pEntity->GetBoundingVolume();
			m_pGraphics->GetLowLevel()->DrawBoxMaxMin(pBV->GetMax(), pBV->GetMin(), a_Color);
		}
	}

	//--------------------------------------------------------------

	cTerrain *cWorld3D::CreateTerrain(cMesh *a_pTerrainMesh)
	{
		cTerrain *pTerrain = efeNew(cTerrain, (a_pTerrainMesh, m_pResources->GetMaterialManager(),
			m_pResources->GetMeshManager(), m_pResources->GetAnimationManager()));

		m_pPortalContainer->Add(pTerrain,false);

		pTerrain->SetWorld(this);

		return pTerrain;
	}

	//--------------------------------------------------------------

	cLight3DPoint *cWorld3D::CreateLightPoint(const tString &a_sName, bool a_bAddToContainer)
	{
		cLight3DPoint *pLight = efeNew(cLight3DPoint, (a_sName, m_pResources));
		m_lstLights.push_back(pLight);

		if (a_bAddToContainer)
			m_pPortalContainer->Add(pLight, false);

		pLight->SetWorld3D(this);

		return pLight;
	}

	//--------------------------------------------------------------

	cLight3DSpot *cWorld3D::CreateLightSpot(const tString &a_sName, const tString &a_sGobo,
			bool a_bAddToContainer)
	{
		cLight3DSpot *pLight = efeNew(cLight3DSpot, (a_sName, m_pResources));
		m_lstLights.push_back(pLight);

		if (a_sGobo != "")
		{
			iTexture *pTexture = m_pResources->GetTextureManager()->Create2D(a_sGobo, true);
			if (pTexture != NULL)
				pLight->SetTexture(pTexture);
			else
				Warning("Couldn't load texture '%s' for light '%s'\n", a_sGobo.c_str(), a_sName.c_str());
		}

		if (a_bAddToContainer)
			m_pPortalContainer->Add(pLight, false);

		pLight->SetWorld3D(this);

		return pLight;
	}

	//--------------------------------------------------------------

	cColliderEntity *cWorld3D::CreateColliderEntity(const tString &a_sName, iPhysicsBody *a_pBody)
	{
		cColliderEntity *pCollider = efeNew(cColliderEntity, (a_sName, a_pBody, m_pPhysicsWorld));

		m_lstColliders.push_back(pCollider);

		return pCollider;
	}
	void cWorld3D::DestroyColliderEntity(cColliderEntity *a_pCollider)
	{
		STLFindAndDelete(m_lstColliders, a_pCollider);
	}
	cColliderEntity *cWorld3D::GetColliderEntity(const tString &a_sName)
	{
		return (cColliderEntity*)STLFindByName(m_lstColliders, a_sName);
	}

	//--------------------------------------------------------------

	cSoundEntity *cWorld3D::CreateSoundEntity(const tString &a_sName, const tString &a_sSoundEntity,
			bool a_bRemoveWhenOver)
	{
		cSoundEntityData *pData = m_pResources->GetSoundEntityManager()->CreateSoundEntity(a_sSoundEntity);
		if (pData == NULL)
		{
			Error("Cannot find sound entity '%s'\n", a_sSoundEntity.c_str());
			return NULL;
		}
	
		cSoundEntity *pSound = efeNew(cSoundEntity, (a_sName, pData, m_pResources->GetSoundEntityManager(),
			this, m_pSound->GetSoundHandler(), a_bRemoveWhenOver));

		m_lstSoundEntities.push_back(pSound);

		return pSound;
	}

	//--------------------------------------------------------------

	void cWorld3D::DestroySoundEntity(cSoundEntity *a_pEntity)
	{
		tSoundEntityListIt it = m_lstSoundEntities.begin();
		for (; it != m_lstSoundEntities.end(); ++it)
		{
			cSoundEntity *pSound = *it;
			if (pSound == a_pEntity)
			{
				m_lstSoundEntities.erase(it);
				efeDelete(pSound);
				break;
			}
		}
	}

	//--------------------------------------------------------------

	cSoundEntity *cWorld3D::GetSoundEntity(const tString &a_sName)
	{
		return (cSoundEntity*)STLFindByName(m_lstSoundEntities, a_sName);
	}

	//--------------------------------------------------------------

	void cWorld3D::DestroyAllSoundEntities()
	{
		if (m_pPhysicsWorld)
		{
			cPhysicsBodyIterator bodyIt = m_pPhysicsWorld->GetBodyIterator();
			while (bodyIt.HasNext())
			{
				iPhysicsBody *pBody = bodyIt.Next();
				pBody->SetScrapeSoundEntity(NULL);
				pBody->SetRollSoundEntity(NULL);
			}

			cPhysicsJointIterator jointIt = m_pPhysicsWorld->GetJointIterator();
			while (jointIt.HasNext())
			{
				iPhysicsJoint *pJoint = jointIt.Next();
				//pJoint->SetSo(NULL);
			}
		}

		STLDeleteAll(m_lstSoundEntities);
		m_lstSoundEntities.clear();
	}

	//--------------------------------------------------------------

	bool cWorld3D::SoundEntityExists(cSoundEntity *a_pEntity)
	{
		tSoundEntityListIt it = m_lstSoundEntities.begin();
		tSoundEntityListIt end = m_lstSoundEntities.end();
		for (; it != end; ++it)
		{
			if (*it == a_pEntity) return true;
		}

		return false;
	}

	//--------------------------------------------------------------

	cStartPosEntity *cWorld3D::CreateStartPos(const tString &a_sName)
	{
		cStartPosEntity *pStartPos = efeNew(cStartPosEntity, (a_sName));

		m_lstStartPosEntities.push_back(pStartPos);

		return pStartPos;
	}

	cStartPosEntity *cWorld3D::GetStartPosEntity(const tString &a_sName)
	{
		return (cStartPosEntity*)STLFindByName(m_lstStartPosEntities, a_sName);
	}

	cStartPosEntity *cWorld3D::GetFirstStartPosEntity()
	{
		return m_lstStartPosEntities.front();
	}

	//--------------------------------------------------------------

	tTempAiNodeList *cWorld3D::GetAINodeList(const tString &a_sType)
	{
		cTempNodeContainer *pContainer = NULL;
		tTempNodeContainerMapIt it = m_mapTempNodes.find(a_sType);
		if (it!=m_mapTempNodes.end())
			pContainer = it->second;

		if (pContainer==NULL)
		{
			pContainer = efeNew(cTempNodeContainer, ());
			m_mapTempNodes.insert(tTempNodeContainerMap::value_type(a_sType, pContainer));
		}

		return &pContainer->m_lstNodes;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cWorld3D::UpdateEntities(float a_fTimeStep)
	{
		tMeshEntityListIt MeshIt = m_lstMeshEntities.begin();
		for (; MeshIt!=m_lstMeshEntities.end(); MeshIt++)
		{
			cMeshEntity *pEntity = *MeshIt;

			if (pEntity->IsActive())
				pEntity->UpdateLogic(a_fTimeStep);
		}
	}

	//--------------------------------------------------------------

	void cWorld3D::UpdateBodies(float a_fTimeStep)
	{
		
	}

	//--------------------------------------------------------------

	void cWorld3D::UpdateLights(float a_fTimeStep)
	{
		tLight3DListIt it = m_lstLights.begin();

		while (it != m_lstLights.end())
		{
			iLight3D *pLight = *it;

			if (pLight->IsActive()) pLight->UpdateLogic(a_fTimeStep);

			++it;
		}
	}

	//--------------------------------------------------------------

	void cWorld3D::UpdateSoundEntities(float a_fTimeStep)
	{
		tSoundEntityListIt it = m_lstSoundEntities.begin();

		while (it != m_lstSoundEntities.end())
		{
			cSoundEntity *pSound = *it;

			if (pSound->IsActive())
			{
				pSound->UpdateLogic(a_fTimeStep);
			}

			if (pSound->IsStopped() && pSound->GetRemoveWhenOver())
			{
				it = m_lstSoundEntities.erase(it);
				efeDelete(pSound);
			}
			else
			{
				it++;
			}
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	kBeginSerializeBase(cAreaEntity)
		kSerializeVar(m_sName, eSerializeType_String)
		kSerializeVar(m_sType, eSerializeType_String)
		kSerializeVar(m_mtxTransform, eSerializeType_Matrixf)
		kSerializeVar(m_vSize, eSerializeType_Vector3f)
	kEndSerialize()

	//--------------------------------------------------------------

	kBeginSerializeBase(cStartPosEntity)
		kSerializeVar(m_sName, eSerializeType_String)
		kSerializeVar(m_mtxTransform, eSerializeType_Matrixf)
	kEndSerialize()

	//--------------------------------------------------------------

	kBeginSerializeBase(cSaveData_cWorld3D)
		kSerializeClassContainer(m_lstStartPos, cStartPosEntity, eSerializeType_Class)
		kSerializeClassContainer(m_lstAreaEntities, cAreaEntity, eSerializeType_Class)
		kSerializeClassContainer(m_lstScriptVars, cScriptVar, eSerializeType_Class)
	kEndSerialize()

	iSaveObject *cSaveData_cWorld3D::CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		cWorld3D *pWorld = a_pGame->GetScene()->GetWorld3D();

		//Start pos
		cContainerListIterator<cStartPosEntity> StartIt = m_lstStartPos.GetIterator();
		while (StartIt.HasNext())
		{
			cStartPosEntity &tempStart = StartIt.Next();
			cStartPosEntity *pStart = pWorld->CreateStartPos(tempStart.GetName());
			pStart->SetMatrix(tempStart.GetWorldMatrix());
		}

		//Area entities
		cContainerListIterator<cAreaEntity> AreaIt = m_lstAreaEntities.GetIterator();
		while (AreaIt.HasNext())
		{
			cAreaEntity &tempArea = AreaIt.Next();
			cAreaEntity *pArea = pWorld->CreateAreaEntity(tempArea.m_sName);
			pArea->m_mtxTransform = tempArea.m_mtxTransform;
			pArea->m_sType = tempArea.m_sType;
			pArea->m_vSize = tempArea.m_vSize;
		}

		//Script vars
		cContainerListIterator<cScriptVar> VarIt = m_lstScriptVars.GetIterator();
		while (VarIt.HasNext())
		{
			cScriptVar &tempVar = VarIt.Next();
			cScriptVar *pVar = a_pGame->GetScene()->CreateLocalVar(tempVar.m_sName);
			pVar->m_lVal = tempVar.m_lVal;
		}

		return NULL;
	}

	//--------------------------------------------------------------

	int cSaveData_cWorld3D::GetSaveCreatePrio()
	{
		return 4;
	}

	//--------------------------------------------------------------

	iSaveData *cWorld3D::CreateSavedData()
	{
		cSaveData_cWorld3D *pData = efeNew(cSaveData_cWorld3D, ());

		//Start pos
		tStartPosEntityListIt StartIt = m_lstStartPosEntities.begin();
		for (; StartIt != m_lstStartPosEntities.end(); ++StartIt)
		{
			pData->m_lstStartPos.Add(*(*StartIt));
		}

		//Area entities
		tAreaEntityMapIt AreaIt = m_mapAreaEntities.begin();
		for (; AreaIt != m_mapAreaEntities.end(); ++AreaIt)
		{
			pData->m_lstAreaEntities.Add(*(AreaIt->second));
		}

		//Local scripts
		tScriptVarMap *pLocalVarMap = m_pScene->GetLocalVarMap();
		tScriptVarMapIt VarIt = pLocalVarMap->begin();
		for (; VarIt != pLocalVarMap->end(); ++VarIt)
		{
			pData->m_lstScriptVars.Add(VarIt->second);
		}

		return pData;
	}
}