#ifndef EFE_MESH_LOADER_H
#define EFE_MESH_LOADER_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"

namespace efe
{
	typedef tFlag tWorldLoadFlag;

	#define eWorldLoadFlag_NoLights			(0x00000001)
	#define eWorldLoadFlag_NoEntities		(0x00000002)
	#define eWorldLoadFlag_NoGameEntites	(0x00000003)

	typedef tFlag tMeshLoadFlag;

	#define eMeshLoadFlag_NoGeometry		(0x0000000)

	class cMesh;

	class cResources;
	class cMaterialManager;
	class cMeshManager;
	class cAnimationManager;
	class cMeshLoaderHandler;
	class iLowLevelGraphics;
	class cWorld3D;
	class cScene;
	class cAnimation;
	class cSystem;

	class iMeshLoader
	{
		friend class cMeshLoaderHandler;
	public:
		iMeshLoader(iLowLevelGraphics *a_pLowLevelGraphics)
			:m_pLowLevelGraphics(a_pLowLevelGraphics){}

		virtual cMesh *LoadMesh(const tString &a_sFile, tMeshLoadFlag a_Flags)=0;
		virtual bool SaveMesh(cMesh *a_pMesh, const tString &a_sFile)=0;

		virtual cWorld3D *LoadWorld(const tString &a_sFile, cScene *a_pScene, tWorldLoadFlag a_Flags)=0;

		//virtual cAnimation *LoadAnimation(const tString &a_sFile)=0;

		virtual bool IsSupported(const tString a_sFileType)=0;

		virtual void AddSupportedTypes(tStringVec *a_vFileTypes)=0;

		static void SetRestricStaticLightToSector(bool a_bX){m_bRestricStaticLightToSector = a_bX;}
		static void SetUseFastMaterial(bool a_bX){m_bUseFastMaterial = a_bX;}
		static void SetFastMaterialFile(const tString &a_sFile){m_sFastMaterialFile = a_sFile;}
		static void SetCacheDir(const tString &a_sDir){m_sCacheDir = a_sDir;}
	protected:
		cMaterialManager *m_pMaterialManager;
		cMeshManager *m_pMeshManager;
		cAnimationManager *m_pAnimationManager;
		iLowLevelGraphics *m_pLowLevelGraphics;
		cSystem *m_pSystem;

		static bool m_bRestricStaticLightToSector;
		static bool m_bUseFastMaterial;
		static tString m_sFastMaterialFile;
		static tString m_sCacheDir;
	};
};

#endif