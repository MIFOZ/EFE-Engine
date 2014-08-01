#include "scene/Scene.h"
#include "game/Updater.h"
#include "system/LowLevelSystem.h"

#include "graphics/Graphics.h"
#include "resources/Resources.h"
#include "physics/Collider2D.h"
#include "sound/Sound.h"
#include "sound/LowLevelSound.h"
#include "sound/SoundHandler.h"
#include "scene/Camera2D.h"
#include "scene/Camera3D.h"
#include "scene/World2D.h"
#include "scene/World3D.h"
#include "graphics/Renderer2D.h"
#include "graphics/Renderer3D.h"
#include "graphics/GraphicsDrawer.h"
#include "graphics/RenderList.h"
#include "system/Script.h"
#include "physics/Physics.h"

namespace efe
{
	cScene::cScene(cGraphics *a_pGraphics, cResources *a_pResources, cSound *a_pSound,
			cPhysics *a_pPhysics, cSystem *a_pSystem, cAI *a_pAI, cHaptic *a_pHaptic)
			: iUpdateable("EFE_Scene")
	{
		m_pGraphics = a_pGraphics;
		m_pResources = a_pResources;
		m_pSound = a_pSound;
		m_pPhysics = a_pPhysics;
		m_pSystem = a_pSystem;
		m_pAI = a_pAI;
		m_pHaptic = a_pHaptic;

		m_pCollider2D = efeNew(cCollider2D,());
		m_pCurrentWorld2D = NULL;
		m_pCurrentWorld3D = NULL;

		m_bCameraIsListener = true;

		m_bDrawScene = true;
		m_bUpdateMap = true;

		m_pActiveCamera = NULL;
	}

	cScene::~cScene()
	{
		STLDeleteAll(m_lstWorld3D);
		STLDeleteAll(m_lstCamera);

		efeDelete(m_pCollider2D);
	}

	//--------------------------------------------------------------

	cCamera3D *cScene::CreateCamera3D(eCameraMoveMode a_MoveMode)
	{
		cCamera3D *pCamera = efeNew(cCamera3D, ());
		pCamera->SetAspect(m_pGraphics->GetLowLevel()->GetScreenSize().x /
							m_pGraphics->GetLowLevel()->GetScreenSize().y);

		m_lstCamera.push_back(pCamera);

		return pCamera;
	}
	
	//--------------------------------------------------------------

	void cScene::DestroyCamera(iCamera *a_pCam)
	{
		for (tCameraListIt it=m_lstCamera.begin();it!=m_lstCamera.end();it++)
		{
			if (*it == a_pCam)
			{
				efeDelete(*it);
				m_lstCamera.erase(it);
				break;
			}
		}
	}

	void cScene::SetCamera(iCamera *a_pCam)
	{
		m_pActiveCamera = a_pCam;

		if (m_bCameraIsListener)
		{
			if (m_pActiveCamera->GetType() == eCameraType_2D)
			{
				cCamera2D *pCamera2D = static_cast<cCamera2D*>(m_pActiveCamera);

				m_pSound->GetLowLevel()->SetListenerPosition(pCamera2D->GetPosition());
			}
		}
	}
	
	//--------------------------------------------------------------

	void cScene::SetCameraPosition(const cVector3f &a_vPos)
	{
		if (m_pActiveCamera->GetType() == eCameraType_2D)
		{
			cCamera2D *pCamera2D = static_cast<cCamera2D*>(m_pActiveCamera);

			pCamera2D->SetPosition(a_vPos);
		}

		if (m_bCameraIsListener)
			m_pSound->GetLowLevel()->SetListenerPosition(a_vPos);
	}

	cVector3f cScene::GetCameraPosition()
	{
		if (m_pActiveCamera->GetType() == eCameraType_2D)
		{
			cCamera2D *pCamera2D = static_cast<cCamera2D*>(m_pActiveCamera);
			return pCamera2D->GetPosition();
		}

		return cVector2f(0);
	}

	//--------------------------------------------------------------

	void cScene::UpdateRenderList(float a_fFrameTime)
	{
		if (m_bDrawScene && m_pActiveCamera)
		{
			if (m_pActiveCamera->GetType() == eCameraType_3D)
			{
				cCamera3D *pCamera3D = static_cast<cCamera3D*>(m_pActiveCamera);

				if (m_pCurrentWorld3D)
					m_pGraphics->GetRenderer3D()->UpdateRenderList(m_pCurrentWorld3D, pCamera3D, a_fFrameTime);
			}
		}
	}

	void cScene::SetDrawScene(bool a_bX)
	{
		m_bDrawScene = a_bX;
		m_pGraphics->GetRenderer3D()->GetRenderList()->Clear();
	}

	//-----------------------------------------------------------

	cScriptVar *cScene::CreateLocalVar(const tString &a_sName)
	{
		cScriptVar *pVar;
		pVar = GetLocalVar(a_sName);
		if (pVar == NULL)
		{
			cScriptVar Var;
			Var.m_sName = a_sName;
			m_mapLocalVars.insert(tScriptVarMap::value_type(cString::ToLowerCase(a_sName), Var));
			pVar = GetLocalVar(a_sName);
		}
		return pVar;
	}

	//-----------------------------------------------------------

	cScriptVar *cScene::GetLocalVar(const tString &a_sName)
	{
		tScriptVarMapIt it = m_mapLocalVars.find(cString::ToLowerCase(a_sName));
		if (it == m_mapLocalVars.end()) return NULL;

		return &it->second;
	}

	//-----------------------------------------------------------

	tScriptVarMap *cScene::GetLocalVarMap()
	{
		return &m_mapLocalVars;
	}

	//-----------------------------------------------------------

	cScriptVar *cScene::CreateGlobalVar(const tString &a_sName)
	{
		cScriptVar *pVar;
		pVar = GetGlobalVar(a_sName);
		if (pVar == NULL)
		{
			cScriptVar Var;
			Var.m_sName = a_sName;
			m_mapGlobalVars.insert(tScriptVarMap::value_type(cString::ToLowerCase(a_sName), Var));
			pVar = GetGlobalVar(a_sName);
		}
		return pVar;
	}

	//-----------------------------------------------------------

	cScriptVar *cScene::GetGlobalVar(const tString &a_sName)
	{
		tScriptVarMapIt it = m_mapGlobalVars.find(cString::ToLowerCase(a_sName));
		if (it == m_mapGlobalVars.end()) return NULL;

		return &it->second;
	}

	//-----------------------------------------------------------

	tScriptVarMap *cScene::GetGlobalVarMap()
	{
		return &m_mapGlobalVars;
	}

	//-----------------------------------------------------------

	void cScene::Render(cUpdater *a_pUpdater, float a_fFrameTime)
	{
		if (m_bDrawScene && m_pActiveCamera)
		{
			if (m_pActiveCamera->GetType() == eCameraType_2D)
			{
				cCamera2D *pCamera2D = static_cast<cCamera2D*>(m_pActiveCamera);

				if (m_pCurrentWorld2D)
					m_pCurrentWorld2D->Render(pCamera2D);

				m_pGraphics->GetRenderer2D()->RenderObjects(pCamera2D,m_pCurrentWorld2D->GetGridMapLights(),m_pCurrentWorld2D);
			}
			else
			{
				cCamera3D *pCamera3D = static_cast<cCamera3D*>(m_pActiveCamera);

				if(m_pCurrentWorld3D)
				{
					START_TIMING(RenderWorld)
						m_pGraphics->GetRenderer3D()->RenderWorld(m_pCurrentWorld3D, pCamera3D, a_fFrameTime);
					STOP_TIMING(RenderWorld)
				}
			}
			START_TIMING(OnPostSceneDraw)
			//	a_pUpdater->OnPostSceneDraw();
			STOP_TIMING(OnPostSceneDraw)

			//START_TIMING(OnPostSceneDraw)
		}
		else
			a_pUpdater->OnPostSceneDraw();

		//m_pGraphics->GetDrawer()->DrawAll();

		//a_pUpdater->OnPostGUIDraw();
	}

	void cScene::Update(float a_fTimeStep)
	{
		if (m_pActiveCamera == NULL)return;

		if (m_pActiveCamera->GetType() == eCameraType_2D)
		{
			
		}
		else
		{
			if (m_bCameraIsListener)
			{
				cCamera3D *pCamera3D = static_cast<cCamera3D*>(m_pActiveCamera);
				m_pSound->GetLowLevel()->SetListenerAttributes(
					pCamera3D->GetPosition(),
					cVector3f(0,0,0),
					pCamera3D->GetForward()*-1.0f,
					pCamera3D->GetUp());
			}

			if (m_bUpdateMap && m_pCurrentWorld3D)
			{
				m_pCurrentWorld3D->Update(a_fTimeStep);

				if (m_pCurrentWorld3D->GetScript())
					m_pCurrentWorld3D->GetScript()->Run("OnUpdate()");
			}
		}
	}

	void cScene::Reset()
	{
		m_mapLocalVars.clear();
		m_mapGlobalVars.clear();
		m_setLoadedMaps.clear();
	}

	cWorld3D *cScene::CreateWorld3D(const tString &a_sName)
	{
		cWorld3D* pWorld = efeNew(cWorld3D, (a_sName, m_pGraphics, m_pResources, m_pSound, m_pPhysics, this,
											m_pSystem, m_pAI, m_pHaptic));

		m_lstWorld3D.push_back(pWorld);

		return pWorld;
	}

	void cScene::DestroyWorld3D(cWorld3D *a_pWorld)
	{
		STLFindAndDelete(m_lstWorld3D, a_pWorld);
	}

	void cScene::SetWorld3D(cWorld3D *a_pWorld)
	{
		m_pCurrentWorld3D = a_pWorld;

		m_pSound->GetSoundHandler()->SetWorld3D(m_pCurrentWorld3D);
	}
}
