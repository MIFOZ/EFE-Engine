#include "resources/AnimationManager.h"
#include "system/String.h"
#include "system/System.h"
#include "resources/Resources.h"
#include "graphics/Mesh.h"
#include "graphics/Animation.h"
#include "system/LowLevelSystem.h"
#include "resources/MeshLoaderHandler.h"
#include "resources/FileSearcher.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cAnimationManager::cAnimationManager(cGraphics *a_pGraphics, cResources *a_pResources)
		:iResourceManager(a_pResources->GetFileSearcher(), a_pResources->GetLowLevel(),
		a_pResources->GetLowLevelSystem())
	{
		m_pGraphics = a_pGraphics;
		m_pResources = a_pResources;
	}

	cAnimationManager::~cAnimationManager()
	{
		DestroyAll();

		Log(" Done with animations\n");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iResourceBase *cAnimationManager::Create(const tString &a_sName)
	{
		return CreateAnimation(a_sName);
	}

	//--------------------------------------------------------------

	cAnimation *cAnimationManager::CreateAnimation(const tString &a_sName)
	{
		tString sPath;
		cAnimation *pAnimation = NULL;
		tString sNewName;

		BeginLoad(a_sName);

		sNewName = a_sName;

		if (cString::GetFileExt(a_sName) == "")
		{
			bool bFound = false;
			tStringVec *pTypes = m_pResources->GetMeshLoaderHandler()->GetSupportedTypes();
			for (size_t i=0; i<pTypes->size(); i++)
			{
				sNewName = cString::SetFileExt(sNewName, (*pTypes)[i]);
				sPath = m_pResources->GetFileSearcher()->GetFilePath(sNewName);
				if (sPath != "")
				{
					bFound = true;
					break;
				}
			}


			if (bFound == false)
			{
				Error("Couldn't create mesh '%s'\n", a_sName.c_str());
				EndLoad();
				return NULL;
			}
		}

		pAnimation = static_cast<cAnimation*>(FindLoadedResource(a_sName,sPath));

		if (pAnimation==NULL && sPath!="")
		{
			cMeshLoaderHandler *pMeshLoaderHandler = m_pResources->GetMeshLoaderHandler();

			cMesh *pTempMesh = pMeshLoaderHandler->LoadMesh(sPath, 0);
			if (pTempMesh == NULL)
			{
				Error("Couldn't load animation from '%s'\n", sPath.c_str());
				EndLoad();
				return NULL;
			}

			if (pTempMesh->GetAnimationNum()<=0)
			{
				Error("No animations found in '%s'\n", sPath.c_str());
				efeDelete(pTempMesh);
				EndLoad();
				return NULL;
			}

			pAnimation = pTempMesh->GetAnimation(0);
			pTempMesh->ClearAnimations(false);

			efeDelete(pTempMesh);

			AddResource(pAnimation);
		}

		if (pAnimation) pAnimation->IncUserCount();
		else Error("Couldn't create animation '%s'\n", sNewName.c_str());

		EndLoad();
		return pAnimation;
	}

	//--------------------------------------------------------------

	void cAnimationManager::Destroy(iResourceBase *a_pResource)
	{
		a_pResource->DecUserCount();

		if (a_pResource->HasUsers()==false)
		{
			RemoveResource(a_pResource);
			efeDelete(a_pResource);
		} 
	}

	//--------------------------------------------------------------

	void cAnimationManager::Unload(iResourceBase *a_pResource)
	{
		
	}
}