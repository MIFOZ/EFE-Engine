#ifndef EFE_PHYSICS_MATERIAL_NEWTON_H
#define EFE_PHYSICS_MATERIAL_NEWTON_H

#include <Newton.h>

#include "physics/PhysicsMaterial.h"

namespace efe
{
	class iPhysicsBody;
	class cPhysicsContactData;

	class cPhysicsMaterialNewton : public iPhysicsMaterial
	{
	public:
		cPhysicsMaterialNewton(const tString &a_sName, iPhysicsWorld *a_pWorld, int a_lMatId=-1);
		virtual ~cPhysicsMaterialNewton();

		void SetElasticity(float a_fElasctictiy);
		float GetElasticity() const;
		void SetStaticFriction(float a_fFriction);
		float GetStaticFriction() const;
		void SetKineticFriction(float a_fFriction);
		float GetKineticFriction() const;

		void SetFrictionCombMode(ePhysicsMaterialCombMode a_Mode);

		void SetElasticityCombMode(ePhysicsMaterialCombMode a_Mode);

		void UpdateMaterials();

		int GetId(){return m_lMaterialId;}
	private:
		float Combine(ePhysicsMaterialCombMode a_Mode, float a_fX, float a_fY);

		static int BeginContactCallback(const NewtonMaterial *a_pMaterial,
			const NewtonBody *a_pBody1, const NewtonBody *a_pBody2, int a_lThreadIndex);
		static void ProcessContactCallback(const NewtonJoint *a_pJoint, dFloat a_fTimeStep, int a_lThreadIndex);

		const NewtonWorld *m_pNewtonWorld;

		int m_lMaterialId;

		ePhysicsMaterialCombMode m_FrictionMode;
		ePhysicsMaterialCombMode m_ElasticityMode;

		float m_fElasticity;
		float m_fStaticFriction;
		float m_fKinectFriction;
	};
};

#endif