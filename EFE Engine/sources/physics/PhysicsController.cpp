#include "physics/PhysicsController.h"

#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsJoint.h"

#include "math/Math.h"

#include "system/LowLevelSystem.h"

namespace efe
{
	bool iPhysicsController::m_bUseInputMatrixFix = false;

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iPhysicsController::iPhysicsController(const tString &a_sName, iPhysicsWorld *a_pWorld)
	{
		m_sName = a_sName;

		m_pWolrd = a_pWorld;

		m_pBody = NULL;
		m_pJoint = NULL;

		m_bActive = false;

		m_PidController.SetErrorNum(10);

		m_bMulMassWithOutput = false;

		m_fMaxOutput = 0;

		m_bLogInfo = false;

		m_bPaused = false;
	}

	//--------------------------------------------------------------

	iPhysicsController::~iPhysicsController()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void iPhysicsController::Update(float a_fTimeStep)
	{
		if (m_bActive == false || m_bPaused) return;
		if (m_pBody == NULL) return;

		cVector3f vInput = GetInputValue(m_InputType);
		//Get the local input
		if (m_bUseInputMatrixFix == false ||
			(m_InputType != ePhysicsControllerInput_JointAngle && m_InputType != ePhysicsControllerInput_JointDist))
			vInput = cMath::MatrixMul(cMath::MatrixInverse(m_pBody->GetLocalMatrix().GetRotation()), vInput);

		float fValue = GetAxisValue(m_InputAxis, vInput);
		float fError = m_fDestValue - fValue;

		float fOutput = GetOutputValue(fError, fValue, a_fTimeStep);

		if (m_fMaxOutput > 0)
		{
			if (fOutput > 0)
				fOutput = cMath::Min(fOutput, m_fMaxOutput);
			else
				fOutput = cMath::Max(fOutput, -m_fMaxOutput);
		}

		if (m_bLogInfo)
			Log("%s | Input: %f Dest: %f Error: %f Output: %f\n", m_sName.c_str(), fValue, m_fDestValue, fError, fOutput);

		AddOutputValue(m_OutputType, m_OutputAxis, fOutput);

		if (m_EndType == ePhysicsControllerEnd_OnDest && m_pJoint)
		{
			if (std::abs(fValue - m_fDestValue) < kEpsilonf)
			{
				m_bActive = false;
				iPhysicsController *pNext = m_pJoint->GetController(m_sNextController);
				if (pNext) pNext->SetActive(true);
			}
		}
	}

	//--------------------------------------------------------------

	void iPhysicsController::SetActive(bool a_bX)
	{
		if (a_bX == m_bActive) return;

		m_PidController.Reset();

		m_bActive = a_bX;
	}

	//--------------------------------------------------------------

	void iPhysicsController::SetPidIntegralSize(int a_lSize)
	{
		m_PidController.SetErrorNum(a_lSize);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cVector3f iPhysicsController::GetInputValue(ePhysicsControllerInput a_Input)
	{
		switch(a_Input)
		{
		case ePhysicsControllerInput_AngularSpeed : return m_pBody ? m_pBody->GetAngularVelocity() : 0;
		case ePhysicsControllerInput_LinearSpeed: return m_pBody ? m_pBody->GetLinearVelocity() : 0;
		case ePhysicsControllerInput_JointAngle: return m_pJoint ? m_pJoint->GetAngle() : 0;
		case ePhysicsControllerInput_JointDist: return m_pJoint ? m_pJoint->GetDistance() : 0;
		}
		return 0;
	}

	//--------------------------------------------------------------

	float  iPhysicsController::GetOutputValue(float a_fError, float a_fInput, float a_fTimeStep)
	{
		if (m_Type == ePhysicsControllerType_Pid)
		{
			m_PidController.p = m_fA;
			m_PidController.i = m_fB;
			m_PidController.d = m_fC;

			return m_PidController.Output(a_fError, a_fTimeStep);
		}
		else
			return a_fError * m_fA - a_fInput * m_fB;

		return 0;
	}

	//--------------------------------------------------------------

	void iPhysicsController::AddOutputValue(ePhysicsControllerOutput a_Output, ePhysicsControllerAxis a_Axis,
			float a_fVal)
	{
		cVector3f vVec(0,0,0);

		switch (a_Axis)
		{
		case efe::ePhysicsControllerAxis_X: vVec.x = a_fVal; break;
		case efe::ePhysicsControllerAxis_Y: vVec.y = a_fVal; break;
		case efe::ePhysicsControllerAxis_Z: vVec.z = a_fVal; break;
		}

		if (m_bMulMassWithOutput) vVec = vVec * m_pBody->GetMass();

		vVec = cMath::MatrixMul(m_pBody->GetLocalMatrix().GetRotation(), vVec);

		switch (a_Output)
		{
		case efe::ePhysicsControllerOutput_Force: m_pBody->AddTorque(vVec); break;
		case efe::ePhysicsControllerOutput_Torque: m_pBody->AddForce(vVec); break;
		}
	}

	//--------------------------------------------------------------

	float iPhysicsController::GetAxisValue(ePhysicsControllerAxis a_Axis, const cVector3f &a_vVec)
	{
		switch (a_Axis)
		{
		case efe::ePhysicsControllerAxis_X: return a_vVec.x;
		case efe::ePhysicsControllerAxis_Y: return a_vVec.y;
		case efe::ePhysicsControllerAxis_Z: return a_vVec.z ;
		}
		return 0;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	kBeginSerializeBase(cSaveData_iPhysicsController)
		kSerializeVar(m_sName, eSerializeType_String)

		kSerializeVar(m_lBodyId, eSerializeType_Int32)
		kSerializeVar(m_lJointId, eSerializeType_Int32)

		kSerializeVar(m_fA, eSerializeType_Float32)
		kSerializeVar(m_fB, eSerializeType_Float32)
		kSerializeVar(m_fC, eSerializeType_Float32)

		kSerializeVar(m_fDestValue, eSerializeType_Float32)
		kSerializeVar(m_fMaxOutput, eSerializeType_Float32)

		kSerializeVar(m_bMulMassWithOutput, eSerializeType_Bool)

		kSerializeVar(m_Type, eSerializeType_Int32)

		kSerializeVar(m_InputType, eSerializeType_Int32)
		kSerializeVar(m_InputAxis, eSerializeType_Int32)

		kSerializeVar(m_OutputType, eSerializeType_Int32)
		kSerializeVar(m_OutputAxis, eSerializeType_Int32)

		kSerializeVar(m_EndType, eSerializeType_Int32)

		kSerializeVar(m_sNextController, eSerializeType_String)

		kSerializeVar(m_bActive, eSerializeType_Bool)
		kSerializeVar(m_bPaused, eSerializeType_Bool)
	kEndSerialize()

	//--------------------------------------------------------------

	iSaveData *iPhysicsController::CreateSaveData()
	{
		return efeNew(cSaveData_iPhysicsController, ());
	}

	//--------------------------------------------------------------

	void iPhysicsController::SaveToSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_SaveToBegin(iPhysicsController);

		kSaveData_SaveTo(m_sName);

		kSaveData_SaveTo(m_fA);
		kSaveData_SaveTo(m_fB);
		kSaveData_SaveTo(m_fC);
		kSaveData_SaveTo(m_fDestValue);
		kSaveData_SaveTo(m_fMaxOutput);

		kSaveData_SaveTo(m_bMulMassWithOutput);

		kSaveData_SaveTo(m_Type);
		kSaveData_SaveTo(m_InputType);
		kSaveData_SaveTo(m_InputAxis);
		kSaveData_SaveTo(m_OutputType);
		kSaveData_SaveTo(m_OutputAxis);
		kSaveData_SaveTo(m_EndType);

		kSaveData_SaveTo(m_sNextController);

		kSaveData_SaveTo(m_bActive);
		kSaveData_SaveTo(m_bPaused);

		kSaveData_SaveObject(m_pBody, m_lBodyId);
		kSaveData_SaveObject(m_pJoint, m_lJointId);
	}

	//--------------------------------------------------------------v

	void iPhysicsController::LoadFromSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_LoadFromBegin(iPhysicsController);

		kSaveData_LoadFrom(m_sName);

		kSaveData_LoadFrom(m_fA);
		kSaveData_LoadFrom(m_fB);
		kSaveData_LoadFrom(m_fC);
		kSaveData_LoadFrom(m_fDestValue);
		kSaveData_LoadFrom(m_fMaxOutput);

		kSaveData_LoadFrom(m_bMulMassWithOutput);

		m_Type = (ePhysicsControllerType)pData->m_Type;
		m_InputType = (ePhysicsControllerInput)pData->m_InputType;
		m_InputAxis = (ePhysicsControllerAxis)pData->m_InputAxis;
		m_OutputType = (ePhysicsControllerOutput)pData->m_OutputType;
		m_OutputAxis = (ePhysicsControllerAxis)pData->m_OutputAxis;
		m_EndType = (ePhysicsControllerEnd)pData->m_EndType;

		kSaveData_LoadFrom(m_sNextController);

		kSaveData_LoadFrom(m_bActive);
		kSaveData_LoadFrom(m_bPaused);
	}

	//--------------------------------------------------------------

	void iPhysicsController::SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		kSaveData_SetupBegin(iPhysicsController);

		kSaveData_LoadObject(m_pBody, m_lBodyId, iPhysicsBody*);
		kSaveData_LoadObject(m_pJoint, m_lJointId, iPhysicsJoint*);
	}
}