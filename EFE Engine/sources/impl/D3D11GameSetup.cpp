#include "impl/D3D11GameSetup.h"
#include "impl/LowLevelGraphicsDX11.h"
#include "impl/LowLevelResourcesDX.h"
#include "impl/LowLevelSystemWin.h"
#include "impl/LowLevelInputRaw.h"
#include "impl/LowLevelSoundOpenAL.h"
#include "impl/LowLevelPhysicsNewton.h"

namespace efe
{
	cD3D11GameSetup::cD3D11GameSetup(int a_hWindow)
	{
		m_pLowLevelSystem = efeNew(cLowLevelSystemWin,());
		m_pLowLevelGraphics = efeNew(cLowLevelGraphicsDX11,(a_hWindow));
		m_pLowLevelInput = efeNew(cLowLevelInputRaw, (m_pLowLevelGraphics));
		m_pLowLevelResources = efeNew(cLowLevelResourcesDX,((cLowLevelGraphicsDX11 *)m_pLowLevelGraphics));
		m_pLowLevelSound = efeNew (cLowLevelSoundOpenAL, ());
		m_pLowLevelPhysics = efeNew(cLowLevelPhysicsNewton, ());
	}

	cD3D11GameSetup::~cD3D11GameSetup()
	{
		Log("- Deleting lowlevel stuff yo.\n");

		Log(" Physics\n");
		efeDelete(m_pLowLevelPhysics);
		Log(" Sound\n");
		efeDelete(m_pLowLevelSound);
		Log("  Input\n");
		efeDelete(m_pLowLevelInput);
		Log("  Resources\n");
		efeDelete(m_pLowLevelResources);
		Log("  System\n");
		efeDelete(m_pLowLevelSystem);
		Log("  Graphics\n");
		efeDelete(m_pLowLevelGraphics);
		Log("  Haptic\n");
		//if (m_pLowLevelHaptic) efeDelete(m_pLowLevelHaptic);
	}

	cScene *cD3D11GameSetup::CreateScene(cGraphics *a_pGraphics,cResources *a_pResources,cSound *a_pSound,
									cPhysics *a_pPhysics,cSystem *a_pSystem,cAI *a_pAI,
									cHaptic *a_pHaptics)
	{
		cScene *pScene = efeNew(cScene, (a_pGraphics, a_pResources, a_pSound, a_pPhysics, a_pSystem, a_pAI, a_pHaptics));
		return pScene;
	}

	cResources *cD3D11GameSetup::CreateResources(cGraphics *a_pGraphics)
	{
		cResources *pResources = efeNew(cResources, (m_pLowLevelResources, m_pLowLevelGraphics));
		return pResources;
	}

	cInput *cD3D11GameSetup::CreateInput(cGraphics *a_pGraphics)
	{
		cInput *pInput = efeNew(cInput, (m_pLowLevelInput));
		return pInput;
	}

	cSystem *cD3D11GameSetup::CreateSystem()
	{
		cSystem *pSystem = efeNew(cSystem, (m_pLowLevelSystem));
		return pSystem;
	}

	cGraphics *cD3D11GameSetup::CreateGraphics()
	{
		cGraphics *pGraphics = efeNew(cGraphics, (m_pLowLevelGraphics,m_pLowLevelResources));
		return pGraphics;
	}

	cSound *cD3D11GameSetup::CreateSound()
	{
		cSound *pSound = efeNew(cSound, (m_pLowLevelSound));
		return pSound;
	}

	cPhysics *cD3D11GameSetup::CreatePhysics()
	{
		cPhysics *pPhysics = efeNew(cPhysics, (m_pLowLevelPhysics));
		return pPhysics;
	}

	cAI *cD3D11GameSetup::CreateAI()
	{
		cAI *pAI = efeNew(cAI, ());
		return pAI;
	}
}