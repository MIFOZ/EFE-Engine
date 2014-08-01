#ifndef EFE_RENDERER3D_H
#define EFE_RENDERER3D_H

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"
#include "graphics/Material.h"
#include "math/Frustum.h"
#include "graphics/GpuParameterContainer.h"

namespace efe
{
#define eRendererDebugFlag_DrawTangents			(0x00000001)
#define eRendererDebugFlag_DrawNormals			(0x00000002)
#define eRendererDebugFlag_DisableLighting		(0x00000004)
#define eRendererDebugFlag_LogRendering			(0x00000008)
#define eRendererDebugFlag_DrawBoundingBox		(0x00000010)
#define eRendererDebugFlag_DrawBoundingSphere	(0x00000020)
#define eRendererDebugFlag_DrawLightBoundingBox	(0x00000040)
#define eRendererDebugFlag_DrawLines			(0x00000080)

	typedef tFlag tRendererDebugFlag;

	class cResources;
	class iLowLevelGraphics;
	class iLowLevelResources;
	class iTexture;
	class cCamera3D;
	class cWorld3D;
	class iVertexBuffer;
	class cMeshCreator;
	class cRenderList;
	class iRenderable;
	class iLight3D;
	class cRenderPostEffects;
	class cSector;

	enum eRendererShowShadows
	{
		eRendererShowShadows_All,
		eRendererShowShadows_Static,
		eRendererShowShadows_None,
		eRendererShowShadows_LastEnum
	};

	class cRenderSettings
	{
	public:
		cRenderSettings();

		void Clear();
		void Reset(iLowLevelGraphics *a_pLowLevel);
		
		iGpuProgram *m_pVtxExtrudeProgram;
		iGpuProgram *m_pPxlExtrudeProgram;
		iLowLevelGraphics *m_pLowLevel;

		iTexture *m_pBaseRT;
		iTexture *m_pNormalRT;
		iTexture *m_pDepthRT;
		iTexture *m_pResultRT;
		iTexture *m_pGeometryRT;

		unsigned int *m_pTempIndexArray;

		iLight3D *m_pLight;
		cCamera3D *m_pCamera;
		cFrustum *m_pFrustum;

		cSector *m_pSector;

		bool m_bFogActive;
		bool m_bFogCulling;
		float m_fFogStart;
		float m_fFogEnd;
		cColor m_FogColor;

		eRendererShowShadows m_ShowShadows;
		bool m_bLog;
		tRendererDebugFlag m_DebugFlags;

		int m_lLastShadowAlgo;

		bool m_bDepthTest;

		eMaterialAlphaMode m_AlphaMode;
		eMaterialBlendMode m_BlendMode;

		iGpuProgram *m_pVertexProgram;
		bool m_bVertexUsusLight;
		iMaterialProgramSetup *m_pVtxProgramSetup;

		iGpuProgram *m_pPixelProgram;

		bool m_bMatrixWasNULL;

		bool m_bUsesLight;
		bool m_bUsesEye;

		cColor m_AmbientColor;

		iTexture *m_pTexture[MAX_TEXTUREUNITS];

		eTextureAddressMode m_TextureAddressMode[MAX_TEXTUREUNITS];
		eTextureFilter m_TextureFilter[MAX_TEXTUREUNITS];

		iVertexBuffer *m_pVtxBuffer;
	};

	class cGpuProgramParameterContainer;

	class cRenderer3D
	{
	public:
		cRenderer3D(iLowLevelGraphics *a_pLowLevelGraphics, cResources *a_pResources,
					cMeshCreator* a_pMeshCreator, cRenderList * a_pRenderList);
		~cRenderer3D();

		void UpdateRenderList(cWorld3D *a_pWorld, cCamera3D *a_pCamera, float a_fFrameTime);

		void RenderWorld(cWorld3D *a_pWorld, cCamera3D *a_pCamera, float a_fFrameTime);

		void SetSkyBox(iTexture *a_pTexture, bool a_bAutoDestroy);
		void SetSkyBoxActive(bool a_bX);

		void SetAmbientColor(const  cColor &a_Color) {m_RenderSettings.m_AmbientColor = a_Color;}
		cColor GetAmbientColor() {return m_RenderSettings.m_AmbientColor;}

		cRenderList *GetRenderList(){return m_pRenderList;}

		//Debug setup
		void SetDebugFlags(tRendererDebugFlag a_Flags){m_DebugFlags = a_Flags;}

		//MY_DEBUG
		cVector3f GetLightPos(){return m_vLightPos;}
	private:
		cVector3f m_vLightPos;

		__forceinline void BeginRendering(cCamera3D *a_pCamera);

		void RenderSkyBox(cCamera3D *a_pCamera);

		void InitSkyBox();

		void RenderZ(cCamera3D *a_pCamera);

		void RenderDiffuse(cCamera3D *a_pCamera);

		void RenderLight(cCamera3D *a_pCamera);

		iLowLevelGraphics *m_pLowLevelGraphics;
		iLowLevelResources *m_pLowLevelResources;

		bool m_bLog;

		float m_fRenderTime;

		iGpuProgram *m_pDiffuseProgram;

		//cGpuProgramParameterContainer m_ParameterContainer;
		cVectorParameterWriter *m_pCameraPositionWriter;

		/*iGpuProgram *m_pDiffuseVtxProgram;
		iGpuProgram *m_pDiffusePxlProgram;
		iGpuProgram *m_pSolidFogVtxProgram;
		iGpuProgram *m_pSolidFogPxlProgram;*/

		cGpuProgramParameterContainer *m_pParameters;

		iGpuProgram *m_pFillBuffersVtxProgram;
		iGpuProgram *m_pFillBuffersPxlProgram;
		iGpuProgram *m_pAmbientVtxProgram;
		iGpuProgram *m_pAmbientPxlProgram;
		iGpuProgram *m_pLightingVtxProgram;
		iGpuProgram *m_pLightingPxlProgram;
		iGpuProgram *m_pCreateMaskProgram;

		iGpuProgram *m_pSkyBoxVtxProgram;
		iGpuProgram *m_pSkyBoxPxlProgram;

		iVertexFormat *m_pPlainVF;
		iVertexFormat *m_pBaseVF;
		iVertexFormat *m_pFormatPos;

		iSamplerState *m_pPointClamp;
		iSamplerState *m_pTriAnisoFilter;

		iTexture *m_pFogLinearSolidTexture;

		bool m_bRefractionAvailable;
		bool m_bRefractionUsed;

		cRenderSettings m_RenderSettings;

		iTexture *m_pBaseRT;
		iTexture *m_pGeometryRT;
		iTexture *m_pNormalRT;
		iTexture *m_pDepthRT;
		iTexture *m_pStencilMaskRT;

		iRasterizerState *m_pCullNone;
		iRasterizerState *m_pCullBack;
		iRasterizerState *m_pCullFront;

		iDepthStencilState *m_pDepthTest;
		iDepthStencilState *m_pDepthNoTest;
		iDepthStencilState *m_pStencilSet;
		iDepthStencilState *m_pStencilTest;

		iBlendState *m_pBlendAdd;

		cBoundingVolume m_FogBV;

		tVertexVec m_vVtxRect;

		cMeshCreator *m_pMeshCreator;
		cRenderList *m_pRenderList;

		iVertexBuffer *m_pSkyBox;
		iTexture *m_pSkyBoxTexture;
		bool m_bAutoDestroySkybox;
		bool m_bSkyBoxActive;
		cColor m_SkyBoxColor;

		iVertexBuffer *m_pLightSphere;

		cResources *m_pResources;

		tRendererDebugFlag m_DebugFlags;

		void RenderOpaque(cCamera3D *a_pCamera);

		void RenderAmbient();

		cMatrixf m_mtxViewProj;
		cColor m_vScaleBias;

	};
};

#endif