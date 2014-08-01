#ifndef EFE_SCRIPT_MANAGER_H
#define EFE_SCRIPT_MANAGER_H

#include "resources/ResourceManager.h"

namespace efe
{
	class cSystem;
	class cResources;
	class iScript;

	class cScriptManager : public iResourceManager
	{
	public:
		cScriptManager(cSystem *a_pSystem, cResources *a_pResources);
		~cScriptManager();

		iResourceBase *Create(const tString &a_sName);

		iScript *CreateScript(const tString &a_sName);

		void Destroy(iResourceBase *a_pResource);
		void Unload(iResourceBase *a_pResource);

	private:
		cSystem *m_pSystem;
		cResources *m_pResources;
	};
};
#endif