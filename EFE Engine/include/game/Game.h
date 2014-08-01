#ifndef EFE_GAME_H
#define EFE_GAME_H

#include "system/SystemTypes.h"

namespace efe
{
	class cUpdater;
	class iLowLevelGameSetup;
	class iLowLevelSystem;
	class cLogicTimer;

	class cSystem;
	class cInput;
	class cResources;
	class cGraphics;
	class cScene;
	class cSound;
	class cPhysics;
	class cAI;
	class cHaptic;
	class cGui;

	class cFPSCounter
	{
	public:
		cFPSCounter(iLowLevelSystem *a_pLowLevelSystem);
		void AddFrame();

		float m_fFPS;
		float m_fUpdateRate;
	private:
		iLowLevelSystem *m_pLowLevelSystem;
		int m_lFramecounter;
		float m_fFrametimestart;
		float m_fFrametime;
	};

	//------------------------------------------------------

	class cSetupVarContainer
	{
	public:
		cSetupVarContainer();

		void AddString(const tString &a_sName, const tString &a_sValue);

		void AddInt(const tString &a_sName, int a_lValue);
		void AddFloat(const tString &a_sName, float a_fValue);
		void AddBool(const tString &a_sName, bool a_bValue);

		const tString &GetString(const tString &a_sName);

		float GetFloat(const tString &a_sName,float a_fDefault);
		int GetInt(const tString &a_sName,int a_lDefault);
		bool GetBool(const tString &a_sName,bool a_bDefault);
	private:
		std::map<tString, tString> m_mapVars;
		tString m_sBlank;
	};

	class cGame
	{
	public:
		cGame(iLowLevelGameSetup *a_pGameSetup, cSetupVarContainer &a_Vars);
		cGame(iLowLevelGameSetup *a_pGameSetup, int a_lWidth, int a_lHeight, int a_iBpp, bool a_bFullscreen,
			unsigned int a_lUpdateRate = 60, int a_lMultiSampling = 0);
		~cGame();
	private:
		void GameInit(iLowLevelGameSetup *a_pGameSetup, cSetupVarContainer &a_Vars);
	public:
		void Run();

		void Exit();

		cScene *GetScene();
		cResources *GetResources();

		cUpdater *GetUpdater();

		cSystem *GetSystem();
		cInput *GetInput();
		cGraphics *GetGraphics();
		cSound *GetSound();
		cPhysics *GetPhysics();
		cAI *GetAI();
		cHaptic *GetHaptic();
		cGui *GetGui();

		void ResetLogicTimer();
		float GetStepSize();

		cLogicTimer *GetLogicTimer(){return m_pLogicTimer;}

		float GetFPS();

		void SetFPSUpdateRate(float a_fSec);
		float GetFPSUpdateRate();

		float GetFrameTime(){return m_fFrameTime;}
		
		float GetUpdateTime(){return m_fFrameTime;}

		void SetLimitFPS(bool a_bX){m_bLimitFPS = a_bX;}
		bool GetLimitFPS(){return m_bLimitFPS;}
	private:
		bool m_bGameIsDone;

		bool m_bRenderOnce;

		float m_fFrameTime;
		float m_fUpdateTime;

		double m_fGameTime;

		iLowLevelGameSetup *m_pGameSetup;
		cUpdater *m_pUpdater;
		cLogicTimer *m_pLogicTimer;

		cFPSCounter *m_pFPSCounter;

		bool m_bLimitFPS;

		cSystem *m_pSystem;
		cInput *m_pInput;
		cResources *m_pResources;
		cGraphics *m_pGraphics;
		cScene *m_pScene;
		cSound *m_pSound;
		cPhysics *m_pPhysics;
		cAI *m_pAI;
		cHaptic *m_pHaptic;
		cGui *m_pGui;
	};
};
#endif