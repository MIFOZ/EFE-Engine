#ifndef EFE_VIDEO_MANAGER_H
#define EFE_VIDEO_MANAGER_H

#include "resources/ResourceManager.h"

namespace efe
{
	class cResources;
	class cGraphics;
	class iVideoStream;
	class iVideoStreamLoader;

	typedef std::list<iVideoStreamLoader*> tVideoStreamLoaderList;
	typedef tVideoStreamLoaderList::iterator tVideoStreamLoaderListIt;

	typedef std::list<iVideoStream*> tVideoStreamList;
	typedef tVideoStreamList::iterator tVideoStreamListIt;

	class cVideoManager : public iResourceManager
	{
	public:
		cVideoManager(cGraphics *a_pGraphics, cResources *a_pResources);
		~cVideoManager();

		iResourceBase *Create(const tString &a_sName);

		iVideoStream *CreateVideo(const tString &a_sName);

		void AddVideoLoader(iVideoStreamLoader *a_pLoader);

		void Destroy(iResourceBase *a_pResource);
		void Unload(iResourceBase *a_pResource);

		void Update(float a_fTimeStep);

	private:
		iVideoStreamLoader *GetLoader(const tString &a_sFileName);

		cGraphics *m_pGraphics;
		cResources *m_pResources;

		tVideoStreamLoaderList m_lstVideoLoaders;
	};
};

#endif