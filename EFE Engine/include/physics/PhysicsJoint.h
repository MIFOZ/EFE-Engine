#ifndef EFE_PHYSICS_JOINT_H
#define EFE_PHYSICS_JOINT_H

#include <map>
#include "system/SystemTypes.h"
#include "math/MathTypes.h"

#include "game/SaveGame.h"

#include "physics/PhysicsController.h"

namespace efe
{
	class iPhysicsBody;
	class iPhysicsWorld;
	class cSoundEntity;
	class iPhysicsJoint;

	typedef std::map<tString, iPhysicsController*> tPhysicsControllerMap;
	typedef tPhysicsControllerMap::iterator tPhysicsControllerMapIt;

	typedef cSTLMapIterator<iPhysicsController*, tPhysicsControllerMap, tPhysicsControllerMapIt> cPhysicsControllerIterator;

	enum ePhysicsJointType
	{
		ePhysicsJointType_Ball,
		ePhysicsJointType_Hinge,
		ePhysicsJointType_Slider,
		ePhysicsJointType_Screw,
		ePhysicsJointType_LastEnum
	};

	enum ePhysicsJointSpeed
	{
		ePhysicsJointSpeed_Linear,
		ePhysicsJointSpeed_Angular,
		ePhysicsJointSpeed_LastEnum
	};

	class cJointLimitEffect : public iSerializable
	{
		kSerializableClassInit(cJointLimitEffect)
	public:
		tString m_sSound;
		float m_fMinSpeed;
		float m_fMaxSpeed;
	};

	class iPhysicsJointCallback
	{
	public:
		virtual ~iPhysicsJointCallback(){}

		virtual void OnMinLimit(iPhysicsJoint *a_pJoint)=0;
		virtual void OnMaxLimit(iPhysicsJoint *a_pJoint)=0;

		virtual bool IsScript(){return false;}
	};

	kSaveData_BaseClass(iPhysicsJoint)
	{
		kSaveData_ClassInit(iPhysicsJoint)
	public:
		tString m_sName;

		int m_lParentBodyId;
		int m_lChildBodyId;

		cMatrixf m_mtxParentBodySetup;
		cMatrixf m_mtxChildBodySetup;

		cVector3f m_vPinDir;
		cVector3f m_vStartPivotPoint;

		cContainerList<cSaveData_iPhysicsController> m_lstControllers;

		cJointLimitEffect m_MaxLimit;
		cJointLimitEffect m_MinLimit;

		tString m_sMoveSound;

		float m_fMinMoveSpeed;
		float m_fMinMoveFreq;
		float m_fMinMoveFreqSpeed;
		float m_fMinMoveVolume;
		float m_fMaxMoveFreq;
		float m_fMaxMoveFreqSpeed;
		float m_fMaxMoveVolume;
		float m_fMiddleMoveSpeed;
		float m_fMiddleMoveVolume;
		int m_MoveSpeedType;

		bool m_bBreakable;
		float m_fBreakForce;
		tString m_sBreakSound;
		bool m_bBroken;

		tString m_sCallbackMaxFunc;
		tString m_sCallbackMinFunc;
		bool m_bAutoDeleteCallback;
	};

	class iPhysicsJoint : public iSaveObject
	{
	public:
		iPhysicsJoint(const tString &a_sName, iPhysicsBody *a_pParentBody, iPhysicsBody *a_pChildBody, 
			iPhysicsWorld *a_pWorld, const cVector3f &a_vPivotPoint);
		virtual ~iPhysicsJoint();

		const tString &GetName(){return m_sName;}

		iPhysicsBody *GetParentBody(){return m_pParentBody;}
		iPhysicsBody *GetChildBody(){return m_pChildBody;}

		void RemoveBody(iPhysicsBody *a_pBody);

		cVector3f GetPivotPoint() {return m_vPivotPoint;}
		cVector3f GetPinDir() {return m_vPinDir;}

		virtual ePhysicsJointType GetType() = 0;

		virtual void SetCollideBodies(bool a_bX) = 0;
		virtual bool GetCollideBodies() = 0;

		virtual void SetStiffness(float a_fX) = 0;
		virtual float GetStiffness() = 0;

		virtual cVector3f GetVelocity() = 0;
		virtual cVector3f GetAngularVelocity() = 0;
		virtual cVector3f GetForce() = 0;

		virtual float GetDistance() = 0;
		virtual float GetAngle() = 0;

		cJointLimitEffect *GetMaxLimit(){return &m_MaxLimit;}
		cJointLimitEffect *GetMinLimit(){return &m_MinLimit;}

		void SetMoveSound(tString &a_sName) {m_sMoveSound = a_sName;}

		void SetMoveSpeedType(ePhysicsJointSpeed a_Type) {m_MoveSpeedType = a_Type;}
		void SetMinMoveSpeed(float a_fX) {m_fMinMoveSpeed = a_fX;}
		void SetMinMoveFreq(float a_fX) {m_fMinMoveFreq = a_fX;}
		void SetMinMoveFreqSpeed(float a_fX) {m_fMinMoveFreqSpeed = a_fX;}
		void SetMinMoveVolume(float a_fX) {m_fMinMoveVolume = a_fX;}
		void SetMaxMoveFreq(float a_fX) {m_fMaxMoveFreq = a_fX;}
		void SetMaxMoveFreqSpeed(float a_fX) {m_fMaxMoveFreqSpeed = a_fX;}
		void SetMaxMoveVolume(float a_fX) {m_fMaxMoveVolume = a_fX;}
		void SetMiddleMoveSpeed(float a_fX) {m_fMiddleMoveSpeed = a_fX;}
		void SetMiddleMoveVolume(float a_fX) {m_fMiddleMoveVolume = a_fX;}

		void SetCallback(iPhysicsJointCallback *a_pCallback, bool a_bAutoDelete)
		{
			m_pCallback = a_pCallback;
			m_bAutoDeleteCallback = a_bAutoDelete;
		}

		iPhysicsJointCallback *GetCallback() {return m_pCallback;}

		bool CheckBreakage();

		void SetBreakable(bool a_bX) {m_bBreakable = a_bX;}
		bool IsBreakable() {return m_bBreakable;}
		void SetBreakForce(float a_fForce) {m_fBreakForce = a_fForce;}
		float GetBreakForce() {return m_fBreakForce;}
		void SetBreakSound(const tString &a_sSound) {m_sBreakSound = a_sSound;}

		void SetLimitAutoSleep(bool a_bX) {m_bLimitAutoSleep = a_bX;}
		void SetLimitAutoSleepDist(float a_fX) {m_fLimitAutoSleepDist = a_fX;}
		void SetLimitAutoSleepNumSteps(int a_lX) {m_lLimitAutoSleepNumSteps = a_lX;}

		bool GetLimitAutoSleep() {return m_bLimitAutoSleep;}
		float GetLimitAutoSleepDist() {return m_fLimitAutoSleepDist;}
		int GetLimitAutoSleepNumSteps() {return m_lLimitAutoSleepNumSteps;}

		void SetStickyMinDistance(float a_fX) {m_fStickyMinDistance = a_fX;}
		void SetStickyMaxDistance(float a_fX) {m_fStickyMaxDistance = a_fX;}
		float GetStickyMinDistance() {m_fStickyMinDistance;}
		float GetStickyMaxDistance() {m_fStickyMaxDistance;}

		void Break();

		bool IsBroken() {return m_bBroken;}

		void SetUserData(void *a_pUserData) {m_pUserData = a_pUserData;}

		void AddController(iPhysicsController *a_pController);
		iPhysicsController *GetController(const tString &a_sName);
		bool ChangeController(const tString &a_sName);
		cPhysicsControllerIterator GetControllerIterator();

		void SetAllControllerPaused(bool a_bX);

		void OnPhysicsUpdate();

		void SetSound(cSoundEntity *a_pSound) {m_pSound = a_pSound;}
		cSoundEntity *GetSound() {return m_pSound;}

		//SaveObject implementation
		virtual iSaveData *CreateSaveData();
		virtual void SaveToSaveData(iSaveData *a_pSaveData);
		virtual void LoadFromSaveData(iSaveData *a_pSaveData);
		virtual void SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);

	protected:
		tString m_sName;

		iPhysicsBody *m_pParentBody;
		iPhysicsBody *m_pChildBody;
		iPhysicsWorld *m_pWorld;

		cMatrixf m_mtxParentBodySetup;
		cMatrixf m_mtxChildBodySetup;

		cVector3f m_vPinDir;
		cVector3f m_vPivotPoint;
		cVector3f m_vStartPivotPoint;

		cVector3f m_vLocalPivot;

		float m_fStickyMinDistance;
		float m_fStickyMaxDistance;

		tPhysicsControllerMap m_mapControllers;

		cJointLimitEffect m_MaxLimit;
		cJointLimitEffect m_MinLimit;

		int m_lSpeedCount;

		cMatrixf m_mtxPrevChild;
		cMatrixf m_mtxPrevParent;

		tString m_sMoveSound;

		float m_fMinMoveSpeed;
		float m_fMinMoveFreq;
		float m_fMinMoveFreqSpeed;
		float m_fMinMoveVolume;
		float m_fMaxMoveFreq;
		float m_fMaxMoveFreqSpeed;
		float m_fMaxMoveVolume;
		float m_fMiddleMoveSpeed;
		float m_fMiddleMoveVolume;
		ePhysicsJointSpeed m_MoveSpeedType;

		bool m_bBreakable;
		float m_fBreakForce;
		tString m_sBreakSound;
		bool m_bBroken;

		bool m_bLimitAutoSleep;
		float m_fLimitAutoSleepDist;
		int m_lLimitAutoSleepNumSteps;

		cSoundEntity *m_pSound;
		bool m_bHasCollided;

		iPhysicsJointCallback *m_pCallback;
		bool m_bAutoDeleteCallback;

		int m_lLimitStepCount;

		void *m_pUserData;

		static void CheckLimitAutoSleep(iPhysicsJoint *a_pJoint, const float a_fMin, const float a_fMax,
										const float a_fDist);

		void OnMaxLimit();
		void OnMinLimit();
		void OnNoLimit();

		void CalcSoundFreq(float a_fSpeed, float *a_pFreq, float *a_pVol);

		void LimitEffect(cJointLimitEffect *a_pEffect);
	};
};
#endif