#ifndef EFE_LOWLEVELGAMESETUP_H
#define EFE_LOWLEVELGAMESETUP_H

#include "system/System.h"
#include "input/Input.h"
#include "graphics/Graphics.h"
#include "resources/Resources.h"
#include "scene/Scene.h"
#include "sound/Sound.h"
#include "physics/Physics.h"
#include "ai/AI.h"

namespace efe
{
	class iLowLevelGameSetup
	{
	public:
		virtual ~iLowLevelGameSetup(){}

		virtual cInput *CreateInput(cGraphics *a_pGraphics)=0;
		virtual cSystem *CreateSystem()=0;
		virtual cGraphics *CreateGraphics()=0;
		virtual cResources *CreateResources(cGraphics *a_pGraphics)=0;
		virtual cScene *CreateScene(cGraphics *a_pGraphics,cResources *a_pResources,cSound *a_pSound,
									cPhysics *a_pPhysics,cSystem *a_pSystem,cAI *a_pAI,
									cHaptic *a_pHaptics)=0;
		virtual cSound *CreateSound()=0;
		virtual cPhysics *CreatePhysics()=0;
		virtual cAI *CreateAI()=0;
	};
};

#endif