#include "game/Game.h"

#include "system/System.h"
#include "system/LogicTimer.h"
#include "system/String.h"
#include "input/Input.h"
#include "input/Mouse.h"
#include "resources/Resources.h"
#include "graphics/Graphics.h"
#include "graphics/LowLevelGraphics.h"
#include "game/Updater.h"
#include "game/ScriptFunc.h"
#include "graphics/Renderer3D.h"

#include "gui/Gui.h"

#include "system/LowLevelSystem.h"
#include "game/LowLevelGameSetup.h"

namespace efe
{
	cFPSCounter::cFPSCounter(iLowLevelSystem *a_pLowLevelSystem)
	{
		m_fFPS = 60;

		m_lFramecounter = 0;
		m_fFrametimestart = 0;
		m_fFrametime = 0;

		m_fUpdateRate = 1;

		m_pLowLevelSystem = a_pLowLevelSystem;

		m_fFrametimestart  = ((float)GetApplicationTime()) / 1000.0f;
	}
	void cFPSCounter::AddFrame()
	{
		m_lFramecounter++;

		m_fFrametime = (((float)GetApplicationTime()) / 1000.0f) - m_fFrametimestart;

		if (m_fFrametime >= m_fUpdateRate)
		{
			m_fFPS = ((float)m_lFramecounter)/m_fFrametime;
			m_lFramecounter = 0;
			m_fFrametimestart  = ((float)GetApplicationTime()) / 1000.0f;
		}
	}

	//////////////////////////////////////////////////////////////
	// SETUP VAR CONTAINER
	//////////////////////////////////////////////////////////////

	//-----------------------------------------------------------

	cSetupVarContainer::cSetupVarContainer()
	{
		m_sBlank = "";
	}

	//-----------------------------------------------------------

	void cSetupVarContainer::AddString(const tString &a_sName, const tString &a_sValue)
	{
		std::map<tString,tString>::value_type val(a_sName, a_sValue);
		m_mapVars.insert(val);
	}

	void cSetupVarContainer::AddInt(const tString &a_sName, int a_lValue)
	{
		AddString(a_sName, cString::ToString(a_lValue));
	}

	void cSetupVarContainer::AddFloat(const tString &a_sName, float a_fValue)
	{
		AddString(a_sName, cString::ToString(a_fValue));
	}

	void cSetupVarContainer::AddBool(const tString &a_sName, bool a_bValue)
	{
		AddString(a_sName, a_bValue ? "true" : "false");
	}

	const tString &cSetupVarContainer::GetString(const tString &a_sName)
	{
		std::map<tString, tString>::iterator it = m_mapVars.find(a_sName);
		if (it == m_mapVars.end()) return m_sBlank;
		else return it->second;
	}

	float cSetupVarContainer::GetFloat(const tString &a_sName,float a_fDefault)
	{
		const tString &sVal = GetString(a_sName);
		if (sVal == "")
			return a_fDefault;
		else
			return cString::ToFloat(sVal.c_str(),a_fDefault);
	}

	int cSetupVarContainer::GetInt(const tString &a_sName,int a_lDefault)
	{
		const tString &sVal = GetString(a_sName);
		if (sVal == "")
			return a_lDefault;
		else
			return cString::ToInt(sVal.c_str(),a_lDefault);
	}

	bool cSetupVarContainer::GetBool(const tString &a_sName,bool a_bDefault)
	{
		const tString &sVal = GetString(a_sName);
		if (sVal == "")
			return a_bDefault;
		else
			return cString::ToBool(sVal.c_str(),a_bDefault);
	}

	//-----------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-----------------------------------------------------------

	cGame::cGame(iLowLevelGameSetup *a_pGameSetup, cSetupVarContainer &a_Vars)
	{
		GameInit(a_pGameSetup,a_Vars);
	}

	//-----------------------------------------------------------

	cGame::cGame(iLowLevelGameSetup *a_pGameSetup, int a_lWidth, int a_lHeight, int a_iBpp, bool a_bFullscreen,
			unsigned int a_lUpdateRate, int a_lMultiSampling)
	{
		cSetupVarContainer Vars;
		Vars.AddInt("ScreenWidth",a_lWidth);
		Vars.AddInt("ScreenHeight",a_lHeight);
		Vars.AddInt("ScreenBpp",a_iBpp);
		Vars.AddBool("Fullscreen",a_bFullscreen);
		Vars.AddInt("Multisampling",a_lMultiSampling);
		Vars.AddInt("LogicUpdateRate",a_lUpdateRate);
		GameInit(a_pGameSetup,Vars);
	}

	//-----------------------------------------------------------

	void cGame::GameInit(iLowLevelGameSetup *a_pGameSetup, cSetupVarContainer &a_Vars)
	{
		m_pGameSetup = a_pGameSetup;

		Log("Creating Engine Modules\n");
		Log("-----------------------------------------------------\n");

		Log(" Creating graphics module\n");
		m_pGraphics = m_pGameSetup->CreateGraphics();

		Log(" Creating system module\n");
		m_pSystem = m_pGameSetup->CreateSystem();

		Log(" Creating resources module\n");
		m_pResources = m_pGameSetup->CreateResources(m_pGraphics);

		Log(" Creating input module\n");
		m_pInput = m_pGameSetup->CreateInput(m_pGraphics);

		Log(" Creating sound module\n");
		m_pSound = m_pGameSetup->CreateSound();

		Log(" Creating physics module\n");
		m_pPhysics = m_pGameSetup->CreatePhysics();

		Log(" Creating ai module\n");
		m_pAI = m_pGameSetup->CreateAI();

		Log(" Creating gui module\n");
		m_pGui = efeNew(cGui,());

		m_pHaptic = NULL;

		Log("Creating Scene Module\n");
		m_pScene = m_pGameSetup->CreateScene(m_pGraphics, m_pResources, m_pSound, m_pPhysics, m_pSystem, m_pAI, m_pHaptic);
		
		Log("-----------------------------------------------------\n\n");

		m_pResources->Init(m_pGraphics, m_pSystem, m_pSound, m_pScene, m_pGui);

		//Init Graphics
		m_pGraphics->Init(a_Vars.GetInt("ScreenWidth",800),
			a_Vars.GetInt("ScreenHeight",600),
			a_Vars.GetInt("ScreenBpp",32),
			a_Vars.GetBool("Fullscreen",false),
			a_Vars.GetInt("Multisampling",0),
			a_Vars.GetString("WindowCaption"),
			m_pResources);

		//Init Sound
		m_pSound->Init(m_pResources, a_Vars.GetBool("UseSoundHardware",true),
			a_Vars.GetBool("ForceGeneric",false),
			a_Vars.GetBool("UseEnvironmentalAudio",false),
			a_Vars.GetInt("MaxSoundChannels",32),
			a_Vars.GetInt("StreamUpdateFreq",10),
			a_Vars.GetBool("UseSoundThreading",true),
			a_Vars.GetBool("UseVoiceManagement",true),
			a_Vars.GetInt("MaxMonoChannelsHint",0),
			a_Vars.GetInt("MaxStereoChannelsHint",0),
			a_Vars.GetInt("StreamBufferSize",65536),
			a_Vars.GetInt("StreamBufferCount",2),
			a_Vars.GetBool("LowLevelSoundLogging",false),
			a_Vars.GetString("DeviceName"));

		//Init physics
		m_pPhysics->Init(m_pResources);

		//Init AI
		m_pAI->Init();

		//Init Gui
		m_pGui->Init(m_pResources, m_pGraphics,m_pSound, m_pScene);

		Log("Initialziing Game Module\n");
		Log("-----------------------------------------------------\n\n");

		Log("Adding engine updates\n");
		m_pUpdater = efeNew(cUpdater,(m_pSystem->GetLowLevel()));

		m_pUpdater->AddGlobalUpdate(m_pInput);
		m_pUpdater->AddGlobalUpdate(m_pPhysics);
		m_pUpdater->AddGlobalUpdate(m_pScene);
		m_pUpdater->AddGlobalUpdate(m_pSound);
		m_pUpdater->AddGlobalUpdate(m_pAI);
		m_pUpdater->AddGlobalUpdate(m_pGui);
		m_pUpdater->AddGlobalUpdate(m_pResources);

		m_pUpdater->AddContainer("Default");
		m_pUpdater->SetContainer("Default");

		m_pLogicTimer = m_pSystem->CreateLogicTimer(a_Vars.GetInt("LogicUpdateRate",800));

		Log(" Initializing script functions\n");
		cScriptFunc::Init(m_pGraphics, m_pResources, m_pSystem, m_pInput, m_pScene, m_pSound, this);

		m_bGameIsDone = false;

		m_bRenderOnce = false;

		m_fUpdateTime = 0;
		m_fGameTime = 0;

		m_bLimitFPS = true;

		m_pFPSCounter = efeNew(cFPSCounter, (m_pSystem->GetLowLevel()));
		Log("-----------------------------------------------------\n\n");

		Log("User initialization\n");
		Log("-----------------------------------------------------\n");
	}

	cGame::~cGame()
	{
		efeDelete(m_pLogicTimer);
		efeDelete(m_pFPSCounter);

		efeDelete(m_pUpdater);

		efeDelete(m_pGui);
		efeDelete(m_pScene);
		if(m_pHaptic) efeDelete(m_pHaptic);
		efeDelete(m_pInput);
		efeDelete(m_pSound);
		efeDelete(m_pGraphics);
		efeDelete(m_pResources);
		efeDelete(m_pPhysics);
		efeDelete(m_pAI);
		efeDelete(m_pSystem);

		Log(" Deleting game setup provided by user\n");
		efeDelete(m_pGameSetup);

		Log("EFE exit was successful!\n");
	}

	int g_lClearUpdateCheck=0;
	void cGame::Run()
	{
		Log("-----------------------------------------------------\n\n");

		bool bDone = false;
		double fNumOfTimes=0;
		double fMediumTime=0;

		m_pUpdater->OnStart();

		m_pLogicTimer->Reset();

		unsigned long lTempTime = GetApplicationTime();

		for (int i=0;i < 10;i++)
			m_pInput->GetMouse()->Reset();

		Log("Game Running\n");
		Log("-----------------------------------------------------\n");

		m_fFrameTime = 0;
		unsigned long lTempFrameTime = GetApplicationTime();

		bool m_bIsUpdated = true;

		while(!m_bGameIsDone)
		{
			START_TIMING_TAB(UpdateRR)
			while (m_pLogicTimer->WantUpdate() && !m_bGameIsDone)
			{
				unsigned int lUpdateTime = GetApplicationTime();

				m_pUpdater->Update(GetStepSize());

				unsigned int lDeltaTime = GetApplicationTime() - lUpdateTime;
				m_fUpdateTime = (float)(lDeltaTime) / 1000.0f;

				m_bIsUpdated = true;

				g_lClearUpdateCheck++;
				if (g_lClearUpdateCheck % 500 == 0)
				{
					if (m_pUpdater->GetCurrentContainerName()=="Default") ClearUpdateLogFile();
				}

				m_fGameTime += GetStepSize();
			}
			STOP_TIMING_TAB(UpdateRR)

			START_TIMING_TAB(MYFUCKINGUPDATER)

			m_pLogicTimer->EndUpdateLoop();

			if (m_bRenderOnce&& bDone)continue;
			if(m_bRenderOnce)bDone = true;

			if (m_bIsUpdated)
			{
				m_pScene->UpdateRenderList(m_fFrameTime);
				if (m_bLimitFPS==false) m_bIsUpdated = false;
			}

			if (m_bLimitFPS == false || m_bIsUpdated)
			{
				m_bIsUpdated = false;

				m_fFrameTime = ((float)GetApplicationTime() - lTempFrameTime) / 1000;
				lTempFrameTime = GetApplicationTime();

				m_pUpdater->OnDraw();
				m_pScene->Render(m_pUpdater, m_fFrameTime);

				m_pFPSCounter->AddFrame();

				m_pGraphics->GetLowLevel()->SwapBuffers();

				fNumOfTimes++;
			}
			STOP_TIMING_TAB(MYFUCKINGUPDATER)
		}
		Log("-----------------------------------------------------\n\n");

		Log("Statistics\n");
		Log("-----------------------------------------------------\n");

		unsigned long lTime = GetApplicationTime() - lTempTime;
		fMediumTime = fNumOfTimes/(((double)lTime)/1000);

		Log(" Medium framerate: %f\n", fMediumTime);
		Log("-----------------------------------------------------\n\n");

		Log("User exit\n");
		Log("-----------------------------------------------------\n");

		m_pUpdater->OnExit();
	}

	void cGame::Exit()
	{
		m_bGameIsDone = true;
	}

	float cGame::GetStepSize()
	{
		return m_pLogicTimer->GetStepSize();
	}

	//-----------------------------------------------------------

	cScene *cGame::GetScene()
	{
		return m_pScene;
	}

	//-----------------------------------------------------------

	cResources *cGame::GetResources()
	{
		return m_pResources;
	}

	//-----------------------------------------------------------

	cGraphics *cGame::GetGraphics()
	{
		return m_pGraphics;
	}

	//-----------------------------------------------------------

	cSound *cGame::GetSound()
	{
		return m_pSound;
	}

	//-----------------------------------------------------------

	cPhysics *cGame::GetPhysics()
	{
		return m_pPhysics;
	}

	//-----------------------------------------------------------

	cInput *cGame::GetInput()
	{
		return m_pInput;
	}

	//-----------------------------------------------------------

	cGui *cGame::GetGui()
	{
		return m_pGui;
	}

	//-----------------------------------------------------------

	cUpdater *cGame::GetUpdater()
	{
		return m_pUpdater;
	}

	float cGame::GetFPS()
	{
		return m_pFPSCounter->m_fFPS;
	}

	//-----------------------------------------------------------

	void cGame::SetFPSUpdateRate(float a_fSec)
	{
		m_pFPSCounter->m_fUpdateRate = a_fSec;
	}

	float cGame::GetFPSUpdateRate()
	{
		return m_pFPSCounter->m_fUpdateRate;
	}
}