#ifndef EFE_CHARACTER_BODY_H
#define EFE_CHARACTER_BODY_H

#include "math/MathTypes.h"
#include "physics/PhysicsWorld.h"

namespace efe
{
	class iPhysicsWorld;
	class iCollideShape;
	class iPhysicsBody;
	class cCamera3D;
	class iCharacterBody;
	class iEntity3D;

	enum eCharDir
	{
		eCharDir_Forward = 0,
		eCharDir_Right = 1,
		eCharDir_LastEnum = 2
	};

	class iCharacterBodyCallback
	{
	public:
		virtual void OnHitGround(iCharacterBody *a_pCharBody, const cVector3f &a_vVel)=0;
		virtual void OnGravityCollide(iCharacterBody *a_pCharBody, iPhysicsBody *a_pCollideBody,
									cCollideData *a_pCollideData)=0;
	};

	class cCharacterBodyRay : public iPhysicsRayCallback
	{
	public:
		cCharacterBodyRay();

		void Clear();
		bool OnIntersect(iPhysicsBody *a_pBody, cPhysicsRayParams *a_pParams);

		float m_fMinDist;
		bool m_bCollide;
	};

	//--------------------------------------------------------------

	class cCharacterBodyCollideGravity : public iPhysicsWorldCollisionCallback
	{
	public:
		cCharacterBodyCollideGravity();

		void OnCollision(iPhysicsBody *a_pBody, cCollideData *a_pCollideData);

		iCharacterBody *m_pCharBody;
	};

	//--------------------------------------------------------------

	class cCharacterBodyCollidePush : public iPhysicsWorldCollisionCallback
	{
	public:
		cCharacterBodyCollidePush();

		void OnCollision(iPhysicsBody *a_pBody, cCollideData *a_pCollideData);

		iCharacterBody *m_pCharBody;
	};

	//--------------------------------------------------------------

	class iCharacterBody
	{
		friend class cCharacterBodyCollideGravity;
	public:
		iCharacterBody(const tString &a_sName, iPhysicsWorld *a_pWorld, const cVector3f a_vSize);
		virtual ~iCharacterBody();

		const tString &GetName(){return m_sName;}

		//Properties

		float GetMass();
		void SetMass(float a_fMass);

		void SetActive(bool a_bX);
		bool IsActive(){return m_bActive;}

		cVector3f GetSize();

		void SetCollideCharacter(bool a_bX);
		bool GetCollideCharacter(){return m_bCollideCharacter;}

		void SetTestCollision(bool a_bX){m_bTestCollision = a_bX;}
		bool GetTestCollision(){return m_bTestCollision;}

		void SetMaxPositiveMoveSpeed(eCharDir a_Dir, float a_fX);
		float GetMaxPositiveMoveSpeed(eCharDir a_Dir);
		void SetMaxNegativeMoveSpeed(eCharDir a_Dir, float a_fX);
		float GetMaxNegativeSpeed(eCharDir a_Dir);

		void SetMoveSpeed(eCharDir a_Dir, float a_fX);
		float GetMoveSpeed(eCharDir a_Dir);
		void SetMoveAcc(eCharDir a_Dir, float a_fX);
		float GetMoveAcc(eCharDir a_Dir);
		void SetMoveDeacc(eCharDir a_Dir, float a_fX);
		float GetMoveDeacc(eCharDir a_Dir);

		cVector3f GetVelocity(float a_fFrameTime);

		void SetPosition(const cVector3f &a_vPos, bool a_bSmooth = false);
		const cVector3f &GetPosition();
		const cVector3f &GetLastPosition();
		void SetFeetPosition(const cVector3f &a_vPos, bool a_bSmooth = false);
		const cVector3f &GetFeetPosition();

		void SetYaw(float a_fX);
		void AddYaw(float a_fX);
		float GetYaw();
		void SetPitch(float a_fX);
		void AddPitch(float a_fX);
		float GetPitch();

		cVector3f GetForward();
		cVector3f GetRight();
		cVector3f GetUp();

		cMatrixf &GetMoveMatrix();

		void SetGravityActive(bool a_bX);
		bool GravityIsActive();
		void SetMaxGravitySpeed(float a_fX);
		float GetMaxGravitySpeed();

		bool GetCustomGravityActive();
		void SetCustomGravityActive(bool a_bX);
		void SetCustomGravity(const cVector3f &a_vCustomGravity);
		cVector3f GetCustomGravity();

		void SetMaxPushMass(float a_fX){m_fMaxPushMass = a_fX;}
		void SetPushForce(float a_fX){m_fPushForce = a_fX;}
		float GetMaxPushMass(){return m_fMaxPushMass;}
		float GetPushForce(){return m_fPushForce;}

		bool GetPushIn2D(){return m_bPushIn2D;}
		void SetPushIn2D(bool a_bX){m_bPushIn2D = a_bX;}

		void AddForceVelocity(const cVector3f &a_vVel){m_vVelocity += a_vVel;}
		void SetForceVelocity(const cVector3f &a_vVel){m_vVelocity = a_vVel;}
		cVector3f GetForceVelocity(){return m_vVelocity;}

		int AddExtraSize(const cVector3f &a_vSize);
		void SetActiveSize(int a_lNum);

		//Actions
		void SetForce(const cVector3f &a_vForce);
		void AddForce(const cVector3f &a_vForce);
		cVector3f GetForce();

		void Move(eCharDir a_Dir, float a_fMul, float a_fTimeStep);

		void Update(float a_fTimeStep);

		//Other
		void SetCamera(cCamera3D *a_Cam);
		cCamera3D *GetCamera();
		void SetCameraPosAdd(const cVector3f &a_vAdd);
		cVector3f GetCameraPosAdd();
		void SetCameraSmoothPosNum(int a_lNum){m_lCameraSmoothPosNum = a_lNum;}
		int GetCameraSmoothPosNum(){return m_lCameraSmoothPosNum;}

		void SetEntity(iEntity3D *a_pEntity);
		iEntity3D *GetEntity();

		iPhysicsBody *GetBody(){return m_pBody;}
		iCollideShape *GetShape();

		void SetAttachedBody(iPhysicsBody *a_pBody);
		iPhysicsBody *GetAttachedBody(){return m_pAttachedBody;}

		void UpdateMoveMatrix();

		void UpdateCamera();
		void UpdateEntity();

		void UpdateAttachment();


	protected:
		tString m_sName;

		float m_fMass;

		bool m_bActive;

		bool m_bCollideCharacter;

		bool m_bTestCollision;

		bool m_bGravityActive;
		float m_fMaxGravitySpeed;
		bool m_bCustomGravity;
		cVector3f m_vCustomGravity;

		cVector3f m_vPosition;
		cVector3f m_vLastPosition;

		float m_fMaxPosMoveSpeed[2];
		float m_fMaxNegMoveSpeed[2];

		float m_fMoveSpeed[2];
		float m_fMoveAcc[2];
		float m_fMoveDeacc[2];
		bool m_bMoving[2];

		float m_fPitch;
		float m_fYaw;

		bool m_bOnGround;

		float m_fMaxPushMass;
		float m_fPushForce;
		bool m_bPushIn2D;

		float m_fCheckStepClimbCount;
		float m_fCheckStepClimbInterval;

		cVector3f m_vForce;
		cVector3f m_vVelocity;

		cVector3f m_vSize;

		cMatrixf m_mtxMove;

		cCamera3D *m_pCamera;
		cVector3f m_vCameraPosAdd;
		int m_lCameraSmoothPosNum;
		tVector3fList m_lstCameraPos;

		iEntity3D *m_pEntity;
		cMatrixf m_mtxEntityOffset;
		cMatrixf m_mtxEntityPostOffset;
		int m_lEntitySmoothPosNum;
		tVector3fList m_lstEntityPos;

		float m_fGroundFriction;
		float m_fAirFriction;

		void *m_pUserData;

		iPhysicsBody *m_pAttachedBody;
		cMatrixf m_matxAttachedPrevMatrix;
		bool m_bAttachmentJustAdded;

		iCharacterBodyCallback *m_pCallback;

		cCharacterBodyRay *m_pRayCallback;

		cCharacterBodyCollideGravity *m_pCallbackCollideGravity;
		cCharacterBodyCollidePush *m_pCallbackCollidePush;

		float m_fMaxStepHeight;
		float m_fStepClimbSpeed;
		float m_fClimbForwardMul;
		float m_fClimbHeightAdd;
		bool m_bClimbing;
		bool m_bAccurateClimbing;

		bool m_bEnableNearbyBodies;

		iPhysicsBody *m_pBody;
		iPhysicsWorld *m_pWorld;

		std::vector<iPhysicsBody*> m_vExtraBodies;
	};
};
#endif