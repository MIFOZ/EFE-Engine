#ifndef EFE_AI_NODE_GENERATOR_H
#define EFE_AI_NODE_GENERATOR_H

#include "system/SystemTypes.h"
#include "game/GameTypes.h"

#include "physics/PhysicsWorld.h"
#include "scene/World3D.h"

namespace efe
{
	class cWorld3D;

	class cAINodeGeneratorParams
	{
	public:
		cAINodeGeneratorParams();

		tString m_sNodeType;

		float m_fHeightFromGround;
		float m_fMinWallDist;

		cVector3f m_vMinPos;
		cVector3f m_vMaxPos;

		float m_fGridSize;
	};

	class cAINodeGenerator : public iPhysicsRayCallback
	{
	public:
		cAINodeGenerator();
		~cAINodeGenerator();

		void Generate(cWorld3D *a_pWorld, cAINodeGeneratorParams *a_pParams);

	private:
		bool OnIntersect(iPhysicsBody *a_pBody, cPhysicsRayParams *a_pParams);

		void SaveToFile();
		void LoadFromFile();

		cAINodeGeneratorParams *m_pParams;
		cWorld3D *m_pWorld;
		tTempAiNodeList *m_pNodeList;
	};
};
#endif