#ifndef EFE_WORLD3D_H
#define EFE_WORLD3D_H

#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"

#include "scene/Entity3D.h"

#include "game/GameTypes.h"
#include "game/SaveGame.h"

class TiXmlElement;
namespace efe
{
	class cGraphics;
	class cResources;
	class cSound;
	class cPhysics;
	class cScene;
	class cSystem;
	class cAI;
	class cHaptic;

	class iCamera;
	class cCamera2D;
	class cNode3D;
	class cLight3DSpot;
	class cLight3DPoint;
	class iLight3D;
	class cImageEntity;
	class cParticleManager;
	class cParticleSytem3D;
	class iScript;
	class cPortalContainer;
	class iRenderableContainer;
	class cMeshEntity;
	class cMesh;
	class cTerrain;
	class cBillboard;
	class cBeam;
	class iPhysicsWorld;
	class cColliderEntity;
	class iPhysicsBody;
	class cSoundEntity;
	class cAINodeContainer;
	class cAStarHandler;
	class cAINodeGenerationParams;

	typedef std::list<iLight3D*> tLight3DList;
	typedef std::list<iLight3D*>::iterator tLight3DListIt;

	typedef std::list<cMeshEntity*> tMeshEntityList;
	typedef std::list<cMeshEntity*>::iterator tMeshEntityListIt;

	typedef std::list<cBillboard*> tBillboardList;
	typedef std::list<cBillboard*>::iterator tBillboardListIt;

	typedef std::list<cBeam*> tBeamList;
	typedef std::list<cBeam*>::iterator tBeamListIt;

	typedef std::list<cParticleSytem3D*> tParticleSytem3DList;
	typedef tParticleSytem3DList::iterator tParticleSytem3DListIt;

	typedef std::list<cColliderEntity*> tColliderEntityList;
	typedef std::list<cColliderEntity*>::iterator tColliderEntityListIt;

	typedef std::list<cSoundEntity*> tSoundEntityList;
	typedef std::list<cSoundEntity*>::iterator tSoundEntityListIt;

	typedef std::list<cAINodeContainer*> tAINodeContainerList;
	typedef std::list<cAINodeContainer*>::iterator tAINodeContainerListIt;

	typedef std::list<cAStarHandler*> tAStarHandlerList;
	typedef std::list<cAStarHandler*>::iterator tAStarHandlerListIt;

	typedef cSTLIterator<cMeshEntity*, tMeshEntityList, tMeshEntityListIt> cMeshEntityIterator;
	typedef cSTLIterator<cBillboard*, tBillboardList, tBillboardListIt> cBillboardIterator;
	typedef cSTLIterator<iLight3D*, tLight3DList, tLight3DListIt> cLight3DListIterator;
	typedef cSTLIterator<cParticleSytem3D*, tParticleSytem3DList, tParticleSytem3DListIt> cParticleSytem3DIterator;
	typedef cSTLIterator<cSoundEntity*, tSoundEntityList, tSoundEntityListIt> cSoundEntityIterator;
	typedef cSTLIterator<cBeam*, tBeamList, tBeamListIt> cBeamIterator;

	class cTempAiNode
	{
	public:
		cTempAiNode(const cVector3f &a_vPos, const tString &a_sName) : m_vPos(a_vPos), m_sName(a_sName){}
		cVector3f m_vPos;
		tString m_sName;
	};

	typedef std::list<cTempAiNode> tTempAiNodeList;
	typedef tTempAiNodeList::iterator tTempAiNodeListIt;

	class cTempNodeContainer
	{
	public:
		tString m_sName;
		tTempAiNodeList m_lstNodes;
	};

	typedef std::map<tString, cTempNodeContainer*> tTempNodeContainerMap;
	typedef tTempNodeContainerMap::iterator tTempNodeContainerMapIt;

	//--------------------------------------------------------------

	class cAreaEntity : public iSerializable
	{
		kSerializableClassInit(cAreaEntity)

	public:
		tString m_sName;
		tString m_sType;
		cMatrixf m_mtxTransform;
		cVector3f m_vSize;
	};

	typedef std::map<tString, cAreaEntity*> tAreaEntityMap;
	typedef tAreaEntityMap::iterator tAreaEntityMapIt;

	//--------------------------------------------------------------

	class cStartPosEntity : public iSerializable
	{
		kSerializableClassInit(cStartPosEntity)

	public:
		cStartPosEntity() {}
		cStartPosEntity(const tString &a_sName) : m_sName(a_sName) {}

		cMatrixf &GetWorldMatrix() {return m_mtxTransform;}
		cMatrixf &GetLocalMatrix() {return m_mtxTransform;}
		void SetMatrix(const cMatrixf &a_mtxTrans) {m_mtxTransform = a_mtxTrans;}

		tString &GetName() {return m_sName;}

		cMatrixf m_mtxTransform;
		tString m_sName;
	};

	typedef std::list<cStartPosEntity*> tStartPosEntityList;
	typedef tStartPosEntityList::iterator tStartPosEntityListIt;

	//--------------------------------------------------------------

	kSaveData_BaseClass(cWorld3D)
	{
		kSaveData_ClassInit(cWorld3D)
	public:
		cContainerList<cStartPosEntity> m_lstStartPos;
		cContainerList<cAreaEntity> m_lstAreaEntities;
		cContainerList<cScriptVar> m_lstScriptVars;

		virtual iSaveObject *CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);
		virtual int GetSaveCreatePrio();
	};

	//--------------------------------------------------------------

	class cWorld3D
	{
	public:
		cWorld3D(tString a_sName, cGraphics *a_pGraphics, cResources *a_pResources, cSound *a_pSound,
					cPhysics *a_pPhysics, cScene *a_pScene, cSystem *a_pSystem, cAI *a_pAI,
					cHaptic *a_pHaptic);
		~cWorld3D();

		tString GetName(){return m_sName;}

		void SetFileName(const tString &a_sFile){m_sFileName = a_sFile;}
		const tString &GetFileName(){return m_sFileName;}

		void Update(float a_fTimeStep);

		void PreUpdate(float a_fTotalTime, float a_fTimeStep);

		iScript *GetScript(){return m_pScript;}

		iRenderableContainer *GetRenderContainer();
		cPortalContainer *GetPortalContainer(){return m_pPortalContainer;}

		cPhysics *GetPhysics(){return m_pPhysics;}
		cResources *GetResources(){return m_pResources;}
		cSound *GetSound(){return m_pSound;}
		cSystem *GetSystem(){return m_pSystem;}

		iEntity3D *CreateEntity(const tString &a_sName, const cMatrixf &a_mtxTransform,
			const tString &a_sFile, bool a_bLoadReference);

		void SetUpData();

		///// PHYSICS ////////////////////////////////

		void SetPhysicsWorld(iPhysicsWorld *a_pWorld, bool a_bAutoDelete = true);
		iPhysicsWorld *GetPhysicsWorld();

		///// AREA ////////////////////////////////

		cAreaEntity *CreateAreaEntity(const tString &a_sName);
		cAreaEntity *GetAreaEntity(const tString &a_sName);
		tAreaEntityMap *GetAreaEntityMap() {return &m_mapAreaEntities;}


		///// MESH ENTITY METHODS ////////////////////////////////
		cMeshEntity *CreateMeshEntity(const tString &a_sName, cMesh *a_pMesh, bool a_bAddToContainer=true);

		void DrawMeshBoundingBoxes(const cColor &a_Color, bool a_bStatic);

		///// MESH ENTITY METHODS ////////////////////////////////
		cTerrain *CreateTerrain(cMesh *a_pTerrainMesh);

		///// LIGHT METHODS ////////////////////////////////

		cLight3DPoint *CreateLightPoint(const tString &a_sName = "", bool a_bAddToContainer = true);
		cLight3DSpot *CreateLightSpot(const tString &a_sName = "", const tString &a_sGobo = "",
			bool a_bAddToContainer = true);

		///// COLLIDER METHODS ////////////////////////////////

		cColliderEntity *CreateColliderEntity(const tString &a_sName, iPhysicsBody *a_pBody);
		void DestroyColliderEntity(cColliderEntity *a_pCollider);
		cColliderEntity *GetColliderEntity(const tString &a_sName);

		///// SOUND ENTITY METHODS ////////////////////////////////

		cSoundEntity *CreateSoundEntity(const tString &a_sName, const tString &a_sSoundEntity,
			bool a_bRemoveWhenOver);
		void DestroySoundEntity(cSoundEntity *a_pEntity);
		cSoundEntity *GetSoundEntity(const tString &a_sName);
		void DestroyAllSoundEntities();
		bool SoundEntityExists(cSoundEntity *a_pEntity);

		cSoundEntityIterator GetSoundEntityIterator(){return cSoundEntityIterator(&m_lstSoundEntities);}

		///// START POS ENTITY METHODS ////////////////////////////////

		cStartPosEntity *CreateStartPos(const tString &a_sName);
		cStartPosEntity *GetStartPosEntity(const tString &a_sName);
		cStartPosEntity *GetFirstStartPosEntity();

		///// AI NODE METHODS ////////////////////////////////

		tTempAiNodeList *GetAINodeList(const tString &a_sType);

	private:
		iSaveData *CreateSavedData();

		void UpdateEntities(float a_fTimeStep);
		void UpdateBodies(float a_fTimeStep);
		void UpdateLights(float a_fTimeStep);
		void UpdateSoundEntities(float a_fTimeStep);

		tString m_sName;
		tString m_sFileName;
		cGraphics *m_pGraphics;
		cSound *m_pSound;
		cResources *m_pResources;
		cPhysics *m_pPhysics;
		cScene *m_pScene;
		cSystem *m_pSystem;
		cAI *m_pAI;
		cHaptic *m_pHaptic;

		iPhysicsWorld *m_pPhysicsWorld;
		bool m_bAutoDeletePhysicsWorld;

		cTerrain *m_pTerrain;

		cVector3f m_vWorldSize;

		cPortalContainer *m_pPortalContainer;

		iScript *m_pScript;

		tLight3DList m_lstLights;
		tMeshEntityList m_lstMeshEntities;
		tColliderEntityList m_lstColliders;
		tSoundEntityList m_lstSoundEntities;
		tStartPosEntityList m_lstStartPosEntities;
		tAreaEntityMap m_mapAreaEntities;

		tTempNodeContainerMap m_mapTempNodes;

		cNode3D *m_pRootNode;

		tString m_sMapName;
		cColor m_AmbientColor;
	};
};
#endif