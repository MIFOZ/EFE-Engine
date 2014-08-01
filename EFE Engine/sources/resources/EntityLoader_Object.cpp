#include "resources/EntityLoader_Object.h"

#include "system/String.h"
#include "scene/World3D.h"
#include "system/LowLevelSystem.h"
#include "tinyxml.h"

#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsJoint.h"
#include "physics/PhysicsController.h"
#include "physics/CollideShape.h"
#include "physics/PhysicsJointBall.h"

#include "math/Math.h"

#include "resources/MeshManager.h"
#include "resources/MaterialManager.h"
#include "resources/AnimationManager.h"
#include "resources/MeshLoaderHandler.h"
#include "resources/FileSearcher.h"

#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "graphics/Animation.h"
#include "scene/AnimationState.h"
#include "scene/MeshEntity.h"
#include "scene/Node3D.h"

#include "scene/Light3DSpot.h"

namespace efe
{
	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iEntity3D *cEntityLoader_Object::Load(const tString &a_sName, TiXmlElement *a_pRootElem, const cMatrixf &a_mtxTransform,
								cWorld3D *a_pWorld, const tString &a_sFileName, bool a_bLoadReference)
	{
		m_vBodies.clear();
		m_vJoints.clear();

		m_vParticleSystem.clear();
		m_vBillboards.clear();
		m_vSoundEntities.clear();
		m_vLights.clear();
		m_vBeams.clear();

		m_pEntity = NULL;
		m_pMesh = NULL;

		m_sFileName = a_sFileName;

		// Load MAIN

		TiXmlElement *pMainElem = a_pRootElem->FirstChildElement("MAIN");
		if (pMainElem == NULL)
		{
			Error("Couldn't load element MAIN"); return NULL;
		}

		m_sName = cString::ToString(pMainElem->Attribute("Name"), "");
		m_sSubType = cString::ToString(pMainElem->Attribute("SubType"), "");

		BeforeLoad(a_pRootElem, a_mtxTransform, a_pWorld);

		// Load GRAPHICS
		TiXmlElement *pGfxElem = a_pRootElem->FirstChildElement("GRAPHICS");
		if (pGfxElem == NULL)
		{
			Error("Couldn't load element GRAPHICS"); return NULL;
		}

		tString sMeshFile = cString::ToString(pMainElem->Attribute("ModelFile"), "");
		bool bShadows = cString::ToBool(pMainElem->Attribute("CastShadows"), true);

		bool bAnimationLoop = cString::ToBool(pMainElem->Attribute("AnimationLoop"), true);
		bool bAnimationRandStart = cString::ToBool(pMainElem->Attribute("AnimationRandStart"), true);

		m_pMesh = a_pWorld->GetResources()->GetMeshManager()->CreateMesh(sMeshFile);
		if (m_pMesh) return NULL;

		// Load SUBMESHES
		TiXmlElement *pSubMeshElem = a_pRootElem->FirstChildElement("SUBMESH");
		for (; pSubMeshElem != NULL; pSubMeshElem = pSubMeshElem->NextSiblingElement("SUBMESH"))
		{
			tString sName = cString::ToString(pSubMeshElem->Attribute("Name"), "");
			tString sMaterialFile = cString::ToString(pSubMeshElem->Attribute("MaterialFile"), "");

			cSubMeshEntity *pSubEntity = m_pEntity->GetSubMeshEntityName(sName);
			if (pSubEntity == NULL)
			{
				Warning("Sub mesh '%s' does not exist in mesh '%s'!\n", sName.c_str(),
					m_pMesh->GetName().c_str());
				continue;
			}

			if (sMaterialFile != "")
			{
				iMaterial *pMaterial = a_pWorld->GetResources()->GetMaterialManager()->CreateMaterial(sMaterialFile);
				if (pMaterial)
					pSubEntity->SetCustomMaterial(pMaterial);
			}
		}

		// Load ANIMATIONS
		TiXmlElement *pAnimRootElem = a_pRootElem->FirstChildElement("ANIMATIONS");
		if (pAnimRootElem)
		{
			m_pEntity->ClearAnimations();

			TiXmlElement *pAnimElement = pAnimRootElem->FirstChildElement("Animation");
			for (; pAnimElement != NULL; pAnimElement = pAnimElement->NextSiblingElement("Animation"))
			{
				tString sFile = cString::ToString(pAnimElement->Attribute("File"), "");
				tString sName = cString::ToString(pAnimElement->Attribute("Name"), "Unknown");
				float fSpeed = cString::ToFloat(pAnimElement->Attribute("Speed"), 1.0f);
				float fSpecialEventTime = cString::ToFloat(pAnimElement->Attribute("SpcialEventTime"), 0.0f);

				cAnimationManager *pAnimManager = a_pWorld->GetResources()->GetAnimationManager();

				cAnimation *pAnimation = pAnimManager->CreateAnimation(sFile);

				if (pAnimation)
				{
					cAnimationState *pState = m_pEntity->AddAnimation(pAnimation, sName, fSpeed);
					pState->SetSpecialEventTime(fSpecialEventTime);

					TiXmlElement *pAnimEventElem = pAnimElement->FirstChildElement("Event");
					for (; pAnimEventElem != NULL; pAnimEventElem = pAnimEventElem->NextSiblingElement("Event"))
					{
						cAnimationEvent *pEvent = pState->CreateEvent();

						pEvent->m_fTime = cString::ToFloat(pAnimEventElem->Attribute("Time"), 1.0f);
						pEvent->m_Type = GetAnimationEventType(pAnimEventElem->Attribute("Type"));
						pEvent->m_sValue = cString::ToString(pAnimEventElem->Attribute("Value"), "");
					}
				}
			}
		}

		//Load PHYSICS

		bool bUsingNodeBodies = false;
		if ((m_pMesh->GetPhysicsJointNum() > 0 || m_pMesh->HasSeveralBodies()) &&
			(m_pMesh->GetAnimationNum() <= 0 || m_pMesh->GetSkeleton()))
		{
			m_pMesh->CreateJointsAndBodies(&m_vBodies, m_pEntity, &m_vJoints, a_mtxTransform, a_pWorld->GetPhysicsWorld());

			TiXmlElement *pPhysicsElem = a_pRootElem->FirstChildElement("PHYSICS");
			for (; pPhysicsElem != NULL; pPhysicsElem = pPhysicsElem->NextSiblingElement("PHYSIS"))
			{
				iPhysicsBody *pBody = NULL;

				tString sSubName = a_sName + "_" + cString::ToString(pPhysicsElem->Attribute("SubName"), "");

				tString sMaterial = cString::ToString(pPhysicsElem->Attribute("Material"), "Default");

				bool bStaticMeshCollider = cString::ToBool(pPhysicsElem->Attribute("StaticMeshCollider"), false);
				tString sColliderMesh = cString::ToString(pPhysicsElem->Attribute("ColliderMesh"), "");

				if (bStaticMeshCollider)
				{
					cSubMesh *pSubMesh = m_pMesh->GetSubMeshName(sColliderMesh);
					if (pSubMesh)
					{
						iCollideShape *pShape = a_pWorld->GetPhysicsWorld()->CreateMeshShape(pSubMesh->GetVertexBuffer());
						pBody = a_pWorld->GetPhysicsWorld()->CreateBody(sColliderMesh, pShape);
					}
					else
					{
						Error("Couldn't find sub mesh '%s' to create collision mesh\n", sColliderMesh.c_str());
						continue;
					}

					cSubMeshEntity *pSubEntity = m_pEntity->GetSubMeshEntityName(sColliderMesh);

					pBody->CreateNode()->AddEntity(pSubEntity);
					pBody->SetMass(0);
					pBody->SetMatrix(a_mtxTransform);

					pSubEntity->SetBody(pBody);

					m_vBodies.push_back(pBody);
				}
				else
				{
					pBody = (iPhysicsBody*)STLFindByName(m_vBodies, sSubName);
					if (pBody == NULL)
					{
						Error("Cannot find sub entity '%s' in mesh '%s'\n", sSubName.c_str(), m_pMesh->GetName().c_str());
						continue;
					}
				}

				SetBodyProperties(pBody, pPhysicsElem);

				iPhysicsMaterial *pMaterial = a_pWorld->GetPhysicsWorld()->GetMaterialFromName(sMaterial);
				if (pMaterial)
					pBody->SetMaterial(pMaterial);
				else
				{
					Error("Physics Material '%s' for body '%s' in mesh '%s' doesn't exists!\n",
						sMaterial.c_str(), pBody->GetName().c_str(), m_pMesh->GetName().c_str());
				}
			}

			// Properties for joint
			TiXmlElement *pJointElem = a_pRootElem->FirstChildElement("JOINT");
			for (; pJointElem != NULL; pJointElem = pJointElem->NextSiblingElement("JOINT"))
			{
				tString sName = a_sName + "_" + cString::ToString(pJointElem->Attribute("Name"), "");

				iPhysicsJoint *pJoint = (iPhysicsJoint*)STLFindByName(m_vJoints, sName);
				if (pJoint)
					SetJointProperties(pJoint, pJointElem, a_pWorld);
				else
					Error("Joint '%s' not found in mesh '%s'\n", sName.c_str(), m_pMesh->GetName().c_str());

			}
		}
		else if (m_pMesh->GetNodeNum() > 0)
		{
			iPhysicsBody *pRootBody = NULL;

			bUsingNodeBodies = true;

			m_pMesh->CreateNodeBodies(&pRootBody, &m_vBodies, m_pEntity, a_pWorld->GetPhysicsWorld(),
				a_mtxTransform);

			TiXmlElement *pPhysicsElem = a_pRootElem->FirstChildElement("PHYSICS");
			for (; pPhysicsElem != NULL; pPhysicsElem = pPhysicsElem->NextSiblingElement("PHYSICS"))
			{
				iPhysicsBody *pBody = NULL;

				tString sSubName = cString::ToString(pPhysicsElem->Attribute("SubName"), "");
				if (sSubName != "")
					sSubName = a_sName + "_" + sSubName;
				else
					sSubName = a_sName;

				tString sMaterial = cString::ToString(pPhysicsElem->Attribute("Material"), "Default");

				bool bStaticMeshCollider = cString::ToBool(pPhysicsElem->Attribute("StaticMeshCollider"), false);
				tString sColliderMesh = cString::ToString(pPhysicsElem->Attribute("ColliderMesh"), "");

				if (bStaticMeshCollider)
				{
					cSubMesh *pSubMesh = m_pMesh->GetSubMeshName(sColliderMesh);
					if (pSubMesh)
					{
						iCollideShape *pShape = a_pWorld->GetPhysicsWorld()->CreateMeshShape(pSubMesh->GetVertexBuffer());
						pBody = a_pWorld->GetPhysicsWorld()->CreateBody(sColliderMesh, pShape);
					}
					else
					{
						Error("Couldn't find sub mesh '%s' to create collision mesh\n", sColliderMesh.c_str());
						continue;
					}
				}
				else
				{
					pBody = (iPhysicsBody*)STLFindByName(m_vBodies, sSubName);
					if (pBody == NULL)
					{
						Error("Cannot find sub entity '%s' in mesh '%s'\n", sSubName.c_str(), m_pMesh->GetName().c_str());
						continue;
					}
				}

				SetBodyProperties(pBody, pPhysicsElem);

				iPhysicsMaterial *pMaterial = a_pWorld->GetPhysicsWorld()->GetMaterialFromName(sMaterial);
				if (pMaterial)
					pBody->SetMaterial(pMaterial);
				else
				{
					Error("Physics Material '%s' for body '%s' in mesh '%s' doesn't exist\n",
						sMaterial.c_str(), pBody->GetName().c_str(), m_pMesh->GetName().c_str());
				}
			}

			if (m_pMesh->GetPhysicsJointNum() == 1 && m_vBodies.size() == 1)
			{
				iPhysicsJoint *pJoint = m_pMesh->CreateJointInWorld(a_sName, m_pMesh->GetPhysicsJoint(0),
					NULL, m_vBodies[0], a_mtxTransform,
					a_pWorld->GetPhysicsWorld());
				if (pJoint)
				{
					TiXmlElement *pJointElem = a_pRootElem->FirstChildElement("JOINT");
					if (pJointElem)
						SetJointProperties(pJoint, pJointElem, a_pWorld);
				}
			}
			else if (m_pMesh->GetPhysicsJointNum() > 1)
			{
				Error("Over 1 joints in '%s'! Animated body meshes only allow for one joint!\n",
					m_pMesh->GetName().c_str());
			}
		}
		// Properties for other entities
		else
		{
			TiXmlElement *pPhysicsElem = a_pRootElem->FirstChildElement("PHYSICS");
			if (pPhysicsElem)
			{
				iPhysicsBody *pBody = NULL;

				tString sSubName = cString::ToString(pPhysicsElem->Attribute("SubName"), "");

				bool bCollides = cString::ToBool(pPhysicsElem->Attribute("Collides"), false);
				bool bHasPhysics = cString::ToBool(pPhysicsElem->Attribute("HasPhysics"), false);
				tString sMaterial = cString::ToString(pPhysicsElem->Attribute("Material"), "Default");

				bool bStaticMeshCollider = cString::ToBool(pPhysicsElem->Attribute("StaticMeshCollider"), false);
				tString sColliderMesh = cString::ToString(pPhysicsElem->Attribute("ColliderMesh"), "");

				if (bCollides)
				{
					if (bStaticMeshCollider)
					{
						cSubMesh *pSubMesh = m_pMesh->GetSubMeshName(sColliderMesh);
						if (pSubMesh)
						{
							iCollideShape *pShape = a_pWorld->GetPhysicsWorld()->CreateMeshShape(pSubMesh->GetVertexBuffer());

							pBody = a_pWorld->GetPhysicsWorld()->CreateBody(sColliderMesh, pShape);
						}
						else
							Error("Couldn't find sub mesh '%s' to create collision mesh\n", sColliderMesh.c_str());
					}
					else
					{
						if (m_pMesh->GetColliderNum() > 0)
						{
							pBody = a_pWorld->GetPhysicsWorld()->CreateBody(a_sName,
								m_pMesh->CreateCollideShape(a_pWorld->GetPhysicsWorld()));
						}
						else
							Warning("No collider found for '%s' \n", a_sFileName.c_str());
					}

					if (pBody)
					{
						iPhysicsMaterial *pMaterial = a_pWorld->GetPhysicsWorld()->GetMaterialFromName(sMaterial);
						if (pMaterial)
							pBody->SetMaterial(pMaterial);
						else
							Error("Physics Material '%s' for body '%s' in mesh '%s' doesn't exist!\n",
							sMaterial.c_str(), pBody->GetName().c_str(), m_pMesh->GetName().c_str());

						SetBodyProperties(pBody, pPhysicsElem);

						if (!bHasPhysics)
							pBody->SetMass(0);

						pBody->CreateNode()->AddEntity(m_pEntity);

						pBody->SetMatrix(a_mtxTransform);

						m_pEntity->SetBody(pBody);

						m_vBodies.push_back(pBody);
					}
				}
			}
		}

		if ((m_vBodies.size() <= 0 || bUsingNodeBodies) && m_pEntity->GetBody() == NULL)
		{
			m_pEntity->SetMatrix(a_mtxTransform);

			m_pEntity->UpdateLogic(0);
		}

		if (m_pEntity->GetAnimationStateNum() > 0)
		{
			cAnimationState *pAnimState = m_pEntity->GetAnimationState(0);

			pAnimState->SetActive(true);
			pAnimState->SetLoop(bAnimationLoop);
			if (bAnimationRandStart)
				pAnimState->SetTimePosition(cMath::RandRectf(0, pAnimState->GetLength()));
			else
				pAnimState->SetTimePosition(0);
		}
		else if (m_pMesh->GetSkeleton())
			m_pEntity->SetSkeletonPhysicsActive(true);

		AfterLoad(a_pRootElem, a_mtxTransform, a_pWorld);

		if (a_bLoadReference)
		{
			cMeshEntity *pEntityCopy = m_pEntity;
			cMesh *pMeshCopy = m_pMesh;
			for (int i=0; i<pMeshCopy->GetReferenceNum(); i++)
				iEntity3D *pRef = m_pMesh->CreateReferenceInWorld(a_sName, pMeshCopy->GetReference(i), pEntityCopy,
				a_pWorld, a_mtxTransform);
			return pEntityCopy;
		}
		else
			return m_pEntity;
	}

	//--------------------------------------------------------------

	void cEntityLoader_Object::SetBodyProperties(iPhysicsBody *a_pBody, TiXmlElement *a_pPhysicsElem)
	{
		float fMass = cString::ToFloat(a_pPhysicsElem->Attribute("Mass"), 1);
		tString sInertiaVec = cString::ToString(a_pPhysicsElem->Attribute("InertiaScale"), "1 1 1");

		float fAngularDamping = cString::ToFloat(a_pPhysicsElem->Attribute("AngularDamping"), 0.1f);
		float fLinearDamping = cString::ToFloat(a_pPhysicsElem->Attribute("LinearDamping"), 0.1f);

		bool bBlocksSound = cString::ToBool(a_pPhysicsElem->Attribute("BlocksSound"), false);
		bool bCollideCharacter = cString::ToBool(a_pPhysicsElem->Attribute("CollideCharacter"), true);
		bool bCollide = cString::ToBool(a_pPhysicsElem->Attribute("CollideNonCharacter"), true);

		bool bHasGravity = cString::ToBool(a_pPhysicsElem->Attribute("HasGravity"), true);

		float fMaxAngularSpeed = cString::ToFloat(a_pPhysicsElem->Attribute("MaxAngularSpeed"), 0);
		float fMaxLinearSpeed = cString::ToFloat(a_pPhysicsElem->Attribute("MaxLinearSpeed"), 0);

		bool bContinuousCollision = cString::ToBool(a_pPhysicsElem->Attribute("ConitnuousCollision"), true);

		bool bPushedByCharacterGravity = cString::ToBool(a_pPhysicsElem->Attribute("PushedByCharacterGravity"), false);

		float fAutoDisableLinearThreshold = cString::ToFloat(a_pPhysicsElem->Attribute("AutoDisableLinearThreshold"), 0.1f);
		float fAutoDisableAngularThreshold = cString::ToFloat(a_pPhysicsElem->Attribute("AutoDisableAngularThreshold"), 0.1f);
		int lAutDisableNumSteps = cString::ToInt(a_pPhysicsElem->Attribute("AutDisableNumSteps"), 10);

		bool bVolatile = cString::ToBool(a_pPhysicsElem->Attribute("Volatile"), false);

		bool bCanAttachCharacter = cString::ToBool(a_pPhysicsElem->Attribute("CanAttachCharacter"), false);

		tFloatVec vInertiaScale;
		cString::GetFloatVec(sInertiaVec, vInertiaScale);

		a_pBody->SetMass(fMass);
		a_pBody->SetAngularDamping(fAngularDamping);
		a_pBody->SetLinearDamping(fLinearDamping);
		a_pBody->SetBlocksSound(bBlocksSound);
		a_pBody->SetCollideCharacter(bCollideCharacter);
		a_pBody->SetCollide(bCollide);
		a_pBody->SetGravity(bHasGravity);

		a_pBody->SetVolatile(bVolatile);

		a_pBody->SetCanAttachCharacter(bCanAttachCharacter);

		a_pBody->SetContinuousCollision(bContinuousCollision);

		a_pBody->SetPushedByCharacterGravity(bPushedByCharacterGravity);

		a_pBody->SetMaxAngularSpeed(fMaxAngularSpeed);
		a_pBody->SetMaxLinearSpeed(fMaxLinearSpeed);
	}

	//--------------------------------------------------------------

	void cEntityLoader_Object::SetJointProperties(iPhysicsJoint *a_pJoint, TiXmlElement *a_pJointElem, cWorld3D *a_pWorld)
	{
		float fMinValue = cString::ToFloat(a_pJointElem->Attribute("MinValue"), -1);
		float fMaxValue = cString::ToFloat(a_pJointElem->Attribute("MaxValue"), -1);

		tString sMoveSound = cString::ToString(a_pJointElem->Attribute("MoveSound"), "");
		float fMinMoveSpeed = cString::ToFloat(a_pJointElem->Attribute("MinMoveSpeed"), 0.5f);
		float fMinMoveFreq = cString::ToFloat(a_pJointElem->Attribute("MinMoveFreq"), 0.9f);
		float fMinMoveFreqSpeed = cString::ToFloat(a_pJointElem->Attribute("MinMoveFreqSpeed"), 0.9f);
		float fMinMoveVolume = cString::ToFloat(a_pJointElem->Attribute("MinMoveVolume"), 0.3f);
		float fMaxMoveFreq = cString::ToFloat(a_pJointElem->Attribute("MaxMoveFreq"), 1.1f);
		float fMaxMoveFreqSpeed = cString::ToFloat(a_pJointElem->Attribute("MaxMoveFreqSpeed"), 1.1f);
		float fMaxMoveVolume = cString::ToFloat(a_pJointElem->Attribute("MaxMoveVolume"), 1.0f);
		float fMiddleMoveSpeed = cString::ToFloat(a_pJointElem->Attribute("MiddleMoveSpeed"), 1.0f);
		float fMiddleMoveVolume = cString::ToFloat(a_pJointElem->Attribute("MiddleMoveVolume"), 1.0f);
		tString sMoveType = cString::ToString(a_pJointElem->Attribute("MoveType"), "Linear");
		sMoveSound = cString::ToLowerCase(sMoveSound);

		float fStickyMinDistance = cString::ToFloat(a_pJointElem->Attribute("StickyMinDistance"), 0.0f);
		float fStickyMaxDistance = cString::ToFloat(a_pJointElem->Attribute("StickyMaxDistance"), 0.0f);

		bool bBreakable = cString::ToBool(a_pJointElem->Attribute("Breakable"), false);
		tString sBreakSound = cString::ToString(a_pJointElem->Attribute("BreakSound"), "");
		float fBreakForce = cString::ToFloat(a_pJointElem->Attribute("BreakForce"), 1000);

		bool bLimitAutoSleep = cString::ToBool(a_pJointElem->Attribute("LimitAutoSleep"), false);
		float fLimitAutoSleepDist = cString::ToFloat(a_pJointElem->Attribute("LimitAutoSleepDist"), 0.02f);
		int lLimitAutoSleepNumSteps = cString::ToInt(a_pJointElem->Attribute("LimitAutoSleepNumSteps"), 10);

		a_pJoint->SetMoveSound(sMoveSound);
		a_pJoint->SetMinMoveSpeed(fMinMoveSpeed);
		a_pJoint->SetMinMoveFreq(fMinMoveFreq);
		a_pJoint->SetMinMoveVolume(fMinMoveVolume);
		a_pJoint->SetMinMoveFreqSpeed(fMinMoveFreqSpeed);
		a_pJoint->SetMaxMoveFreq(fMaxMoveFreq);
		a_pJoint->SetMaxMoveVolume(fMaxMoveVolume);
		a_pJoint->SetMaxMoveFreqSpeed(fMaxMoveFreqSpeed);
		a_pJoint->SetMiddleMoveSpeed(fMiddleMoveSpeed);
		a_pJoint->SetMiddleMoveVolume(fMiddleMoveVolume);
		a_pJoint->SetMoveSpeedType(sMoveType == "angular" ? ePhysicsJointSpeed_Angular : ePhysicsJointSpeed_Linear);

		a_pJoint->GetMaxLimit()->m_sSound = cString::ToString(a_pJointElem->Attribute("MaxLimit_Sound"), "");
		a_pJoint->GetMaxLimit()->m_fMaxSpeed = cString::ToFloat(a_pJointElem->Attribute("MaxLimit_MaxSpeed"), 10.0f);
		a_pJoint->GetMaxLimit()->m_fMinSpeed = cString::ToFloat(a_pJointElem->Attribute("MaxLimit_MinSpeed"), 20.0f);

		a_pJoint->GetMinLimit()->m_sSound = cString::ToString(a_pJointElem->Attribute("MinLimit_Sound"), "");
		a_pJoint->GetMinLimit()->m_fMaxSpeed = cString::ToFloat(a_pJointElem->Attribute("MinLimit_MaxSpeed"), 10.0f);
		a_pJoint->GetMinLimit()->m_fMinSpeed = cString::ToFloat(a_pJointElem->Attribute("MinLimit_MinSpeed"), 20.0f);

		a_pJoint->SetStickyMinDistance(fStickyMinDistance);
		a_pJoint->SetStickyMaxDistance(fStickyMaxDistance);

		a_pJoint->SetBreakable(bBreakable);
		a_pJoint->SetBreakSound(sBreakSound);
		a_pJoint->SetBreakForce(fBreakForce);

		a_pJoint->SetLimitAutoSleep(bLimitAutoSleep);
		a_pJoint->SetLimitAutoSleepDist(fLimitAutoSleepDist);
		a_pJoint->SetLimitAutoSleepNumSteps(lLimitAutoSleepNumSteps);

		if (fMaxValue >= 0 && fMinValue >= 0)
		{
			switch (a_pJoint->GetType())
			{
			case ePhysicsJointType_Ball:
				{
					iPhysicsJointBall *pBallJoint = static_cast<iPhysicsJointBall*>(a_pJoint);
					pBallJoint->SetConeLimits(pBallJoint->GetPinDir(), cMath::ToRad(fMinValue), cMath::ToRad(fMaxValue));
					break;
				}

			default:
				break;
			}
		}

		TiXmlElement *pControllerElem = a_pJointElem->FirstChildElement("Controller");
		for (; pControllerElem != NULL; pControllerElem = pControllerElem->NextSiblingElement("Controller"))
		{
			LoadController(a_pJoint, a_pWorld->GetPhysicsWorld(), pControllerElem);
		}
	}

	//--------------------------------------------------------------

	ePhysicsControllerType GetControllerType(const char *a_pString)
	{
		if (a_pString == NULL) return ePhysicsControllerType_LastEnum;

		tString sName = a_pString;

		if (sName == "Pid") return ePhysicsControllerType_Pid;
		else if (sName == "Spring") return ePhysicsControllerType_Spring;

		return ePhysicsControllerType_LastEnum;
	}

	ePhysicsControllerInput GetControllerInput(const char *a_pString)
	{
		if (a_pString == NULL) return ePhysicsControllerInput_LastEnum;

		tString sName = a_pString;

		if (sName == "JointAngle") return ePhysicsControllerInput_JointAngle;
		else if (sName == "JointDist") return ePhysicsControllerInput_JointDist;
		else if (sName == "LinearSpeed") return ePhysicsControllerInput_LinearSpeed;
		else if (sName == "AngularSpeed") return ePhysicsControllerInput_AngularSpeed;

		return ePhysicsControllerInput_LastEnum;
	}

	ePhysicsControllerOutput GetControllerOutput(const char *a_pString)
	{
		if (a_pString == NULL) return ePhysicsControllerOutput_LastEnum;

		tString sName = a_pString;

		if (sName == "Force") return ePhysicsControllerOutput_Force;
		else if (sName == "Torque") return ePhysicsControllerOutput_Torque;

		return ePhysicsControllerOutput_LastEnum;
	}

	ePhysicsControllerAxis GetControllerAxis(const char *a_pString)
	{
		if (a_pString == NULL) return ePhysicsControllerAxis_LastEnum;

		tString sName = a_pString;

		if (sName == "X") return ePhysicsControllerAxis_X;
		else if (sName == "Y") return ePhysicsControllerAxis_Y;
		else if (sName == "Z") return ePhysicsControllerAxis_Z;

		return ePhysicsControllerAxis_LastEnum;
	}

	ePhysicsControllerEnd GetControllerEnd(const char *a_pString)
	{
		if (a_pString == NULL) return ePhysicsControllerEnd_Null;

		tString sName = a_pString;

		if (sName == "OnMax") return ePhysicsControllerEnd_OnMax;
		else if (sName == "OnMin") return ePhysicsControllerEnd_OnMin;
		else if (sName == "OnDest") return ePhysicsControllerEnd_OnDest;

		return ePhysicsControllerEnd_Null;
	}

	void cEntityLoader_Object::LoadController(iPhysicsJoint *a_pJoint, iPhysicsWorld *a_pPhysicsWorld, TiXmlElement *a_pElem)
	{
		// Get the properties
		tString sName = cString::ToString(a_pElem->Attribute("Name"), "");
		bool bActive = cString::ToBool(a_pElem->Attribute("Active"), false);

		ePhysicsControllerType CtrlType = GetControllerType(a_pElem->Attribute("Type"));
		float fA = cString::ToFloat(a_pElem->Attribute("A"), 0);
		float fB = cString::ToFloat(a_pElem->Attribute("B"), 0);
		float fC = cString::ToFloat(a_pElem->Attribute("C"), 0);
		int lIntegralSize = cString::ToInt(a_pElem->Attribute("IntegralSize"), 1);

		ePhysicsControllerInput CtrlInput = GetControllerInput(a_pElem->Attribute("Input"));
		ePhysicsControllerAxis CtrlInputAxis = GetControllerAxis(a_pElem->Attribute("InputAxis"));
		float fDestValue = cString::ToFloat(a_pElem->Attribute("DestValue"), 0);
		float fMaxOutput = cString::ToFloat(a_pElem->Attribute("MaxOutput"), 0);

		ePhysicsControllerOutput CtrlOutput = GetControllerOutput(a_pElem->Attribute("Output"));
		ePhysicsControllerAxis CtrlOutputAxis = GetControllerAxis(a_pElem->Attribute("OutputAxis"));
		bool bMulMassWithOutput = cString::ToBool(a_pElem->Attribute("MulMassWithOutput"), false);

		ePhysicsControllerEnd CtrlEnd = GetControllerEnd(a_pElem->Attribute("EndType"));
		tString sNextCtrl = cString::ToString(a_pElem->Attribute("NextController"), "");

		bool bLogInfo = cString::ToBool(a_pElem->Attribute("LogInfo"), false);

		if (CtrlInput == ePhysicsControllerInput_JointAngle)
			fDestValue = cMath::ToRad(fDestValue);

		iPhysicsController *pController = a_pPhysicsWorld->CreateController(sName);

		pController->SetType(CtrlType);

		pController->SetA(fA);
		pController->SetB(fB);
		pController->SetC(fC);

		pController->SetPidIntegralSize(lIntegralSize);

		pController->SetActive(bActive);
		pController->SetInputType(CtrlInput, CtrlInputAxis);
		pController->SetDestValue(fDestValue);

		pController->SetOutputType(CtrlOutput, CtrlOutputAxis);
		pController->SetMaxOutput(fMaxOutput);
		pController->SetMulMassWithOutput(bMulMassWithOutput);

		pController->SetEndType(CtrlEnd);
		pController->SetNextController(sNextCtrl);

		pController->SetLogInfo(bLogInfo);

		a_pJoint->AddController(pController);
	}
}