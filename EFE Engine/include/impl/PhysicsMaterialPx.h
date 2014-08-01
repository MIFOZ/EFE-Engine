#ifndef EFE_PHYSICS_MATERIAL_PX_H
#define EFE_PHYSICS_MATERIAL_PX_H

#include <PxPhysicsAPI.h>

#include "physics/PhysicsMaterial.h"

namespace efe
{
	using namespace physx;

	class iPhysicsBody;
	class cPhysicsContactData;

	class cPhysicsMaterialPx : public iPhysicsMaterial
	{
	public:
		cPhysicsMaterialPx(const tString &a_sName, iPhysicsWorld *a_pWorld, int a_lMatId=-1);
		~cPhysicsMaterialPx();

		void SetElasticity(float a_fElasctictiy);
		float GetElasticity() const;
		void SetStaticFriction(float a_fFriction);
		float GetStaticFriction() const;
		void SetKineticFriction(float a_fFriction);
		float GetKineticFriction() const;

		void SetFrictionCombMode(ePhysicsMaterialCombMode a_Mode);

		void SetElasticityCombMode(ePhysicsMaterialCombMode a_Mode);

		void UpdateMaterials();

		PxMaterial *GetPxMaterial(){return m_pPxMaterial;}
	private:
		PxCombineMode::Enum GetPxCombineModeEnum(ePhysicsMaterialCombMode a_Mode);

		static iPhysicsBody *m_pContatctBody1;
		static iPhysicsBody *m_pContatctBody2;
		static cPhysicsContactData m_ContactData;
		static int m_lContactNum;

		PxPhysics *m_pDSK;

		PxMaterial *m_pPxMaterial;

		ePhysicsMaterialCombMode m_FrictionMode;
		ePhysicsMaterialCombMode m_ElasticityMode;

		float m_fElasticity;
		float m_fStaticFriction;
		float m_fKinectFriction;
	};
};
#endif