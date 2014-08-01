#ifndef EFE_RESOURCES_H
#define EFE_RESOURCES_H

#include <map>
#include "system/SystemTypes.h"

#include "graphics/GPUProgram.h"

#include "game/Updateable.h"

class TiXmlElement;

namespace efe
{
	class iLowLevelResources;
	class iLowLevelGraphics;
	class iLowLevelSystem;
	class cSystem;
	class iResourceManager;
	class cFileSearcher;
	class cImageManager;
	class cGpuProgramManager;
	class cTileSetManager;
	class cSoundManager;
	class cFontManager;
	class cScriptManager;
	class cTextureManager;
	class cMaterialManager;
	class cSoundEntityManager;
	class cAnimationManager;
	class cMeshManager;
	class cVideoManager;
	class cSound;
	class cMeshLoaderHandler;
	class cScene;
	class cGraphics;
	class cWorld3D;
	class iEntity3D;
	class cGui;

	class iEntity3DLoader
	{
	public:
		iEntity3DLoader(const tString &a_sName) : m_sName(a_sName){}
		virtual ~iEntity3DLoader(){}

		const tString &GetName(){return m_sName;}

		virtual iEntity3D *Load(const tString &a_sName, TiXmlElement *a_pRootElem, const cMatrixf &a_mtxTransform,
								cWorld3D *a_pWorld, const tString &a_sFileName, bool a_bLoadReferences)=0;
	protected:
		tString m_sName;
	};

	typedef std::list<iResourceManager*> tResourceManagerList;
	typedef tResourceManagerList::iterator tResourceManagerListIt;

	typedef std::map<tString, iEntity3DLoader*> tEntity3DLoaderMap;
	typedef tEntity3DLoaderMap::iterator tEntity3DLoaderMapIt;

	//--------------------------------------------------------------

	class iArea3DLoader
	{
	public:
		iArea3DLoader(const tString &a_sName) : m_sName(a_sName){}
		virtual ~iArea3DLoader();

		const tString &GetName() {return m_sName;}

		virtual iEntity3D *Load(const tString &a_sName, const cVector3f &a_vSize, const cMatrixf &a_mtxTransform, cWorld3D *a_pWorld)=0;

	protected:
		tString m_sName;
	};

	typedef std::map<tString, iArea3DLoader*> tArea3DLoaderMap;
	typedef tArea3DLoaderMap::iterator tArea3DLoaderMapIt;

	//--------------------------------------------------------------

	typedef std::list<iResourceManager*> tResourcesManagerList;
	typedef tResourcesManagerList::iterator tResourcesManagerListIt;

	//--------------------------------------------------------------

	class cResources : public iUpdateable
	{
	public:
		cResources(iLowLevelResources *a_pLowLevelResources, iLowLevelGraphics *a_pLowLevelGraphics);
		~cResources();

		void Init(cGraphics *a_pGraphics,cSystem *a_pSystem,cSound *a_pSound, cScene *a_pScene, cGui *a_pGui);
		
		void Update(float a_fTimeStep);

		iLowLevelResources *GetLowLevel();
		cFileSearcher *GetFileSearcher();

		bool AddResourceDir(const tString &a_sDir, const tString &a_sMask = "*.*");
		void ClearResourceDirs();

		void AddEntity3DLoader(iEntity3DLoader *a_pLoader, bool a_bSetAsDefault = false);
		iEntity3DLoader *GetEntity3DLoader(const tString &a_sName);

		void AddArea3DLoader(iArea3DLoader *a_pLoader, bool a_bSetAsDefault = false);
		iArea3DLoader *GetArea3DLoader(const tString &a_sName);

		bool LoadResourceDirsFile(const tString &a_sFile);

		cImageManager *GetImageManager(){return m_pImageManager;}
		cGpuProgramManager *GetGpuProgramManager(){return m_pGpuProgramManager;}
		cTileSetManager *GetTileSetManager(){return m_pTileSetManager;}
		cSoundManager *GetSoundManager(){return m_pSoundManager;}
		cFontManager *GetFontManager(){return m_pFontManager;}
		cScriptManager *GetScriptManager(){return m_pScriptManager;}
		cTextureManager *GetTextureManager(){return m_pTextureManager;}
		cMaterialManager *GetMaterialManager(){return m_pMaterialManager;}
		cMeshManager *GetMeshManager(){return m_pMeshManager;}
		cMeshLoaderHandler *GetMeshLoaderHandler(){return m_pMeshLoaderHandler;}
		cSoundEntityManager *GetSoundEntityManager(){return m_pSoundEntityManager;}
		cAnimationManager *GetAnimationManager(){return m_pAnimationManager;}
		cVideoManager *GetVideoManager(){return m_pVideoManager;}

		iLowLevelSystem *GetLowLevelSystem(){return m_pLowLevelSystem;}
	private:
		iLowLevelResources *m_pLowLevelResources;
		iLowLevelGraphics *m_pLowLevelGraphics;
		iLowLevelSystem *m_pLowLevelSystem;
		cFileSearcher *m_pFileSearcher;

		tResourcesManagerList m_lstManagers;
		cImageManager *m_pImageManager;
		cGpuProgramManager *m_pGpuProgramManager;
		cTileSetManager *m_pTileSetManager;
		cSoundManager *m_pSoundManager;
		cFontManager *m_pFontManager;
		cScriptManager *m_pScriptManager;
		cTextureManager *m_pTextureManager;
		cMaterialManager *m_pMaterialManager;
		cSoundEntityManager *m_pSoundEntityManager;
		cAnimationManager *m_pAnimationManager;
		cVideoManager *m_pVideoManager;

		cMeshManager *m_pMeshManager;
		cMeshLoaderHandler *m_pMeshLoaderHandler;

		tEntity3DLoaderMap m_mapEntity3DLoaders;
		iEntity3DLoader *m_pDefaultEntity3DLoader;

		tArea3DLoaderMap m_mapArea3DLoaders;
		iArea3DLoader *m_pDefaultArea3DLoader;

		tWString m_wsEmptyString;
	};
};
#endif