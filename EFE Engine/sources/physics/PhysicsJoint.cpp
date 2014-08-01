#include "physics/PhysicsJoint.h"

#include "physics/CollideShape.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "system/LowLevelSystem.h"

#include "sound/Sound.h"
#include "scene/SoundEntity.h"
#include "sound/SoundHandler.h"

#include "scene/World3D.h"
#include "scene/Scene.h"

#include "game/Game.h"

#include "math/Math.h"

#include "game/ScriptFunc.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	iPhysicsJoint::iPhysicsJoint(const tString &a_sName, iPhysicsBody *a_pParentBody, iPhysicsBody *a_pChildBody, 
		iPhysicsWorld *a_pWorld, const cVector3f &a_vPivotPoint)
	{
		m_MaxLimit.m_sSound = "";
		m_MinLimit.m_sSound = "";

		if (a_pParentBody)
		{
			a_pParentBody->AddJoint(this);
			m_mtxParentBodySetup = a_pParentBody->GetLocalMatrix();
		}
		else
			m_mtxParentBodySetup = cMatrixf::Identity;

		m_mtxPrevChild = cMatrixf::Identity;
		m_mtxPrevParent = cMatrixf::Identity;

		a_pChildBody->AddJoint(this);
		m_mtxChildBodySetup = a_pChildBody->GetLocalMatrix();

		cMatrixf mtxInvChild = cMath::MatrixInverse(a_pChildBody->GetLocalMatrix());
		m_vLocalPivot = cMath::MatrixMul(mtxInvChild, a_vPivotPoint);
		m_vStartPivotPoint = a_vPivotPoint;

		m_sMoveSound = "";

		m_bHasCollided = false;
		m_pSound = NULL;

		m_pCallback = NULL;

		m_bAutoDeleteCallback = false;

		m_pUserData = NULL;

		m_bBreakable = false;
		m_fBreakForce = 0;
		m_sBreakSound = "";
		m_bBroken = false;

		m_fStickyMinDistance = 0;
		m_fStickyMaxDistance = 0;

		m_lLimitStepCount = 0;

		m_lSpeedCount = 0;

		m_bLimitAutoSleep = false;
		m_fLimitAutoSleepDist = 0.02f;
		m_lLimitAutoSleepNumSteps = 10;
	}

	iPhysicsJoint::~iPhysicsJoint()
	{
		if (m_bAutoDeleteCallback && m_pCallback) efeDelete(m_pCallback);

		//Destroy all controllers
		tPhysicsControllerMapIt it = m_mapControllers.begin();
		for (; it != m_mapControllers.end(); ++it)
			m_pWorld->DestroyController(it->second);

		if (m_pChildBody) m_pChildBody->RemoveJoint(this);
		if (m_pParentBody) m_pParentBody->RemoveJoint(this);

		if (m_pSound) m_pWorld->GetWorld3D()->DestroySoundEntity(m_pSound);
	}

	//------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//------------------------------------------------------------

	void iPhysicsJoint::RemoveBody(iPhysicsBody *a_pBody)
	{
		if (m_pParentBody == a_pBody) m_pParentBody = NULL;
		if (m_pChildBody == a_pBody) m_pChildBody = NULL;
	}

	//------------------------------------------------------------

	bool iPhysicsJoint::CheckBreakage()
	{
		if (m_bBreakable == false) return false;

		float fForceSize = GetForce().Length();

		if (fForceSize >= m_fBreakForce || m_bBroken)
		{
			if (m_sBreakSound != "")
			{
				cWorld3D *pWorld3D = m_pWorld->GetWorld3D();
				cSoundEntity *pSound = pWorld3D->CreateSoundEntity("BreakSound", m_sBreakSound, true);
				if (pSound) pSound->SetPosition(m_vPivotPoint);
			}
			return true;
		}

		return false;
	}

	//------------------------------------------------------------

	void iPhysicsJoint::Break()
	{
		m_bBroken = true;
		m_bBreakable = true;
	}

	//------------------------------------------------------------

	void iPhysicsJoint::AddController(iPhysicsController *a_pController)
	{
		m_mapControllers.insert(tPhysicsControllerMap::value_type(a_pController->GetName(), a_pController));

		a_pController->SetBody(m_pChildBody);
		a_pController->SetJoint(this);
	}

	//------------------------------------------------------------

	iPhysicsController *iPhysicsJoint::GetController(const tString &a_sName)
	{
		tPhysicsControllerMapIt it = m_mapControllers.find(a_sName);
		if (it == m_mapControllers.end()) return NULL;

		return it->second;
	}

	//------------------------------------------------------------

	bool iPhysicsJoint::ChangeController(const tString &a_sName)
	{
		iPhysicsController *pNewCtrl = GetController(a_sName);
		if (pNewCtrl == NULL) return false;

		tPhysicsControllerMapIt it = m_mapControllers.begin();
		for (; it != m_mapControllers.end(); ++it)
		{
			iPhysicsController *pCtrl = it->second;

			if (pCtrl == pNewCtrl)
				pCtrl->SetActive(true);
			else
				pCtrl->SetActive(false);
		}

		return true;
	}

	//------------------------------------------------------------

	cPhysicsControllerIterator iPhysicsJoint::GetControllerIterator()
	{
		return cPhysicsControllerIterator(&m_mapControllers);
	}

	//------------------------------------------------------------

	void iPhysicsJoint::SetAllControllerPaused(bool a_bX)
	{
		tPhysicsControllerMapIt it = m_mapControllers.begin();
		for (; it != m_mapControllers.end(); ++it)
		{
			iPhysicsController *pCtrl = it->second;

			pCtrl->SetPaused(a_bX);
		}
	}

	//--------------------------------------------------------------

	void iPhysicsJoint::OnPhysicsUpdate()
	{
		if (m_pParentBody)
			m_vPivotPoint = cMath::MatrixMul(m_pChildBody->GetLocalMatrix(), m_vLocalPivot);

		cWorld3D *pWorld3D = m_pWorld->GetWorld3D();
		if (pWorld3D == NULL) return;
		if (m_sMoveSound == "") return;

		if (pWorld3D->GetSound()->GetSoundHandler()->GetSilent()) return;

		cVector3f vVel(0,0,0);
		if (m_MoveSpeedType == ePhysicsJointSpeed_Angular)
		{
			if (m_pParentBody)
				vVel = m_pChildBody->GetLinearVelocity() - m_pParentBody->GetLinearVelocity();
			else
				vVel = m_pChildBody->GetLinearVelocity();
		}
		else
		{
			if (m_pParentBody)
				vVel = m_pChildBody->GetAngularVelocity() - m_pParentBody->GetAngularVelocity();
			else
				vVel = m_pChildBody->GetAngularVelocity();
		}

		if (m_pParentBody)
		{
			if (m_mtxPrevChild == m_pChildBody->GetLocalMatrix() &&
				m_mtxPrevParent == m_pParentBody->GetLocalMatrix())
				vVel = 0;

			m_mtxPrevChild = m_pChildBody->GetLocalMatrix();
			m_mtxPrevParent = m_pParentBody->GetLocalMatrix();
		}
		else
		{
			if (m_mtxPrevChild == m_pChildBody->GetLocalMatrix())
				vVel = 0;

			m_mtxPrevChild = m_pChildBody->GetLocalMatrix();
		}

		float fSpeed = vVel.Length();

		if (pWorld3D->SoundEntityExists(m_pSound) == false)
			m_pSound = NULL;

		if (m_pSound)
		{
			float fMin = cMath::Max(m_fMinMoveSpeed-0.2f, 0.1f);
			if (fSpeed <= fMin)
			{
				m_pSound->FadeOut(4.3f);
				m_pSound = NULL;
			}
			else
			{
				cSoundEntry *pEntry = m_pSound->GetSoundEntry(eSoundEntityType_Main);
				if (pEntry)
				{
					float fFreq, fVolume;
					CalcSoundFreq(fSpeed, &fFreq, &fVolume);

					pEntry->m_fNormalSpeed = fFreq;
					pEntry->m_fNormalVolumeMul = fVolume;
				}
				else
				{}

				m_pSound->SetPosition(m_vPivotPoint);
			}
		}
		else
		{
			if (fSpeed > m_fMinMoveSpeed)
			{
				if (m_lSpeedCount >= 3)
				{
					m_lSpeedCount = 0;
					m_pSound = pWorld3D->CreateSoundEntity("MoveSound", m_sMoveSound, true);
					if (m_pSound)
					{
						m_pSound->SetIsSaved(false);
						m_pSound->FadeIn(3.3f);
					}
				}
				else
					m_lSpeedCount++;
			}
			else
				m_lSpeedCount = 0;
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void iPhysicsJoint::CheckLimitAutoSleep(iPhysicsJoint *a_pJoint, const float a_fMin, const float a_fMax,
		const float a_fDist)
	{
		if (a_pJoint->m_bLimitAutoSleep)
		{
			float fMinDiff = std::abs(a_fMin - a_fDist);
			float fMaxDiff = std::abs(a_fMax - a_fDist);

			if (fMaxDiff < a_pJoint->m_fLimitAutoSleepDist ||
				fMinDiff < a_pJoint->m_fLimitAutoSleepDist)
			{
				if (a_pJoint->m_lLimitStepCount >= a_pJoint->m_lLimitAutoSleepNumSteps)
					a_pJoint->m_pChildBody->DisableAfterSimulation();
				else
					a_pJoint->m_lLimitStepCount++;
			}
			else
				a_pJoint->m_lLimitStepCount = 0;
		}
	}

	//--------------------------------------------------------------

	void iPhysicsJoint::OnMaxLimit()
	{
		if (m_bHasCollided == false && m_pCallback)
			m_pCallback->OnMaxLimit(this);

		if (m_bHasCollided == false)
		{
			tPhysicsControllerMapIt it = m_mapControllers.begin();
			for (; it != m_mapControllers.end(); ++it)
			{
				iPhysicsController *pCtrl = it->second;

				if (pCtrl->IsActive() && pCtrl->GetEndType() == ePhysicsControllerEnd_OnMax)
				{
					pCtrl->SetActive(false);
					iPhysicsController *pNextCtrl = GetController(pCtrl->GetNextController());
					if (pNextCtrl) pNextCtrl->SetActive(true);
					else Warning("Controller '%s' does not exist in joint '%s'\n", pCtrl->GetNextController().c_str(),
						m_sName.c_str());
				}
			}
		}

		LimitEffect(&m_MaxLimit);
	}

	//--------------------------------------------------------------

	void iPhysicsJoint::OnMinLimit()
	{
		if (m_bHasCollided == false && m_pCallback)
			m_pCallback->OnMinLimit(this);

		if (m_bHasCollided == false)
		{
			tPhysicsControllerMapIt it = m_mapControllers.begin();
			for (; it != m_mapControllers.end(); ++it)
			{
				iPhysicsController *pCtrl = it->second;

				if (pCtrl->IsActive() && pCtrl->GetEndType() == ePhysicsControllerEnd_OnMin)
				{
					pCtrl->SetActive(false);
					iPhysicsController *pNextCtrl = GetController(pCtrl->GetNextController());
					if (pNextCtrl) pNextCtrl->SetActive(true);
					else Warning("Controller '%s' does not exist in joint '%s'\n", pCtrl->GetNextController().c_str(),
						m_sName.c_str());
				}
			}
		}

		LimitEffect(&m_MinLimit);
	}

	//--------------------------------------------------------------

	void iPhysicsJoint::OnNoLimit()
	{
		m_bHasCollided = false;
	}

	//--------------------------------------------------------------

	void iPhysicsJoint::CalcSoundFreq(float a_fSpeed, float *a_pFreq, float *a_pVol)
	{
		float fAbsSpeed = std::abs(a_fSpeed);
		float fFreq = 1;
		float fVolume = 1;

		if (fAbsSpeed >= m_fMiddleMoveSpeed)
		{
			if (fAbsSpeed >= m_fMaxMoveFreqSpeed)
			{
				fFreq = m_fMaxMoveFreq;
				fVolume = m_fMaxMoveVolume;
			}
			else
			{
				float fT = (fAbsSpeed - m_fMiddleMoveSpeed) /
					(m_fMaxMoveFreqSpeed - m_fMiddleMoveSpeed);

				fFreq = (1 - fT) + fT * m_fMaxMoveFreq;
				fVolume = m_fMiddleMoveVolume * (1 - fT) + fT * m_fMaxMoveVolume;
			}
		}
		else
		{
			if (fAbsSpeed <= m_fMinMoveFreqSpeed)
			{
				fFreq = m_fMinMoveFreq;
				fVolume = m_fMinMoveVolume;
			}
			else
			{
				float fT = (m_fMiddleMoveSpeed - fAbsSpeed) /
					(m_fMiddleMoveSpeed - m_fMinMoveFreqSpeed);

				fFreq = (1 - fT) + fT * m_fMinMoveFreq;
				fVolume = m_fMiddleMoveVolume * (1 - fT) + fT * m_fMinMoveVolume;
			}
		}

		*a_pFreq = fFreq;
		*a_pVol = fVolume;
	}

	//--------------------------------------------------------------

	void iPhysicsJoint::LimitEffect(cJointLimitEffect *a_pEffect)
	{
		cWorld3D *pWorld3D = m_pWorld->GetWorld3D();

		if (pWorld3D && a_pEffect->m_sSound != "")
		{
			cVector3f vVel(0,0,0);
			if (m_pParentBody)
				vVel = m_pChildBody->GetLinearVelocity() - m_pParentBody->GetLinearVelocity();
			else
				vVel = m_pChildBody->GetLinearVelocity();

			float fSpeed = vVel.Length();
			if (fSpeed > a_pEffect->m_fMaxSpeed) fSpeed = a_pEffect->m_fMaxSpeed;

			if (fSpeed >= a_pEffect->m_fMinSpeed && m_bHasCollided == false && a_pEffect->m_sSound != "")
			{
				float fVolume = (fSpeed - a_pEffect->m_fMinSpeed) / (a_pEffect->m_fMaxSpeed - a_pEffect->m_fMinSpeed);

				cSoundEntity *pSound = pWorld3D->CreateSoundEntity("LimitSound", a_pEffect->m_sSound, true);
				if (pSound)
				{
					pSound->SetVolume(fVolume);
					pSound->SetPosition(m_pChildBody->GetLocalPosition());
				}
			}
		}

		m_bHasCollided = true;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	kBeginSerializeBase(cJointLimitEffect)
		kSerializeVar(m_sSound, eSerializeType_String)
		kSerializeVar(m_fMinSpeed, eSerializeType_Float32)
		kSerializeVar(m_fMaxSpeed, eSerializeType_Float32)
	kEndSerialize()

	//--------------------------------------------------------------

	kBeginSerializeVirtual(cSaveData_iPhysicsJoint, iSaveData)
	kSerializeVar(m_sName, eSerializeType_String)

	kSerializeVar(m_lParentBodyId, eSerializeType_Int32)
	kSerializeVar(m_lChildBodyId, eSerializeType_Int32)

	kSerializeVar(m_mtxParentBodySetup, eSerializeType_Matrixf)
	kSerializeVar(m_mtxChildBodySetup, eSerializeType_Matrixf)

	kSerializeVar(m_vPinDir, eSerializeType_Vector3f)
	kSerializeVar(m_vStartPivotPoint, eSerializeType_Vector3f)

	kSerializeClassContainer(m_lstControllers, cSaveData_iPhysicsController, eSerializeType_Class)

	kSerializeVar(m_MaxLimit, eSerializeType_Class)
	kSerializeVar(m_MinLimit, eSerializeType_Class)

	kSerializeVar(m_sMoveSound, eSerializeType_String)

	kSerializeVar(m_fMinMoveSpeed, eSerializeType_Float32)
	kSerializeVar(m_fMinMoveFreq, eSerializeType_Float32)
	kSerializeVar(m_fMinMoveFreqSpeed, eSerializeType_Float32)
	kSerializeVar(m_fMinMoveVolume, eSerializeType_Float32)
	kSerializeVar(m_fMaxMoveFreq, eSerializeType_Float32)
	kSerializeVar(m_fMaxMoveFreqSpeed, eSerializeType_Float32)
	kSerializeVar(m_fMaxMoveVolume, eSerializeType_Float32)
	kSerializeVar(m_fMiddleMoveSpeed, eSerializeType_Float32)
	kSerializeVar(m_fMiddleMoveVolume, eSerializeType_Float32)
	kSerializeVar(m_MoveSpeedType, eSerializeType_Int32)

	kSerializeVar(m_bBreakable, eSerializeType_Bool)
	kSerializeVar(m_fBreakForce, eSerializeType_Float32)
	kSerializeVar(m_sBreakSound, eSerializeType_String)
	kSerializeVar(m_bBroken, eSerializeType_Bool)

	kSerializeVar(m_sCallbackMaxFunc, eSerializeType_String)
	kSerializeVar(m_sCallbackMinFunc, eSerializeType_String)

	kSerializeVar(m_bAutoDeleteCallback, eSerializeType_Bool)
	kEndSerialize()

	//--------------------------------------------------------------

	iSaveData *iPhysicsJoint::CreateSaveData()
	{
		return NULL;
	}

	//--------------------------------------------------------------

	void iPhysicsJoint::SaveToSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_SaveToBegin(iPhysicsJoint);

		//Variables
		kSaveData_SaveTo(m_sName);

		kSaveData_SaveTo(m_mtxParentBodySetup);
		kSaveData_SaveTo(m_mtxChildBodySetup);

		kSaveData_SaveTo(m_vPinDir);
		kSaveData_SaveTo(m_vStartPivotPoint);

		kSaveData_SaveTo(m_MaxLimit);
		kSaveData_SaveTo(m_MinLimit);
		kSaveData_SaveTo(m_sMoveSound);
		kSaveData_SaveTo(m_fMinMoveSpeed);
		kSaveData_SaveTo(m_fMinMoveFreq);
		kSaveData_SaveTo(m_fMinMoveFreqSpeed);
		kSaveData_SaveTo(m_fMinMoveVolume);
		kSaveData_SaveTo(m_fMaxMoveFreq);
		kSaveData_SaveTo(m_fMaxMoveFreqSpeed);
		kSaveData_SaveTo(m_fMaxMoveVolume);
		kSaveData_SaveTo(m_fMiddleMoveSpeed);
		kSaveData_SaveTo(m_fMiddleMoveVolume);
		kSaveData_SaveTo(m_MoveSpeedType);
		kSaveData_SaveTo(m_bBreakable);
		kSaveData_SaveTo(m_fBreakForce);
		kSaveData_SaveTo(m_sBreakSound);
		kSaveData_SaveTo(m_bBroken);

		kSaveData_SaveTo(m_bAutoDeleteCallback);

		//Callback
		if (m_pCallback && m_pCallback->IsScript())
		{
			cScriptJointCallback *pScriptCallback = static_cast<cScriptJointCallback*>(m_pCallback);
			pData->m_sCallbackMaxFunc = pScriptCallback->m_sMaxFunc;
			pData->m_sCallbackMinFunc = pScriptCallback->m_sMinFunc;
		}
		else
		{
			pData->m_sCallbackMaxFunc = "";
			pData->m_sCallbackMinFunc = "";
		}

		//Controllers
		pData->m_lstControllers.Clear();
		tPhysicsControllerMapIt it = m_mapControllers.begin();
		for (; it != m_mapControllers.end(); ++it)
		{
			iPhysicsController *pController = it->second;
			cSaveData_iPhysicsController saveController;
			pController->SaveToSaveData(&saveController);

			pData->m_lstControllers.Add(saveController);
		}

		//Pointers
		kSaveData_SaveObject(m_pParentBody, m_lParentBodyId);
		kSaveData_SaveObject(m_pChildBody, m_lChildBodyId);
	}

	//--------------------------------------------------------------

	void iPhysicsJoint::LoadFromSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_LoadFromBegin(iPhysicsJoint);

		//Variables
		kSaveData_LoadFrom(m_sName);

		kSaveData_LoadFrom(m_mtxParentBodySetup);
		kSaveData_LoadFrom(m_mtxChildBodySetup);

		kSaveData_LoadFrom(m_vPinDir);
		kSaveData_LoadFrom(m_vStartPivotPoint);

		kSaveData_LoadFrom(m_MaxLimit);
		kSaveData_LoadFrom(m_MinLimit);
		kSaveData_LoadFrom(m_sMoveSound);
		kSaveData_LoadFrom(m_fMinMoveSpeed);
		kSaveData_LoadFrom(m_fMinMoveFreq);
		kSaveData_LoadFrom(m_fMinMoveFreqSpeed);
		kSaveData_LoadFrom(m_fMinMoveVolume);
		kSaveData_LoadFrom(m_fMaxMoveFreq);
		kSaveData_LoadFrom(m_fMaxMoveFreqSpeed);
		kSaveData_LoadFrom(m_fMaxMoveVolume);
		kSaveData_LoadFrom(m_fMiddleMoveSpeed);
		kSaveData_LoadFrom(m_fMiddleMoveVolume);
		kSaveData_LoadFrom(m_bBreakable);
		kSaveData_LoadFrom(m_fBreakForce);
		kSaveData_LoadFrom(m_sBreakSound);
		kSaveData_LoadFrom(m_bBroken);
		m_MoveSpeedType = (ePhysicsJointSpeed)pData->m_MoveSpeedType;

		kSaveData_LoadFrom(m_bAutoDeleteCallback);

		//Controllers
		cContainerListIterator<cSaveData_iPhysicsController> CtrlIt = pData->m_lstControllers.GetIterator();
		while (CtrlIt.HasNext())
		{
			cSaveData_iPhysicsController &saveCtrl = CtrlIt.Next();
			iPhysicsController *pController = m_pWorld->CreateController(saveCtrl.m_sName);
			pController->LoadFromSaveData(&saveCtrl);

			AddController(pController);
		}
	}

	//--------------------------------------------------------------

	void iPhysicsJoint::SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		kSaveData_SetupBegin(iPhysicsJoint);

		if (pData->m_sCallbackMaxFunc != "" || pData->m_sCallbackMinFunc != "")
		{
			cScriptJointCallback *pCallback = efeNew(cScriptJointCallback, (a_pGame->GetScene()));
			pCallback->m_sMaxFunc = pData->m_sCallbackMaxFunc;
			pCallback->m_sMinFunc = pData->m_sCallbackMinFunc;
			SetCallback(pCallback, true);
		}
	}
}