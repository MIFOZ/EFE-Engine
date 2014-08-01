#ifndef EFE_PHYSICS_MATERIAL_H
#define EFE_PHYSICS_MATERIAL_H

#include "math/MathTypes.h"

namespace efe
{
	class iPhysicsWorld;
	class cSurfaceData;

	enum ePhysicsMaterialCombMode
	{
		ePhysicsMaterialCombMode_Average =	0,
		ePhysicsMaterialCombMode_Min =		1,
		ePhysicsMaterialCombMode_Multiply = 2,
		ePhysicsMaterialCombMode_Max =		3,
		ePhysicsMaterialCombMode_LastEnum
	};

	class cPhysicsContactData
	{
	public:
		cPhysicsContactData()
		{
			m_vMaxContactNormalSpeed = 0;
			m_vMaxContactTangentSpeed = 0;
			m_vContactNormal = cVector3f(0,0,0);
			m_vContactPosition = cVector3f(0,0,0);
			m_vForce = cVector3f(0,0,0);
		}

		float m_vMaxContactNormalSpeed;
		float m_vMaxContactTangentSpeed;
		cVector3f m_vContactNormal;
		cVector3f m_vContactPosition;
		cVector3f m_vForce;
	};

	class iPhysicsMaterial
	{
	public:
		iPhysicsMaterial(const tString a_sName, iPhysicsWorld *a_pWorld)
			: m_sName(a_sName), m_pWorld(a_pWorld), m_pSurfaceData(NULL),
			m_bPreloaded(false){}
		virtual ~iPhysicsMaterial(){}

		const tString &GetName(){return m_sName;}

		virtual void SetElasticity(float a_fElasctictiy)=0;
		virtual float GetElasticity() const=0;
		virtual void SetStaticFriction(float a_fFriction)=0;
		virtual float GetStaticFriction() const=0;
		virtual void SetKineticFriction(float a_fFriction)=0;
		virtual float GetKineticFriction() const=0;

		virtual void SetFrictionCombMode(ePhysicsMaterialCombMode a_Mode)=0;

		virtual void SetElasticityCombMode(ePhysicsMaterialCombMode a_Mode)=0;
		
		void SetSurfaceData(cSurfaceData *a_pSurfaceData){m_pSurfaceData = a_pSurfaceData;}
		cSurfaceData *GetSurfaceData(){return m_pSurfaceData;}
		
		void SetPreloaded(bool a_bX){m_bPreloaded = a_bX;}
		bool IsPreloaded(){return m_bPreloaded;}
	protected:
		iPhysicsWorld *m_pWorld;
		tString m_sName;

		bool m_bPreloaded;

		cSurfaceData *m_pSurfaceData;
	};
};

#endif