#ifndef EFE_PHYSICS_BODY_H
#define EFE_PHYSICS_BODY_H

#include "scene/Entity3D.h"
#include "graphics/GraphicsTypes.h"

namespace efe
{
	class iPhysicsWorld;
	class iCollideShape;
	class iPhysicsMaterial;
	class iLowLevelGraphics;
	class cNode3D;
	class cSoundEntity;
	class iPhysicsJoint;
	class cPhysicsContactData;
	class iCharacterBody;
	class iHapticShape;

	class iPhysicsBody;
	class iPhysicsBodyCallback
	{
	public:
		virtual bool OnBeginCollision(iPhysicsBody *a_pBody, iPhysicsBody *a_pCollideBody)=0;
		virtual void OnCollide(iPhysicsBody *a_pBody, iPhysicsBody *a_pCollideBody,
								cPhysicsContactData *a_pContactData)=0;
	};

	typedef std::list<iPhysicsBodyCallback*> tPhysicsBodyCallbackList;
	typedef tPhysicsBodyCallbackList::iterator tPhysicsBodyCallbackListIt;

	struct cPhysicsBody_Buoyancy
	{
		cPhysicsBody_Buoyancy() : m_bActive(false), m_fDensity(1),
								m_fLinearViscosity(1), m_fAngularViscosity(1){}

		bool m_bActive;

		float m_fDensity;
		float m_fLinearViscosity;
		float m_fAngularViscosity;

		cPlanef m_Surface;
	};

	class iPhysicsBody : public iEntity3D
	{
	public:
		iPhysicsBody(const tString &a_sName, iPhysicsWorld *a_pWorld, iCollideShape *a_pShape);
		virtual ~iPhysicsBody();

		void Destroy();

		virtual void SetMaterial(iPhysicsMaterial *a_pMaterial)=0;
		iPhysicsMaterial *GetMaterial();

		cNode3D *GetNode();
		cNode3D *CreateNode();

		iCollideShape *GetShape();

		void AddJoint(iPhysicsJoint *a_pJoint);
		iPhysicsJoint *GetJoint(int a_lIndex);
		int GetJointNum();
		void RemoveJoint(iPhysicsJoint *a_pJoint);

		virtual void SetLinearVelocity(const cVector3f &a_vVel)=0;
		virtual cVector3f GetLinearVelocity()const=0;
		virtual void SetAngularVelocity(const cVector3f &a_vVel)=0;
		virtual cVector3f GetAngularVelocity()const=0;
		virtual void SetLinearDamping(float a_fDamping)=0;
		virtual float GetLinearDamping()const=0;
		virtual void SetAngularDamping(float a_fDamping)=0;
		virtual float GetAngularDamping()const=0;
		virtual void SetMaxLinearSpeed(float a_fSpeed)=0;
		virtual float GetMaxLinearSpeed()const=0;
		virtual void SetMaxAngularSpeed(float a_fSpeed)=0;
		virtual float GetMaxAngularSpeed()const=0;
		virtual cMatrixf GetInertiaMatrix()=0;

		cVector3f GetVelocityAtPosition(cVector3f a_vPos);

		virtual void SetMass(float a_fMass)=0;
		virtual float GetMass()const=0;

		virtual cVector3f GetMassCentre()const=0;

		virtual void AddForce(const cVector3f &a_vForce)=0;
		virtual void AddForceAtPosition(const cVector3f &a_vForce, const cVector3f &a_vPos)=0;
		virtual void AddTorque(const cVector3f &a_vTorque)=0;

		virtual void SetEnabled(bool a_bEnabled)=0;
		virtual bool GetEnabled() const=0;
		virtual void SetAutoDisable(bool a_bEnabled)=0;
		virtual bool GetAutoDisable() const=0;
		virtual void SetContinuousCollision(bool a_bOn)=0;
		virtual bool GetContinuousCollision()=0;

		cBoundingVolume *GetBV(){return &m_BoundingVolume;}

		void SetBlocksSound(bool a_bX){m_bBlocksSound = a_bX;}
		bool GetBlocksSound(){return m_bBlocksSound;}

		virtual void SetGravity(bool a_bEnabled)=0;
		virtual bool GetGravity()const=0;

		virtual void RenderDebugGeometry(iLowLevelGraphics *a_pLowLevel, const cColor &a_Color)=0;

		void UpdateBeforeSimulate(float a_fTimeStep);
		void UpdateAfterSimulate(float a_fTimeStep);
		
		void SetScrapeSoundEntity(cSoundEntity *a_pEntity){m_pScrapeSoundEntity = a_pEntity;}
		cSoundEntity *GetScrapeSoundEntity(){return m_pScrapeSoundEntity;}
		void SetScrapeBody(iPhysicsBody *a_pBody){m_pScrapeBody = a_pBody;}
		iPhysicsBody *GetScrapeBody() {return m_pScrapeBody;}
		const cMatrixf &GetPrevScrapeMatrix() {return m_mtxPrevScrapeMatrix;}
		void SetPrevScrapeMatrix(const cMatrixf &a_mtxMtx) {m_mtxPrevScrapeMatrix = a_mtxMtx;}

		void SetRollSoundEntity(cSoundEntity *a_pEntity){m_pRollSoundEntity = a_pEntity;}
		cSoundEntity *GetRollSoundEntity(){return m_pRollSoundEntity;}

		void SetHasImpact(bool a_bX){m_bHasImpact = a_bX;}
		bool HasImpact(){return m_bHasImpact;}
		void SetHasSlide(bool a_bX){m_bHasSlide = a_bX;}
		bool HasSlide(){return m_bHasSlide;}

		bool OnBeginCollision(iPhysicsBody *a_pBody);
		void OnCollide(iPhysicsBody *a_pBody, cPhysicsContactData *a_pContactData);

		void SetCollide(bool a_bX) {m_bCollide = a_bX;}
		bool GetCollide() {return m_bCollide;}

		void SetIsCharacter(bool a_bX){m_bIsCharacter = a_bX;}
		bool IsCharacter(){return m_bIsCharacter;}

		void SetCollideCharacter(bool a_bX){m_bCollideCharacter = a_bX;}
		bool GetCollideCharacter(){return m_bCollideCharacter;}

		void SetCharacterBody(iCharacterBody *a_pCharBody) {m_pCharacterBody = a_pCharBody;}
		iCharacterBody *GetCharacterBody() {return m_pCharacterBody;}

		void SetIsRagDoll(bool a_bX) {m_bIsRagDoll = a_bX;}
		bool IsRagDoll() {return m_bIsRagDoll;}

		void SetCollideRagDoll(bool a_bX) {m_bCollideRagDoll = a_bX;}
		bool GetCollideRagDoll() {return m_bCollideRagDoll;}

		void SetVolatile(bool a_bX) {m_bVolatile = a_bX;}
		bool GetVolatile() {return m_bVolatile;}

		void SetPushedByCharacterGravity(bool a_bX){m_bPushedByCharacterGravity = a_bX;}
		bool GetPushedByCharacterGravity(){return m_bPushedByCharacterGravity;}

		void SetCanAttachCharacter(bool a_bX) {m_bCanAttachCharacter = a_bX;}
		bool GetCanAttachCharacter() {return m_bCanAttachCharacter;}
		void AddAttachedCharacter(iCharacterBody *a_pChar);
		void RemoveAttachedCharacter(iCharacterBody *a_pChar);

		iPhysicsWorld *GetWorld() {return m_pWorld;}

		void DisableAfterSimulation() {m_bDisableAfterSimulation = true;}

		//Entity implementation
		tString GetEntityType(){return "Body";}

		virtual void DeleteLowLevel()=0;
	protected:
		iPhysicsWorld *m_pWorld;
		iCollideShape *m_pShape;
		iPhysicsMaterial *m_pMaterial;
		cNode3D *m_pNode;

		iCharacterBody *m_pCharacterBody;

		std::vector<iPhysicsJoint*> m_vJoints;

		std::list<iCharacterBody*> m_lstAttachedCharacters;

		iPhysicsBody *m_pScrapeBody;
		cSoundEntity *m_pScrapeSoundEntity;
		cSoundEntity *m_pRollSoundEntity;
		cMatrixf m_mtxPrevScrapeMatrix;
		bool m_bHasImpact;
		bool m_bHasSlide;
		int m_lSlideCount;
		int m_lImpactCount;

		bool m_bPushedByCharacterGravity;

		bool m_bBlocksSound;
		bool m_bBlocksLight;
		bool m_bIsCharacter;
		bool m_bCollideCharacter;
		bool m_bIsRagDoll;
		bool m_bCollideRagDoll;
		bool m_bVolatile;

		bool m_bCanAttachCharacter;

		cPhysicsBody_Buoyancy m_Buoyancy;
		int m_lBuoyancy;

		bool m_bDisableAfterSimulation;

		bool m_bHasCollision;

		tPhysicsBodyCallbackList m_lstBodyCallbacks;

		iHapticShape *m_pHapticsShape;

		void *m_pUserData;

		bool m_bCollide;
	};
};
#endif