#include "game/ScriptFunc.h"

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "game/Game.h"
#include "graphics/Graphics.h"
#include "sound/Sound.h"
#include "resources/Resources.h"
#include "math/Math.h"
#include "scene/Scene.h"

#include "scene/World3D.h"

#include "scene/PortalContainer.h"
#include "input/Input.h"
#include "system/System.h"
#include "system/LowLevelSystem.h"

#include "system/Script.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// JOINT CALLBACK
	//////////////////////////////////////////////////////////////

	cScriptJointCallback::cScriptJointCallback(cScene *a_pScene)
	{
		m_pScene = a_pScene;

		m_sMaxFunc = "";
		m_sMinFunc = "";
	}

	void cScriptJointCallback::OnMinLimit(iPhysicsJoint *a_pJoint)
	{
		if (m_sMinFunc!="")
		{
			iScript *pScript = m_pScene->GetWorld3D()->GetScript();

			tString sCommand = m_sMinFunc + "(\"" + a_pJoint->GetName() + "\")";
			if (pScript->Run(sCommand)==false)
				Warning("Couldn't run script command '%s\n", sCommand.c_str());
		}
	}

	void cScriptJointCallback::OnMaxLimit(iPhysicsJoint *a_pJoint)
	{
		if (m_sMaxFunc!="")
		{
			iScript *pScript = m_pScene->GetWorld3D()->GetScript();

			tString sCommand = m_sMaxFunc + "(\"" + a_pJoint->GetName() + "\")";
			if (pScript->Run(sCommand)==false)
				Warning("Couldn't run script command '%s\n", sCommand.c_str());
		}
	}

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	static cResources *g_pResources = NULL;
	static cSystem *g_pSystem = NULL;
	static cInput *g_pInput = NULL;
	static cGraphics *g_pGraphics = NULL;
	static cScene *g_pScene = NULL;
	static cSound *g_pSound = NULL;
	static cGame *g_pGame = NULL;

	static void __stdcall Print(std::string a_sText)
	{
		Log(a_sText.c_str());
	}
	SCRIPT_DEFINE_FUNC_1(void, Print, string)

	//static 

	void cScriptFunc::Init(cGraphics *a_pGraphics,
			cResources *a_pResources,
			cSystem *a_pSystem,
			cInput *a_pInput,
			cScene *a_pScene,
			cSound *a_pSound,
			cGame *a_pGame)
	{
		g_pGraphics = a_pGraphics;
		g_pResources = a_pResources;
		g_pSystem = a_pSystem;
		g_pInput = a_pInput;
		g_pScene = a_pScene;
		g_pSound = a_pSound;
		g_pGame = a_pGame;

		g_pSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(Print));
	}
}