#ifndef EFE_GPU_PROGRAM_MANAGER_H
#define EFE_GPU_PROGRAM_MANAGER_H

#include "resources/ResourceManager.h"

#include "graphics/GpuProgram.h"

namespace efe
{
	class iLowLevelGraphics;

	class cGpuProgramManager : public iResourceManager
	{
	public:
		cGpuProgramManager(cFileSearcher *a_pFileSearcher, iLowLevelGraphics *a_pLowLevelGraphics,
			iLowLevelResources *a_pLowLevelResource, iLowLevelSystem *a_pLowLevelSystem);
		~cGpuProgramManager();

		iResourceBase *Create(const tString &a_sName);

		iGpuProgram *CreateProgram(const tString &a_sName, const tString &a_sEntry, eGpuProgramType a_Type, tGpuProgramDefineVec &a_vDefines = tGpuProgramDefineVec());

		void Destroy(iResourceBase *a_pResource);
		void Unload(iResourceBase *a_pResource);

	private:
		iLowLevelGraphics *m_pLowLevelGraphics;

		tGpuProgramsMap m_mapGpuPrograms;
		std::multimap<tString, tString> m_mapEntriesByNames;
	};
};
#endif