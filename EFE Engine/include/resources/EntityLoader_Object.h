#ifndef EFE_ENTITY_LOADER_OBJECT_H
#define EFE_ENTITY_LOADER_OBJECT_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

#include "resources/Resources.h"

class TiXmlElement;

namespace efe
{
	class iPhysicsBody;
	class iPhysicsJoint;
	class iPhysicsWorld;
	class cMesh;
	class cMeshEntity;
	class cParticleSystem3D;
	class cBillboard;
	class cBeam;
	class cSoundEntity;
	class iLight3D;

	class cEntityLoader_Object : public iEntity3DLoader
	{
	public:
		cEntityLoader_Object(const tString &a_sName) : iEntity3DLoader(a_sName){}
		virtual ~cEntityLoader_Object(){}

		iEntity3D *Load(const tString &a_sName, TiXmlElement *a_pRootElem, const cMatrixf &a_mtxTransform,
								cWorld3D *a_pWorld, const tString &a_sFileName, bool a_bLoadReferences);

	protected:
		virtual void BeforeLoad(TiXmlElement *a_pRootElem, const cMatrixf &a_mtxTransform, cWorld3D *a_pWorld){}
		virtual void AfterLoad(TiXmlElement *a_pRootElem, const cMatrixf &a_mtxTransform, cWorld3D *a_pWorld){}

		void SetBodyProperties(iPhysicsBody *a_pBody, TiXmlElement *a_pPhysicsElem);
		void SetJointProperties(iPhysicsJoint *a_pJoint, TiXmlElement *a_pJointElem, cWorld3D *a_pWorld);

		void LoadController(iPhysicsJoint *a_pJoint, iPhysicsWorld *a_pPhysicsWorld, TiXmlElement *a_pElem);

		eAnimationEventType GetAnimationEventType(const char *a_pString);

		tString m_sSubType;
		tString m_sName;

		tString m_sFileName;

		std::vector<iPhysicsBody*> m_vBodies;
		std::vector<iPhysicsJoint*> m_vJoints;

		std::vector<iLight3D*> m_vLights;
		std::vector<cParticleSystem3D*> m_vParticleSystem;
		std::vector<cBillboard*> m_vBillboards;
		std::vector<cBeam*> m_vBeams;
		std::vector<cSoundEntity*> m_vSoundEntities;

		cMeshEntity *m_pEntity;
		cMesh *m_pMesh;
	};
};
#endif