#ifndef EFE_PHYSICS_CONTROLLER_H
#define EFE_PHYSICS_CONTROLLER_H

#include "math/MathTypes.h"
#include "math/PidController.h"

#include "game/SaveGame.h"

namespace efe
{
	enum ePhysicsControllerType
	{
		ePhysicsControllerType_Pid,
		ePhysicsControllerType_Spring,
		ePhysicsControllerType_LastEnum
	};

	enum ePhysicsControllerInput
	{
		ePhysicsControllerInput_JointAngle,
		ePhysicsControllerInput_JointDist,
		ePhysicsControllerInput_LinearSpeed,
		ePhysicsControllerInput_AngularSpeed,
		ePhysicsControllerInput_LastEnum
	};

	enum ePhysicsControllerOutput
	{
		ePhysicsControllerOutput_Force,
		ePhysicsControllerOutput_Torque,
		ePhysicsControllerOutput_LastEnum
	};

	enum ePhysicsControllerAxis
	{
		ePhysicsControllerAxis_X,
		ePhysicsControllerAxis_Y,
		ePhysicsControllerAxis_Z,
		ePhysicsControllerAxis_LastEnum
	};

	enum ePhysicsControllerEnd
	{
		ePhysicsControllerEnd_Null,
		ePhysicsControllerEnd_OnDest,
		ePhysicsControllerEnd_OnMin,
		ePhysicsControllerEnd_OnMax,
		ePhysicsControllerEnd_LastEnum
	};

	kSaveData_BaseClass(iPhysicsController)
	{
		kSaveData_ClassInit(iPhysicsController)
	public:
		tString m_sName;

		int m_lBodyId;
		int m_lJointId;

		float m_fA;
		float m_fB;
		float m_fC;
		float m_fDestValue;
		float m_fMaxOutput;

		bool m_bMulMassWithOutput;

		int m_Type;
		int m_InputType;
		int m_InputAxis;
		int m_OutputType;
		int m_OutputAxis;
		int m_EndType;

		tString m_sNextController;

		bool m_bActive;
		bool m_bPaused;

		iSaveObject *CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame) {return NULL;}
		int GetSaveCreatePrio() {return 0;}
	};

	class iPhysicsWorld;
	class iPhysicsJoint;
	class iPhysicsBody;

	class iPhysicsController : public iSaveObject
	{
	public:
		iPhysicsController(const tString &a_sName, iPhysicsWorld *a_pWorld);
		virtual ~iPhysicsController();

		void Update(float a_fTimeStep);

		const tString &GetName() {return m_sName;}

		void SetJoint(iPhysicsJoint *a_pJoint) {m_pJoint = a_pJoint;}
		iPhysicsJoint *GetJoint() {return m_pJoint;}
		void SetBody(iPhysicsBody *a_pBody) {m_pBody = a_pBody;}
		iPhysicsBody *GetBody() {return m_pBody;}

		bool IsActive() {return m_bActive;}
		void SetActive(bool a_bX);

		// p in Pid and k in springs
		void SetA(float a_fA) {m_fA = a_fA;}

		// i in Pid and b in springs
		void SetB(float a_fB) {m_fB = a_fB;}

		// d in Pid and no used in springs
		void SetC(float a_fC) {m_fC = a_fC;}

		void SetPidIntegralSize(int a_lSize);

		void SetType(ePhysicsControllerType a_Type) {m_Type = a_Type;}

		void SetDestValue(float a_fX) {m_fDestValue = a_fX;}
		float GetDestValue() {return m_fDestValue;}

		void SetMaxOutput(float a_fX) {m_fMaxOutput = a_fX;}

		void SetInputType(ePhysicsControllerInput a_Input, ePhysicsControllerAxis a_Axis){
			m_InputType = a_Input; m_InputAxis = a_Axis;}

		void SetOutputType(ePhysicsControllerOutput a_Output, ePhysicsControllerAxis a_Axis){
			m_OutputType = a_Output; m_OutputAxis = a_Axis;}

		void SetMulMassWithOutput(bool a_bX) {m_bMulMassWithOutput = a_bX;}

		void SetEndType(ePhysicsControllerEnd a_End) {m_EndType = a_End;}
		ePhysicsControllerEnd GetEndType() {return m_EndType;}

		void SetNextController(const tString &a_sName) {m_sNextController = a_sName;}
		const tString &GetNextController() {return m_sNextController;}

		void SetLogInfo(bool a_bX) {m_bLogInfo = a_bX;}

		void SetPaused(bool a_bX) {m_bPaused = a_bX;}

		static bool m_bUseInputMatrixFix;

		//SaveObject implementation
		virtual iSaveData *CreateSaveData();
		virtual void SaveToSaveData(iSaveData *a_pSaveData);
		virtual void LoadFromSaveData(iSaveData *a_pSaveData);
		virtual void SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);

	protected:
		cVector3f GetInputValue(ePhysicsControllerInput a_Input);
		float  GetOutputValue(float a_fError, float a_fInput, float a_fTimeStep);
		void AddOutputValue(ePhysicsControllerOutput a_Output, ePhysicsControllerAxis a_Axis,
			float a_fVal);
		float GetAxisValue(ePhysicsControllerAxis a_Axis, const cVector3f &a_vVec);

		iPhysicsWorld *m_pWolrd;
		tString m_sName;

		iPhysicsBody *m_pBody;
		iPhysicsJoint *m_pJoint;

		float m_fA, m_fB, m_fC;

		float m_fDestValue;
		float m_fMaxOutput;

		bool m_bMulMassWithOutput;

		ePhysicsControllerType m_Type;

		ePhysicsControllerInput m_InputType;
		ePhysicsControllerAxis m_InputAxis;

		ePhysicsControllerOutput m_OutputType;
		ePhysicsControllerAxis m_OutputAxis;

		ePhysicsControllerEnd m_EndType;

		tString m_sNextController;

		cPidControllerf m_PidController;

		bool m_bActive;
		bool m_bPaused;

		bool m_bLogInfo;
	};
};

#endif