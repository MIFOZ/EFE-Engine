#ifndef EFE_TEXTURE_MANAGER_H
#define EFE_TEXTURE_MANAGER_H

#include "resources/ResourceManager.h"
#include "graphics/Texture.h"

namespace efe
{
	class cGraphics;
	class cResources;
	class iTexture;

	//--------------------------------------------------------------

	typedef std::map<tString, iTexture*> tTextureAttenuationMap;
	typedef tTextureAttenuationMap::iterator tTextureAttenuationMapIt;

	//--------------------------------------------------------------

	class cTextureManager : public iResourceManager
	{
	public:
		cTextureManager(cGraphics *a_pGraphics, cResources *a_pResources);
		~cTextureManager();

		iResourceBase *Create(const tString &a_sName);
		iTexture *Create1D(const tString &a_sName, bool a_bUseMapMips, bool a_bCompress=false, eTextureType a_Type=eTextureType_Normal,
							unsigned int a_lTextureSizeLevel=0);

		iTexture *Create2D(const tString &a_sName, bool a_bUseMapMips, bool a_bCompress=false, eTextureType a_Type=eTextureType_Normal,
							unsigned int a_lTextureSizeLevel=0, eTextureTarget a_Target=eTextureTarget_2D);

		iTexture *CreateAnim2D(const tString &a_sName, bool a_bUseMapMips, bool a_bCompress=false, eTextureType a_Type=eTextureType_Normal,
							unsigned int a_lTextureSizeLevel=0);

		iTexture *CreateCubeMap(const tString &a_sPathName, bool a_bUseMipMaps, eTextureType a_Type = eTextureType_Normal,
							unsigned int a_lArraySlices = 1, bool m_bSRGB = false);

		iTexture *CreateAttenuation(const tString &a_sFallOffName);

		void Destroy(iResourceBase *a_pResource);
		void Unload(iResourceBase *a_pResource);

		void Update(float a_fTimeStep);
	private:
		iTexture *CreateFlatTexture(const tString &a_sName, bool a_bUseMipMaps,
									bool a_bCompress, eTextureType a_Type, eTextureTarget a_Target,
									unsigned int a_lTextureSizeLevel);

		iTexture *FindTexture2D(const tString &a_sName, tString &a_sFilePath);

		tTextureAttenuationMap m_mapAttenuationTextures;

		tStringList m_lstFileFormats;

		tStringVec m_vCubeSideSuffixes;

		cGraphics *m_pGraphics;
		cResources *m_pResources;
	};
};
#endif