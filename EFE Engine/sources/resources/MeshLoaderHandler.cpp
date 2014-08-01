#include "resources/MeshLoaderHandler.h"

#include "resources/MeshLoader.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "scene/Scene.h"

namespace efe
{
	bool iMeshLoader::m_bRestricStaticLightToSector = false;
	bool iMeshLoader::m_bUseFastMaterial = false;
	tString iMeshLoader::m_sFastMaterialFile = "";
	tString iMeshLoader::m_sCacheDir = "core/cache";

	cMeshLoaderHandler::cMeshLoaderHandler(cResources *a_pResources, cScene *a_pScene)
	{
		m_pResources = a_pResources;
		m_pScene = a_pScene;
	}

	cMeshLoaderHandler::~cMeshLoaderHandler()
	{
		tMeshLoaderListIt it = m_lstLoaders.begin();
		for (;it != m_lstLoaders.end(); it++)
			efeDelete(*it);
		m_lstLoaders.clear();
	}

	cMesh *cMeshLoaderHandler::LoadMesh(const tString &a_sFile, tMeshLoadFlag a_tFlags)
	{
		tString sType = cString::ToLowerCase(cString::GetFileExt(a_sFile));

		tMeshLoaderListIt it = m_lstLoaders.begin();
		for (;it != m_lstLoaders.end(); it++)
		{
			iMeshLoader *pLoader = *it;

			if (pLoader->IsSupported(sType))
				return pLoader->LoadMesh(a_sFile, a_tFlags);
		}

		Log("No loader for '%s' found!\n", sType.c_str());
		return NULL;
	}

	bool cMeshLoaderHandler::SaveMesh(cMesh *a_pMesh, const tString &a_sFile)
	{
		tString sType = cString::ToLowerCase(cString::GetFileExt(a_sFile));

		tMeshLoaderListIt it = m_lstLoaders.begin();
		for (;it != m_lstLoaders.end();++it)
		{
			iMeshLoader *pLoader = *it;

			if (pLoader->IsSupported(sType))
				return pLoader->SaveMesh(a_pMesh,a_sFile);
		}
		Log("No loader for '%s' found!\n",sType.c_str());
		return false;
	}

	//cWorld3D *cMeshLoaderHandler::LoadWorld(const tString &a_sFile, cScene *a_pScene, tWorldLoadFlag a_Flags)
	//{
	//}

	void cMeshLoaderHandler::AddLoader(iMeshLoader *a_pLoader)
	{
		m_lstLoaders.push_back(a_pLoader);

		a_pLoader->m_pMaterialManager = m_pResources->GetMaterialManager();
		a_pLoader->m_pMeshManager = m_pResources->GetMeshManager();
		a_pLoader->m_pAnimationManager = m_pResources->GetAnimationManager();
		//a_pLoader->m_pSystem = m_pScene-

		a_pLoader->AddSupportedTypes(&m_vSupportedTypes);
	}
}