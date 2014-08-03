#include "scene/MeshEntity.h"

#include "resources/Resources.h"
#include "resources/MeshManager.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "graphics/RenderList.h"

#include "resources/MeshLoaderHandler.h"
#include "resources/FileSearcher.h"

#include "graphics/Animation.h"
#include "graphics/AnimationTrack.h"
#include "graphics/BoneState.h"

#include "scene/AnimationState.h"
#include "scene/Scene.h"
#include "scene/World3D.h"
#include "scene/SoundEntity.h"

#include "physics/PhysicsBody.h"
#include "physics/PhysicsWorld.h"

#include "math/Math.h"

#include "game/Game.h"

namespace efe
{

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMeshEntity::cMeshEntity(const tString a_sName, cMesh *a_pMesh, cMaterialManager *a_pMaterialManager,
		cMeshManager *a_pMeshManager, cAnimationManager *a_pAnimationManager) :
	iRenderable(a_sName)
	{
		m_pMaterialManager = a_pMaterialManager;
		m_pMeshManager = a_pMeshManager;
		m_pAnimationManager = a_pAnimationManager;

		m_pWorld = NULL;

		m_pCallback = NULL;

		m_bCastShadows = false;

		m_pMesh = a_pMesh;

		m_pRootNode = NULL;
		m_pRootCallback = NULL;

		m_pBody = NULL;

		m_bSkeletonPhysics = false;
		m_fSkeletonPhysicsWeight = 1.0f;
		m_bSkeletonPhysicsFading = false;
		m_fSkeletonPhysicsFadeSpeed = 1.0f;

		m_bSkeletonPhysicsSleeping = false;
		m_bSkeletonPhysicsCanSleep = true;

		m_bSkeletonColliders = false;

		m_bUpdatedBones = false;

		m_lStartSleepCount = 0;
		m_lUpdateCount = 0;
		m_fTimeStepAccum = 0;

		for (int i = 0; i < m_pMesh->GetSubMeshNum(); i++)
		{
			cSubMesh *pSubMesh = m_pMesh->GetSubMesh(i);
			cSubMeshEntity *pSub = efeNew(cSubMeshEntity, (pSubMesh->GetName(), this, pSubMesh, m_pMaterialManager));

			m_vSubMeshes.push_back(pSub);
			m_mapSubMeshes.insert(tSubMeshEntityMap::value_type(m_pMesh->GetSubMesh(i)->GetName(), pSub));

			iVertexBuffer *pVtxBuffer  = m_pMesh->GetSubMesh(i)->GetVertexBuffer();

			if (m_pMesh->GetNodeNum()<=0)
			{
				m_BoundingVolume.AddArrayPoints(pVtxBuffer->GetArray(eVertexFlag_Position),
					pVtxBuffer->GetVertexNum());
			}
			else
			{
				pSub->m_BoundingVolume.AddArrayPoints(pVtxBuffer->GetArray(eVertexFlag_Position),
					pVtxBuffer->GetVertexNum());
				pSub->m_BoundingVolume.CreateFromPoints(kvVertexElements[cMath::Log2ToInt(eVertexFlag_Position)]);
			}
		}

		if (m_pMesh->GetNodeNum() <= 0)
			m_BoundingVolume.CreateFromPoints(kvVertexElements[cMath::Log2ToInt(eVertexFlag_Position)]);

		//////////////////////////////////////////////////////////////////////////
		// Create animation states
		m_vAnimationStates.reserve(m_pMesh->GetAnimationNum());
		for (int i=0; i<m_pMesh->GetAnimationNum(); i++)
		{
			cAnimation *pAnimation = m_pMesh->GetAnimation(i);

			cAnimationState *pAnimState = efeNew(cAnimationState, (pAnimation, pAnimation->GetName(), NULL));

			m_vAnimationStates.push_back(pAnimState);

			tAnimationStateIndexMap::value_type value(pAnimState->GetName(), (int)m_vAnimationStates.size()-1);
			m_mapAnimationStateIndices.insert(value);
		}

		//////////////////////////////////////////////////////////////////////////
		// Create nodes
		m_bHasNodes = false;
		if (m_pMesh->GetNodeNum() >0 &&
			(m_pMesh->GetPhysicsJointNum()<=0 || m_pMesh->GetSkeleton()==NULL) && 
			m_pMesh->GetAnimationNum() > 0)
		{
			m_bHasNodes = true;
			m_vNodeStates.reserve(m_pMesh->GetNodeNum());

			m_pRootNode = efeNew(cNode3D, ("NodeRoot", false));

			m_pRootCallback = efeNew(cMeshEntityRootNodeUpdate, ());
			this->AddCallback(m_pRootCallback);

			for (int i=0; i<m_pMesh->GetNodeNum(); i++)
			{
				cNode3D *pMeshNode = m_pMesh->GetNode(i);

				cBoneState *pNode = efeNew(cBoneState, (pMeshNode->GetName(), false));
				pNode->SetMatrix(pMeshNode->GetLocalMatrix());

				m_vNodeStates.push_back(pNode);
				m_mapNodeStateIndices.insert(tNodeStateIndexMap::value_type(pNode->GetName(), i));

				cSubMeshEntity *pSubEntity = GetSubMeshEntityName(pMeshNode->GetSource());
				if (pSubEntity)
					pSubEntity->SetLocalNode(pNode);
			}

			for (int i=0;i<(int)m_vNodeStates.size();i++)
			{
				cNode3D *pStateNode = m_vNodeStates[i];
				cNode3D *pMeshNode = m_pMesh->GetNode(i);

				if (pMeshNode->GetParent())
				{
					cNode3D *pParentNode = GetNodeStateFromName(pMeshNode->GetParent()->GetName());
					if (pParentNode)
						pStateNode->SetParent(pParentNode);
					else
						pStateNode->SetParent(m_pRootNode);
				}
				else
					pStateNode->SetParent(m_pRootNode);

				cNodeIterator it = pMeshNode->GetChildIterator();
				while(it.HasNext())
				{
					cNode3D *pChildNode = static_cast<cNode3D*>(it.Next());
					pStateNode->AddChild(GetNodeStateFromName(pChildNode->GetName()));
				}
			}

			m_pRootNode->SetMatrix(cMatrixf::Identity);

			UpdateBVFromSubs();
		}

		if (m_bHasNodes == false)
		{
			for (size_t i=0; i<m_vSubMeshes.size(); ++i)
				AddChild(m_vSubMeshes[i]);
		}
	}

	cMeshEntity::~cMeshEntity()
	{
		for (tEntity3DListIt it = m_lstAttachedEntities.begin(); it != m_lstAttachedEntities.end();++it)
		{
			iEntity3D *pEntity = *it;
		}

		for (int i=0; i < (int)m_vSubMeshes.size(); i++)
		{
			efeDelete(m_vSubMeshes[i]);
		}

		if (m_pRootNode) efeDelete(m_pRootNode);
		if (m_pRootCallback) efeDelete(m_pRootCallback);

		m_pMeshManager->Destroy(m_pMesh);

		STLDeleteAll(m_vNodeStates);
		//STLDeleteAll(m_vBo);

		//STLDeleteAll(m_vTemp
	}

	void cMeshEntityRootNodeUpdate::OnTransformUpdate(iEntity3D *a_pEntity)
	{
		cMeshEntity *pMeshEntity = static_cast<cMeshEntity*>(a_pEntity);

		pMeshEntity->m_pRootNode->SetMatrix(a_pEntity->GetWorldMatrix());
	}

	void cMeshEntity::SetCastsShadows(bool a_bX)
	{
		if (a_bX == m_bCastShadows) return;

		m_bCastShadows = a_bX;
		for (int i=0; i<(int)m_vSubMeshes.size(); i++)
			m_vSubMeshes[i]->SetCastsShadows(a_bX);
	}

	cSubMeshEntity *cMeshEntity::GetSubMeshEntity(unsigned int a_lIdx)
	{
		if (a_lIdx >= m_vSubMeshes.size()) return NULL;

		return m_vSubMeshes[a_lIdx];
	}

	cSubMeshEntity *cMeshEntity::GetSubMeshEntityName(const tString &a_sName)
	{
		tSubMeshEntityMapIt it = m_mapSubMeshes.find(a_sName);
		if (it==m_mapSubMeshes.end()) return NULL;

		return it->second;
	}

	int cMeshEntity::GetSubMeshEntityNum()
	{
		return (int)m_vSubMeshes.size();
	}

	//--------------------------------------------------------------

	cAnimationState *cMeshEntity::AddAnimation(cAnimation *a_pAnimation, const tString &a_sName, float a_fBaseSpeed)
	{
		cAnimationState *pAnimState = efeNew(cAnimationState, (a_pAnimation, a_sName, m_pAnimationManager));

		pAnimState->SetBaseSpeed(a_fBaseSpeed);

		m_vAnimationStates.push_back(pAnimState);

		tAnimationStateIndexMap::value_type value(pAnimState->GetName(), (int)m_vAnimationStates.size() - 1);
		m_mapAnimationStateIndices.insert(value);

		return pAnimState;
	}

	void cMeshEntity::ClearAnimations()
	{
		STLDeleteAll(m_vAnimationStates);
	}

	//--------------------------------------------------------------

	cAnimationState *cMeshEntity::GetAnimationState(int a_lIndex)
	{
		return m_vAnimationStates[a_lIndex];
	}
	int cMeshEntity::GetAnimationStateIndex(const tString &a_sName)
	{
		tAnimationStateIndexMapIt it = m_mapAnimationStateIndices.find(a_sName);
		if (it != m_mapAnimationStateIndices.end())
			return it->second;
		else
			return -1;
	}
	cAnimationState *cMeshEntity::GetAnimationStateFromName(const tString &a_sName)
	{
		int lIdx = GetAnimationStateIndex(a_sName);
		if (lIdx >= 0)
			return m_vAnimationStates[lIdx];
		else
			return NULL;

	}
	int cMeshEntity::GetAnimationStateNum()
	{
		return m_vAnimationStates.size();
	}

	//--------------------------------------------------------------

	void cMeshEntity::Play(int a_lIndex, bool a_bLoop, bool a_bStopPrev)
	{
		if (a_bStopPrev) Stop();

		m_vAnimationStates[a_lIndex]->SetActive(true);
		m_vAnimationStates[a_lIndex]->SetTimePosition(0);
		m_vAnimationStates[a_lIndex]->SetLoop(a_bLoop);
		m_vAnimationStates[a_lIndex]->SetWeight(1);
	}

	void cMeshEntity::PlayName(const tString &a_sName, bool a_bLoop, bool a_bStopPrev)
	{
		int lIdx = GetAnimationStateIndex(a_sName);
		if (lIdx >= 0)
			Play(lIdx, a_bLoop, a_bStopPrev);
		else
			Warning("Can not find animation '%s' in meshentity '%s'\n", a_sName.c_str(), m_sName.c_str());
	}

	void cMeshEntity::Stop()
	{
		for (size_t i=0; i<m_vAnimationStates.size(); i++)
		{
			m_vAnimationStates[i]->SetActive(false);
			m_vAnimationStates[i]->SetTimePosition(0);
		}
	}

	//--------------------------------------------------------------

	cBoneState *cMeshEntity::GetBoneState(int a_lIndex)
	{
		return m_vBoneStates[a_lIndex];
	}

	int cMeshEntity::GetBoneStateIndex(const tString &a_sName)
	{
		tNodeStateIndexMapIt it = m_mapNodeStateIndices.find(a_sName);
		if (it != m_mapNodeStateIndices.end())
			return it->second;
		else
			return -1;
	}

	cBoneState *cMeshEntity::GetBoneStateFromName(const tString &a_sName)
	{
		int lIdx = GetBoneStateIndex(a_sName);
		if (lIdx >= 0)
			return m_vBoneStates[lIdx];
		else
			return NULL;
	}

	int cMeshEntity::GetBoneStateNum()
	{
		return (int)m_vBoneStates.size();
	}

	//--------------------------------------------------------------

	void cMeshEntity::SetSkeletonPhysicsActive(bool a_bX)
	{
		m_bSkeletonPhysics = a_bX;

		m_bUpdatedBones = false;
		ResetGraphicsUpdated();

		m_bSkeletonPhysicsFading = false;
		m_fSkeletonPhysicsWeight = 1.0f;

		for (int bone=0; bone<GetBoneStateNum(); ++bone)
		{
			cBoneState *pState = GetBoneState(bone);
			iPhysicsBody *pBody = pState->GetBody();
			iPhysicsBody *pColliderBody = pState->GetColliderBody();

			if (pBody)
			{
				pBody->SetActive(a_bX);
				pBody->SetEnabled(a_bX);

				if (a_bX == false)
				{
					pBody->SetLinearVelocity(0);
					pBody->SetAngularVelocity(0);
				}

				if (m_bSkeletonColliders)
					pColliderBody->SetActive(!a_bX);
			}
		}
	}

	bool cMeshEntity::GetSkeletonPhysicsActive()
	{
		return m_bSkeletonPhysics;
	}

	//--------------------------------------------------------------

	void cMeshEntity::ResetGraphicsUpdated()
	{
		for (size_t i=0; i<m_vSubMeshes.size(); i++)
		{
			m_vSubMeshes[i]->m_bGraphicsUpdated = false;
		}
		m_bUpdatedBones = false;
	}

	//--------------------------------------------------------------

	cNode3D *cMeshEntity::GetNodeState(int a_lIndex)
	{
		return m_vNodeStates[a_lIndex];
	}

	int cMeshEntity::GetNodeStateIndex(const tString &a_sName)
	{
		tNodeStateIndexMapIt it = m_mapNodeStateIndices.find(a_sName);
		if (it != m_mapNodeStateIndices.end())
			return it->second;
		else
			return -1;
	}

	cNode3D *cMeshEntity::GetNodeStateFromName(const tString &a_sName)
	{
		int lIdx = GetNodeStateIndex(a_sName);
		if (lIdx >= 0)
			return m_vNodeStates[lIdx];
		else
			return NULL;
	}

	int cMeshEntity::GetNodeStateNum()
	{
		return (int)m_vNodeStates.size();
	}

	//--------------------------------------------------------------

	bool cMeshEntity::AttachEntityToParent(iEntity3D *a_pEntity, const tString &a_sParent)
	{
		m_lstAttachedEntities.push_back(a_pEntity);

		if (a_sParent == "")
		{
			AddChild(a_pEntity);
			return true;
		}

		cSubMeshEntity *pSubEntity = GetSubMeshEntityName(a_sParent);
		if (pSubEntity)
		{
			pSubEntity->AddChild(a_pEntity);
			return true;
		}

		cNode3D *pNode = GetNodeStateFromName(a_sParent);
		if (pNode)
		{
			pNode->AddEntity(a_pEntity);
			return true;
		}

		cNode3D *pBone = GetBoneStateFromName(a_sParent);
		if (pBone)
		{
			pBone->AddEntity(a_pEntity);
			return true;
		}

		Warning("Parent '%s' couldn't be found! Failed to attach '%s' to '%s'. Attaching directly to mesh.\n", a_sParent.c_str(),
			a_pEntity->GetName().c_str(), GetName().c_str());
		AddChild(a_pEntity);

		return false;
	}

	//--------------------------------------------------------------

	void cMeshEntity::UpdateLogic(float a_fTimeStep)
	{
		bool bRegularUpdate = GetGlobalRenderCount() == cRenderList::GetGlobalRenderCount();

		if (m_pMesh->GetSkeleton() && bRegularUpdate == false)
		{
			for (size_t i=0; i<m_vBoneStates.size(); ++i)
			{
				cBoneState *pState = m_vBoneStates[i];

				cEntityIterator it = pState->GetEntityIterator();
				while (it.HasNext())
				{
					iEntity3D *pEntity = static_cast<iEntity3D*>(it.Next());
					if (pEntity->GetGlobalRenderCount() == cRenderList::GetGlobalRenderCount())
					{
						bRegularUpdate = true;
						break;
					}
				}

				if (bRegularUpdate) break;
			}
		}

		const int lMaxSleepCount = 30;
		if (bRegularUpdate == false)
		{
			if (m_lStartSleepCount < lMaxSleepCount)
				++m_lStartSleepCount;
		}
		else
			m_lStartSleepCount = 0;

		if (m_lStartSleepCount >= lMaxSleepCount)
		{
			++m_lUpdateCount;
			m_fTimeStepAccum += a_fTimeStep;

			if (m_lUpdateCount < 20)
				return;

			m_lUpdateCount = 0;
			a_fTimeStep = m_fTimeStepAccum;
			m_fTimeStepAccum = 0;
		}
		else
		{
			m_lUpdateCount = 0;
			m_fTimeStepAccum = 0;
		}

		if (m_bHasNodes)
		{
			//Reset all node states to prepare for animations.
			if (m_vAnimationStates.size() > 0)
			{
				for (size_t i=0; i<m_vNodeStates.size(); i++)
				{
					cNode3D *pState = m_vNodeStates[i];
					if (pState->IsActive()) pState->SetMatrix(cMatrixf::Identity);
				}
			}

			//Go through all animation states and set the node's temporary states
			bool bAnimated = false;
			for (size_t i=0; i<m_vAnimationStates.size(); i++)
			{
				cAnimationState *pAnimState = m_vAnimationStates[i];
				if (pAnimState->IsActive())
				{
					bAnimated = true;

					cAnimation *pAnim = pAnimState->GetAnimation();

					for (int j=0; j<pAnim->GetTrackNum(); j++)
					{
						cAnimationTrack *pTrack = pAnim->GetTrack(j);

						if (pTrack->GetNodeIndex() < 0)
						{
							pTrack->SetNodeIndex(GetNodeStateIndex(pTrack->GetName()));
						}
						cNode3D *pNodeState = GetNodeState(pTrack->GetNodeIndex());

						if (pNodeState->IsActive())
							pTrack->ApplyToNode(pNodeState, pAnimState->GetTimePosition(), pAnimState->GetWeight());
					}

					pAnimState->Update(a_fTimeStep);
				}
			}

			//Go through all states and update the matrices (and thereby adding the animations together)
			if (m_vAnimationStates.size()>0 && bAnimated)
			{
				cNodeIterator NodeIt = m_pRootNode->GetChildIterator();
				while (NodeIt.HasNext())
				{
					cNode3D *pBoneState = static_cast<cNode3D*>(NodeIt.Next());

					UpdateNodeMatrixRec(m_pRootNode->GetWorldMatrix(), pBoneState);
				}
			}

			//Call callback to be run after animation
			if (m_pCallback) m_pCallback->AfterAnimationUpdate(this, a_fTimeStep);

			UpdateBVFromSubs();

			SetTransformUpdated(true);

		}
		else if (m_pMesh->GetSkeleton())
		{
		}

		for (size_t i=0; i<m_vSubMeshes.size(); ++i)
		{
			cSubMeshEntity *pSub = m_vSubMeshes[i];

			pSub->UpdateLogic(a_fTimeStep);
		}

		// Update animation events
		for (size_t i=0; i<m_vAnimationStates.size(); i++)
		{
			cAnimationState *pState = m_vAnimationStates[i];

			if (pState->IsActive() == false || pState->IsPaused()) continue;

			for (int j=0; j<pState->GetEventNum(); j++)
			{
				cAnimationEvent *pEvent = pState->GetEvent(j);

				if (pEvent->m_fTime >= pState->GetPreviousTimePosition() &&
					pEvent->m_fTime < pState->GetTimePosition())
				{
					HandleAnimationEvent(pEvent);
				}
			}
		}
	}

	//--------------------------------------------------------------

	void cMeshEntity::SetRendered(bool a_bX)
	{
		if (a_bX==m_bRendered) return;

		m_bRendered = a_bX;
		for (unsigned int i=0;i<m_vSubMeshes.size();i++)
		{
			m_vSubMeshes[i]->SetRendered(a_bX);
			m_vSubMeshes[i]->SetGlobalRenderCount(cRenderList::GetGlobalRenderCount());
		}

		SetGlobalRenderCount(cRenderList::GetGlobalRenderCount());
		m_lStartSleepCount = 0;
	}

	//--------------------------------------------------------------

	iMaterial *cMeshEntity::GetMaterial()
	{
		return NULL;
	}

	//--------------------------------------------------------------

	iVertexBuffer *cMeshEntity::GetVertexBuffer()
	{
		return NULL;
	}

	//--------------------------------------------------------------

	bool cMeshEntity::IsShadowCaster()
	{
		return m_bCastShadows;
	}

	//--------------------------------------------------------------

	cMatrixf *cMeshEntity::GetModelMatrix(cCamera3D *a_pCamera)
	{
		mtxTemp = GetWorldMatrix();
		return &mtxTemp;
	}

	//--------------------------------------------------------------

	int cMeshEntity::GetMatrixUpdateCount()
	{
		return GetTransformUpdateCount();
	}

	//--------------------------------------------------------------

	eRenderableType cMeshEntity::GetRenderType()
	{
		return eRenderableType_Mesh;
	}

	//--------------------------------------------------------------

	void cMeshEntity::UpdateNodeMatrixRec(const cMatrixf &a_mtxParentWorld, cNode3D *a_pNode)
	{
		if (a_pNode->IsActive())
		{
			a_pNode->UpdateMatrix(false);
		}

		a_pNode->UpdateWorldTransform();
		const cMatrixf &mtxWorld = a_pNode->GetWorldMatrix();

		cNodeIterator NodeIt = a_pNode->GetChildIterator();
		while (NodeIt.HasNext())
		{
			cNode3D *pChildNode = static_cast<cNode3D*>(NodeIt.Next());

			UpdateNodeMatrixRec(mtxWorld, pChildNode);
		}
	}

	//--------------------------------------------------------------

	void cMeshEntity::HandleAnimationEvent(cAnimationEvent *a_pEvent)
	{
		if (a_pEvent->m_sValue == "") return;

		switch (a_pEvent->m_Type)
		{
		case eAnimationEventType_PlaySound:
			{
				cSoundEntity *pSound = m_pWorld->CreateSoundEntity("AnimEvent", a_pEvent->m_sValue, true);
				if (pSound)
				{
					cNodeIterator NodeIt = m_pRootNode->GetChildIterator();
					if (NodeIt.HasNext())
					{
						iNode *pNode = NodeIt.Next();
						pNode->AddEntity(pSound);
					}
					else
					{
						pSound->SetPosition(m_BoundingVolume.GetWorldCenter());
					}
				}
				break;
			}
		default:
			break;
		}
	}

	//--------------------------------------------------------------

	void cMeshEntity::UpdateBVFromSubs()
	{
		if (m_pMesh->GetSkeleton())
		{
			if (m_vBoneStates.empty())
			{
				for (int i=0; i<GetSubMeshEntityNum(); i++)
				{
					cSubMeshEntity *pSub = GetSubMeshEntity(i);

					iVertexBuffer *pVtxBuffer = pSub->GetVertexBuffer();

					m_BoundingVolume.AddArrayPoints(pVtxBuffer->GetArray(eVertexFlag_Position),
						pVtxBuffer->GetVertexNum());

				}
				m_BoundingVolume.CreateFromPoints(kvVertexElements[cMath::Log2ToInt(eVertexFlag_Position)]);
			}
		}
		else
		{
			cMatrixf mtxTemp;
			if (m_pRootNode)
			{
				mtxTemp = m_pRootNode->GetWorldMatrix();
				m_pRootNode->SetMatrix(cMatrixf::Identity);
			}

			cVector3f vFinalMin = m_vSubMeshes[0]->m_BoundingVolume.GetMin();
			cVector3f vFinalMax = m_vSubMeshes[0]->m_BoundingVolume.GetMax();

			for (int i=1; i<(int)m_vSubMeshes.size(); i++)
			{
				cVector3f vMin = m_vSubMeshes[i]->m_BoundingVolume.GetMin();
				cVector3f vMax = m_vSubMeshes[i]->m_BoundingVolume.GetMax();

				if (vFinalMin.x > vMin.x) vFinalMin.x = vMin.x;
				if (vFinalMax.x < vMax.x) vFinalMax.x = vMax.x;

				if (vFinalMin.y > vMin.y) vFinalMin.y = vMin.y;
				if (vFinalMax.y < vMax.y) vFinalMax.y = vMax.y;

				if (vFinalMin.z > vMin.z) vFinalMin.z = vMin.z;
				if (vFinalMax.z < vMax.z) vFinalMax.z = vMax.z;
			}

			m_BoundingVolume.SetLocalMinMax(vFinalMin, vFinalMax);

			if (m_pRootNode)
				m_pRootNode->SetMatrix(mtxTemp);
		}
	}

	//////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////

	kBeginSerialize(cSaveData_cMeshEntity, cSaveData_iRenderable)
		kSerializeVar(m_sMeshName, eSerializeType_String)

		kSerializeVar(m_bCastShadows, eSerializeType_Bool)

		kSerializeVar(m_lBodyId, eSerializeType_Int32)

		//kSerializeClassContainer
	kEndSerialize()

		iSaveObject *cSaveData_cMeshEntity::CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler,cGame *a_pGame)
	{
		cResources *pResources = a_pGame->GetResources();
		cWorld3D *pWorld = a_pGame->GetScene()->GetWorld3D();

		cMesh *pMesh = pResources->GetMeshManager()->CreateMesh(m_sMeshName);
		if (pMesh==NULL) return NULL;

		cMeshEntity *pEntity = pWorld->CreateMeshEntity(m_sName, pMesh);

		return pEntity;
	}

	int cSaveData_cMeshEntity::GetSaveCreatePrio()
	{
		return 2;
	}

	iSaveData *cMeshEntity::CreateSaveData()
	{
		return efeNew(cSaveData_cMeshEntity,());
	}
}