#include "impl/PhysicsMaterialPx.h"

#include "impl/PhysicsWorldPx.h"
#include "impl/PhysicsBodyPx.h"
#include "physics/SurfaceData.h"

#include "system/LowLevelSystem.h"
#include <algorithm>

//#include <PxMaterial.h>

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cPhysicsMaterialPx::cPhysicsMaterialPx(const tString &a_sName, iPhysicsWorld *a_pWorld, int a_lMatId)
		: iPhysicsMaterial(a_sName, a_pWorld)
	{
		cPhysicsWorldPx *pWorld = static_cast<cPhysicsWorldPx*>(a_pWorld);

		m_pPxMaterial = pWorld->GetPxSDK()->createMaterial(0.3f, 0.3f, 0.5f);

		m_FrictionMode = ePhysicsMaterialCombMode_Average;
		m_ElasticityMode = ePhysicsMaterialCombMode_Average;

		m_fElasticity = 0.5f;
		m_fStaticFriction = 0.3f;
		m_fKinectFriction = 0.3f;
	}

	//--------------------------------------------------------------

	cPhysicsMaterialPx::~cPhysicsMaterialPx()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cPhysicsMaterialPx::SetElasticity(float a_fElasctictiy)
	{
		m_fElasticity = a_fElasctictiy;

		UpdateMaterials();
	}

	float cPhysicsMaterialPx::GetElasticity() const
	{
		return m_fElasticity;
	}

	//--------------------------------------------------------------

	void cPhysicsMaterialPx::SetStaticFriction(float a_fFriction)
	{
		m_fStaticFriction = a_fFriction;

		UpdateMaterials();
	}

	float cPhysicsMaterialPx::GetStaticFriction() const
	{
		return m_fStaticFriction;
	}

	//--------------------------------------------------------------

	void cPhysicsMaterialPx::SetKineticFriction(float a_fFriction)
	{
		m_fKinectFriction = a_fFriction;

		UpdateMaterials();
	}

	float cPhysicsMaterialPx::GetKineticFriction() const
	{
		return m_fKinectFriction;
	}

	//--------------------------------------------------------------

	void cPhysicsMaterialPx::SetFrictionCombMode(ePhysicsMaterialCombMode a_Mode)
	{
		m_FrictionMode = a_Mode;

		UpdateMaterials();
	}

	//--------------------------------------------------------------

	void cPhysicsMaterialPx::SetElasticityCombMode(ePhysicsMaterialCombMode a_Mode)
	{
		m_ElasticityMode = a_Mode;

		UpdateMaterials();
	}

	//--------------------------------------------------------------

	void cPhysicsMaterialPx::UpdateMaterials()
	{
		cPhysicsMaterialIterator MatIt = m_pWorld->GetMaterialIterator();

		while (MatIt.HasNext())
		{
			cPhysicsMaterialPx *pMat = static_cast<cPhysicsMaterialPx*>(MatIt.Next());

			ePhysicsMaterialCombMode frictionMode = (ePhysicsMaterialCombMode) std::max(m_FrictionMode, 
				pMat->m_FrictionMode);
			ePhysicsMaterialCombMode elasticityMode = (ePhysicsMaterialCombMode) std::max(m_ElasticityMode, 
				pMat->m_ElasticityMode);

			if (pMat==this)
			{
				frictionMode = ePhysicsMaterialCombMode_Average;
				elasticityMode = ePhysicsMaterialCombMode_Average;
			}

			m_pPxMaterial->setRestitutionCombineMode(GetPxCombineModeEnum(elasticityMode));
			m_pPxMaterial->setRestitution(m_fElasticity);

			m_pPxMaterial->setFrictionCombineMode(GetPxCombineModeEnum(frictionMode));
			m_pPxMaterial->setDynamicFriction(m_fKinectFriction);
			m_pPxMaterial->setStaticFriction(m_fStaticFriction);
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	PxCombineMode::Enum cPhysicsMaterialPx::GetPxCombineModeEnum(ePhysicsMaterialCombMode a_Mode)
	{
		switch (a_Mode)
		{
		case efe::ePhysicsMaterialCombMode_Average: return PxCombineMode::eAVERAGE;
		case efe::ePhysicsMaterialCombMode_Min: return PxCombineMode::eMIN;
		case efe::ePhysicsMaterialCombMode_Multiply: return PxCombineMode::eMULTIPLY;
		case efe::ePhysicsMaterialCombMode_Max: return PxCombineMode::eMAX;
		}

		return PxCombineMode::eAVERAGE;
	}
}