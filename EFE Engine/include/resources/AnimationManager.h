#ifndef EFE_ANIMATION_MANAGER_H
#define EFE_ANIMATION_MANAGER_H

#include "resources/ResourceManager.h"

namespace efe
{
	class cGraphics;
	class cResources;
	class cAnimation;

	class cAnimationManager : public iResourceManager
	{
	public:
		cAnimationManager(cGraphics *a_pGraphics, cResources *a_pResources);
		~cAnimationManager();

		iResourceBase *Create(const tString &a_sName);
		cAnimation *CreateAnimation(const tString &a_sName);

		void Destroy(iResourceBase *a_pResource);
		void Unload(iResourceBase *a_pResource);

	private:
		cGraphics *m_pGraphics;
		cResources *m_pResources;
	};
};
#endif