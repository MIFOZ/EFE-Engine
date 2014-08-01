#ifndef EFE_SCENE_H
#define EFE_SCENE_H

#include <list>

#include "system/SystemTypes.h"
#include "game/GameTypes.h"

#include "game/Updateable.h"
#include "scene/Camera3D.h"

#include "resources/MeshLoader.h"

namespace efe
{
	class cAI;
	class cGraphics;
	class cResources;
	class cSystem;
	class cSound;
	class cPhysics;
	class cHaptic;
	class cCollider2D;
	class iCamera;
	class cCamera2D;
	class cUpdater;
	class cWorld3D;
	class cWorld2D;

	typedef std::list<iCamera*> tCameraList;
	typedef tCameraList::iterator tCameraListIt;

	typedef std::list<cWorld3D*> tWorld3DList;
	typedef tWorld3DList::iterator tWorld3DListIt;

	class cScene : public iUpdateable
	{
	public:
		cScene(cGraphics *a_pGraphics, cResources *a_pResources, cSound *a_pSound,
			cPhysics *a_pPhysics, cSystem *a_pSystem, cAI *a_pAI, cHaptic *a_pHaptic);
		~cScene();

		void Reset();

		void UpdateRenderList(float a_fFrameTime);

		void Render(cUpdater *a_pUpdater, float a_fFrameTime);

		bool LoadMap2D(tString a_sFile);

		void RenderWorld2D(cCamera2D *a_pCam, cWorld2D *a_pWorld);

		void Update(float a_fTimeStep);

		void ClearLoadedMaps(){m_setLoadedMaps.clear();}
		tStringSet *GetLoadedMapSet(){return &m_setLoadedMaps;}

		void SetDrawScene(bool a_bX);
		bool GetDrawScene(){return m_bDrawScene;}

		////// SCRIPT VAR MEHODS ////////////

		cScriptVar *CreateLocalVar(const tString &a_sName);
		cScriptVar *GetLocalVar(const tString &a_sName);
		tScriptVarMap *GetLocalVarMap();
		cScriptVar *CreateGlobalVar(const tString &a_sName);
		cScriptVar *GetGlobalVar(const tString &a_sName);
		tScriptVarMap *GetGlobalVarMap();

		////// CAMERA MEHODS ////////////

		cCamera2D *CreateCamera2D(unsigned int a_lW, unsigned int a_lH);
		cCamera3D *CreateCamera3D(eCameraMoveMode a_MoveMode);

		void DestroyCamera(iCamera *a_pCam);

		void SetCamera(iCamera *a_pCam);
		iCamera *GetCamera(){return m_pActiveCamera;}
		void SetCameraPosition(const cVector3f &a_vPos);
		cVector3f GetCameraPosition();

		void SetCameraListener(bool a_bX){m_bCameraIsListener = a_bX;}

		////// WORLD MEHODS ////////////

		cWorld3D *LoadWorld3D(const tString &a_sFile, bool a_bLoadScript, tWorldLoadFlag a_Flags);
		cWorld3D *CreateWorld3D(const tString &a_sName);
		void DestroyWorld3D(cWorld3D *a_pWorld);

		void SetWorld3D(cWorld3D *a_pWorld);
		cWorld3D *GetWorld3D(){return m_pCurrentWorld3D;}

		bool HasLoadedWorld(const tString &a_sFile);

		void SetUpdateMap(bool a_bX){m_bUpdateMap = a_bX;}
		bool GetUpdateMap(){return m_bUpdateMap;}
	private:
		cGraphics *m_pGraphics;
		cResources *m_pResources;
		cSound *m_pSound;
		cPhysics *m_pPhysics;
		cSystem *m_pSystem;
		cAI *m_pAI;
		cHaptic *m_pHaptic;

		cCollider2D *m_pCollider2D;

		bool m_bDrawScene;
		bool m_bUpdateMap;

		tWorld3DList m_lstWorld3D;
		cWorld3D *m_pCurrentWorld3D;

		cWorld2D *m_pCurrentWorld2D;
		tCameraList m_lstCamera;
		iCamera *m_pActiveCamera;
		bool m_bCameraIsListener;

		tScriptVarMap m_mapLocalVars;
		tScriptVarMap m_mapGlobalVars;

		tStringSet m_setLoadedMaps;
	};
};
#endif