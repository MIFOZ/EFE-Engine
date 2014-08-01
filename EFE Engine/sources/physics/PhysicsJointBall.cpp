#include "physics/PhysicsJointBall.h"

#include "scene/World3D.h"
#include "scene/Scene.h"
#include "game/Game.h"

#include "physics/PhysicsBody.h"
#include "physics/PhysicsWorld.h"

#include "system/LowLevelSystem.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	kBeginSerialize(cSaveData_iPhysicsJointBall, cSaveData_iPhysicsJoint)
		kSerializeVar(m_fMaxConeAngle, eSerializeType_Float32)
		kSerializeVar(m_fMaxTwistAngle, eSerializeType_Float32)
		kSerializeVar(m_vConePin, eSerializeType_Vector3f)
	kEndSerialize()

	//--------------------------------------------------------------

	iSaveObject *cSaveData_iPhysicsJointBall::CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		iPhysicsWorld *pWorld = a_pGame->GetScene()->GetWorld3D()->GetPhysicsWorld();

		cMatrixf mtxChildTemp, mtxParentTemp;

		iPhysicsBody *pChildBody = static_cast<iPhysicsBody*>(a_pSaveObjectHandler->Get(m_lChildBodyId));
		if (pChildBody == NULL) return NULL;

		iPhysicsBody *pParentBody = NULL;
		if (m_lParentBodyId > 0) pParentBody = static_cast<iPhysicsBody*>(a_pSaveObjectHandler->Get(m_lParentBodyId));

		mtxChildTemp = pChildBody->GetLocalMatrix();
		if (pParentBody) mtxParentTemp = pParentBody->GetLocalMatrix();

		pChildBody->SetMatrix(m_mtxChildBodySetup);
		if (pParentBody) pParentBody->SetMatrix(m_mtxParentBodySetup);

		iPhysicsJointBall *pJoint = pWorld->CreateJointBall(m_sName, m_vStartPivotPoint, pParentBody, pChildBody);
		pJoint->SetConeLimits(m_vConePin, m_fMaxConeAngle, m_fMaxTwistAngle);

		pChildBody->SetMatrix(mtxChildTemp);
		if (pParentBody) pParentBody->SetMatrix(mtxParentTemp);

		return pJoint;
	}

	//--------------------------------------------------------------

	int cSaveData_iPhysicsJointBall::GetSaveCreatePrio()
	{
		return 1;
	}

	//--------------------------------------------------------------

	iSaveData *iPhysicsJointBall::CreateSaveData()
	{
		return efeNew(cSaveData_iPhysicsJointBall, ());
	}

	//--------------------------------------------------------------

	void iPhysicsJointBall::SaveToSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_SaveToBegin(iPhysicsJointBall);

		kSaveData_SaveTo(m_fMaxConeAngle);
		kSaveData_SaveTo(m_fMaxTwistAngle);
		kSaveData_SaveTo(m_vConePin);
	}

	//--------------------------------------------------------------

	void iPhysicsJointBall::LoadFromSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_LoadFromBegin(iPhysicsJointBall);

		kSaveData_LoadFrom(m_fMaxConeAngle);
		kSaveData_LoadFrom(m_fMaxTwistAngle);
		kSaveData_LoadFrom(m_vConePin);
	}

	//--------------------------------------------------------------

	void iPhysicsJointBall::SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		kSaveData_SetupBegin(iPhysicsJointBall);
	}
}