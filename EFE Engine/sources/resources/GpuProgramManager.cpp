#include "resources/GpuProgramManager.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/GpuProgram.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGpuProgramManager::cGpuProgramManager(cFileSearcher *a_pFileSearcher, iLowLevelGraphics *a_pLowLevelGraphics,
		iLowLevelResources *a_pLowLevelResource, iLowLevelSystem *a_pLowLevelSystem)
		: iResourceManager(a_pFileSearcher, a_pLowLevelResource, a_pLowLevelSystem)
	{
		m_pLowLevelGraphics = a_pLowLevelGraphics;
	}

	cGpuProgramManager::~cGpuProgramManager()
	{
		DestroyAll();

		Log(" Done with Gpu programs\n");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iResourceBase *cGpuProgramManager::Create(const tString &a_sName)
	{
		return NULL;
	}

	iGpuProgram *cGpuProgramManager::CreateProgram(const tString &a_sName, const tString &a_sEntry, eGpuProgramType a_Type, tGpuProgramDefineVec &a_vDefines)
	{
		tString sPath;
		iGpuProgram *pProgram;
		tString sNewName = cString::GetFileName(a_sName) + "_" + a_sEntry;
		pProgram = static_cast<iGpuProgram*>(FindLoadedResource(a_sName,sPath));

		BeginLoad(a_sName);

		if (pProgram==NULL && sPath!="")
		{
		 	tGpuProgramsMapIt it = m_mapGpuPrograms.find(sNewName);
			if (it != m_mapGpuPrograms.end())
			{
				pProgram = it->second;
			}
			else
			{
				pProgram = m_pLowLevelGraphics->CreateGpuProgram(sNewName, a_Type);

				if (pProgram->CreateFromFile(sPath, a_sEntry, a_vDefines)==false)
				{
					Error("Couldn't create program '%s'\n", a_sName.c_str());
					efeDelete(pProgram);
					EndLoad();
					return NULL;
				}

				m_mapGpuPrograms.insert(tGpuProgramsMap::value_type(sNewName, pProgram));

				AddResource(pProgram);
			}
		}

		if (pProgram)pProgram->IncUserCount();
		else Error("Couldn't load program '%s'\n", a_sName.c_str());

		EndLoad();
		return pProgram;
	}

	//--------------------------------------------------------------

	void cGpuProgramManager::Destroy(iResourceBase *a_pResource)
	{
		a_pResource->DecUserCount();

		if (a_pResource->HasUsers()==false)
		{
			RemoveResource(a_pResource);
			efeDelete(a_pResource);
		}
	}

	void cGpuProgramManager::Unload(iResourceBase *a_pResource)
	{
		
	}
}