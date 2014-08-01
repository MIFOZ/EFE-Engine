#include "resources/ScriptManager.h"
#include "system/String.h"
#include "system/System.h"
#include "resources/Resources.h"
#include "system/Script.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cScriptManager::cScriptManager(cSystem *a_pSystem, cResources *a_pResources)
		: iResourceManager(a_pResources->GetFileSearcher(), a_pResources->GetLowLevel(),
							a_pResources->GetLowLevelSystem())
	{
		m_pSystem = a_pSystem;
		m_pResources = a_pResources;
	}

	cScriptManager::~cScriptManager()
	{
		DestroyAll();
		Log(" Done with scripts\n");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	iResourceBase *cScriptManager::Create(const tString &a_sName)
	{
		return CreateScript(a_sName);
	}

	//-------------------------------------------------------------

	iScript *cScriptManager::CreateScript(const tString &a_sName)
	{
		tString sPath;
		iScript *pScript;
		tString sNewName;

		BeginLoad(a_sName);

		sNewName = cString::SetFileExt(a_sName, "efs");

		pScript = static_cast<iScript*>(this->FindLoadedResource(sNewName, sPath));

		if (pScript==NULL && sPath!="")
		{
			pScript = m_pSystem->GetLowLevel()->CreateScript(sNewName);

			if (pScript->CreateFromFile(sPath)==false)
			{
				efeDelete(pScript);
				EndLoad();
				return NULL;
			}

			AddResource(pScript);
		}

		if (pScript) pScript->IncUserCount();
		else Error("Couldn't create script '%s'\n", sNewName.c_str());

		EndLoad();
		return pScript;
	}

	//-------------------------------------------------------------

	void cScriptManager::Destroy(iResourceBase *a_pResource)
	{
		a_pResource->DecUserCount();

		if (a_pResource->HasUsers()==false)
		{
			RemoveResource(a_pResource);
			efeDelete(a_pResource);
		}
	}

	//-------------------------------------------------------------

	void cScriptManager::Unload(iResourceBase *a_pResource)
	{
		
	}
}