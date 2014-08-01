#ifndef EFE_D3D11_GAMESETUP_H
#define EFE_D3D11_GAMESETUP_H

#include "game/LowLevelGameSetup.h"
#include "graphics/LowLevelGraphics.h"
#include "input/LowLevelInput.h"
#include "physics/LowLevelPhysics.h"
#include "resources/LowLevelResources.h"
#include "sound/LowLevelSound.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	class cD3D11GameSetup : public iLowLevelGameSetup
	{
	public:
		cD3D11GameSetup(int a_hWindow);
		~cD3D11GameSetup();

		cInput *CreateInput(cGraphics *a_pGraphics);
		cSystem *CreateSystem();
		cGraphics *CreateGraphics();
		cResources *CreateResources(cGraphics *a_pGraphics);
		cScene *CreateScene(cGraphics *a_pGraphics,cResources *a_pResources,cSound *a_pSound,
									cPhysics *a_pPhysics,cSystem *a_pSystem,cAI *a_pAI,
									cHaptic *a_pHaptics);
		cSound *CreateSound();
		cPhysics *CreatePhysics();
		cAI *CreateAI();
	public:
		iLowLevelSystem *m_pLowLevelSystem;
		iLowLevelGraphics *m_pLowLevelGraphics;
		iLowLevelInput *m_pLowLevelInput;
		iLowLevelResources *m_pLowLevelResources;
		iLowLevelSound *m_pLowLevelSound;
		iLowLevelPhysics *m_pLowLevelPhysics;
	};
};
#endif