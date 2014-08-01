#ifndef EFE_MATERIAL_MANAGER_H
#define EFE_MATERIAL_MANAGER_H

#include "resources/ResourceManager.h"

#include "graphics/Material.h"

namespace efe
{
	class cGraphics;
	class cResources;
	class iMaterial;

	class cMaterialManager : public iResourceManager
	{
	public:
		cMaterialManager(cGraphics *a_pGraphics, cResources *a_pResources);
		~cMaterialManager();

		iResourceBase *Create(const tString &a_sName);
		iMaterial *CreateMaterial(const tString &a_sName);

		void Update(float a_fTimeStep);

		void Destroy(iResourceBase *a_pResource);
		void Unload(iResourceBase *a_pResource);

		tString GetPhysicsMaterialName(const tString &a_sName);

	private:
		iMaterial *LoadFromFile(const tString &a_sName, const tString &a_sPath);

		eTextureTarget GetTarget(const tString &a_sType);
		tString GetTextureString(eMaterialTexture a_Type);
		eTextureAddressMode GetAddressMode(const tString &a_sType);
		eTextureAnimMode GetAnimMode(const tString &a_sType);

		unsigned int m_lTextureSizeLevel;

		float m_fTextureAnisotropy;

		tStringList m_lstFileFormats;

		tStringVec m_vCubeSideSuffixes;

		cGraphics *m_pGraphics;
		cResources *m_pResources;

		int m_lIdCounter;
	};
};
#endif