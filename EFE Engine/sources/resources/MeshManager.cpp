#include "resources/MeshManager.h"
#include "system/String.h"
#include "system/System.h"
#include "resources/Resources.h"
#include "graphics/Mesh.h"
#include "system/LowLevelSystem.h"
#include "resources/MeshLoaderhandler.h"
#include "resources/FileSearcher.h"

namespace efe
{
	cMeshManager::cMeshManager(cGraphics *a_pGraphics, cResources *a_pResources)
		: iResourceManager(a_pResources->GetFileSearcher(),a_pResources->GetLowLevel(),
							a_pResources->GetLowLevelSystem())
	{
		m_pGraphics = a_pGraphics;
		m_pResources = a_pResources;
	}

	cMeshManager::~cMeshManager()
	{
		DestroyAll();

		Log("Done with meshes\n");
	}

	//--------------------------------------------------------------

	cMesh *cMeshManager::CreateMesh(const tString &a_sName)
	{
		tString sPath;
		cMesh *pMesh;
		tString a_sNewName;

		BeginLoad(a_sName);

		a_sNewName = a_sName;

		if (cString::GetFileExt(a_sNewName) == "")
		{
			bool bFound = false;
			tStringVec *pTypes = m_pResources->GetMeshLoaderHandler()->GetSupportedTypes();
			for (size_t i=0; i < pTypes->size();i++)
			{
				a_sNewName = cString::SetFileExt(a_sNewName, (*pTypes)[i]);
				tString sPath = m_pResources->GetFileSearcher()->GetFilePath(a_sNewName);
				if (sPath != "")
				{
					bFound = true;
					break;
				}
			}

			if (!bFound)
			{
				Error("Couldn't create mesh '%s'\n", a_sName.c_str());
				EndLoad();
				return NULL;
			}
		}

		pMesh = static_cast<cMesh*> (this->FindLoadedResource(a_sNewName, sPath));

		if (pMesh==NULL && sPath != "")
		{
			pMesh = m_pResources->GetMeshLoaderHandler()->LoadMesh(sPath, 0);
			if (pMesh == NULL)
			{
				EndLoad();
				return NULL;
			}

			AddResource(pMesh);
		}

		if (pMesh) pMesh->IncUserCount();
		else Error("Couldn't create mesh '%s'\n", a_sNewName.c_str());

		EndLoad();
		return pMesh;
	}

	iResourceBase *cMeshManager::Create(const tString &a_sName)
	{
		return CreateMesh(a_sName);
	}

	//--------------------------------------------------------------

	void cMeshManager::Unload(iResourceBase *a_pResource)
	{
	}

	//--------------------------------------------------------------

	void cMeshManager::Destroy(iResourceBase *a_pResource)
	{
		a_pResource->DecUserCount();

		if (a_pResource->HasUsers()==false)
		{
			RemoveResource(a_pResource);
			efeDelete(a_pResource);
		}
	}
	//--------------------------------------------------------------
};