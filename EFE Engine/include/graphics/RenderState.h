#ifndef EFE_RENDER_SET_H
#define EFE_RENDER_SET_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "graphics/Material.h"

namespace efe
{
	class cRenderSettings;
	class iRenderable;
	class iLight3D;
	class cSector;

	enum eRenderStateType
	{
		eRenderStateType_Sector = 0,
		eRenderStateType_Pass = 1,
		eRenderStateType_DepthTest = 2,
		eRenderStateType_Depth = 3,
		eRenderStateType_AlphaMode = 4,
		eRenderStateType_BlendMode = 5,
		eRenderStateType_VertexProgram = 6,
		eRenderStateType_PixelProgram = 7,
		eRenderStateType_Texture = 8,
		eRenderStateType_VertexBuffer = 9,
		eRenderStateType_Sampler = 9,
		eRenderStateType_Matrix = 10,
		eRenderStateType_Render = 11,
		eRenderStateType_LastEnum = 12
	};

	class iRenderState
	{
	public:
		iRenderState(){}

		int Compare(const iRenderState *a_pState) const;
		void SetMode(cRenderSettings *a_pSettings);

		void Set(const iRenderState *a_pState);

		eRenderStateType m_Type;

		cSector *m_pSector;

		int m_lPass;

		bool m_bDepthTest;

		//Depth
		float m_fZ;

		//Alpha
		eMaterialAlphaMode m_AlphaMode;

		//Blend
		eMaterialBlendMode m_BlendMode;
		eMaterialChannelMode m_ChannelMode;

		//Vertex program
		iGpuProgram *m_pVtxProgram;
		iMaterialProgramSetup *m_pVtxProgramSetup;
		bool m_bUsesLight;
		bool m_bUsesEye;
		iLight3D *m_pLight;

		//Pixel program
		iGpuProgram *m_pPxlProgram;
		iMaterialProgramSetup *m_pPxlProgramSetup;

		//Texture
		cMaterialTextureParam *m_pTextureParams[eMaterialTexture_LastEnum];
		iTexture *m_pTexture[eMaterialTexture_LastEnum];
		//iTexture *m_pTexture[MAX_TEXTUREUNITS];
		eMaterialBlendMode m_TextureBlend[MAX_TEXTUREUNITS];

		//Texture sampler
		eTextureAddressMode m_TextureAddressMode[MAX_TEXTUREUNITS];
		eTextureFilter m_TextureFilter[MAX_TEXTUREUNITS];

		//Vertex buffer
		iVertexBuffer *m_pVtxBuffer;

		//Matrix
		cMatrixf *m_pModelMatrix;
		cMatrixf *m_pInvModelMatrix;

		//Scale
		cVector3f m_vScale;

		//Render
		iRenderable *m_pObject;
		cMaterialRenderEffect *m_pRenderEffect;
	private:
		//Compare
		int CompareSector(const iRenderState *a_pSettings)const;
		int ComparePass(const iRenderState *a_pState)const;
		int CompareDepthTest(const iRenderState *a_pState)const;
		int CompareDepth(const iRenderState *a_pState)const;
		int CompareAlpha(const iRenderState *a_pState)const;
		int CompareBlend(const iRenderState *a_pState)const;
		int CompareVtxProg(const iRenderState *a_pState)const;
		int ComparePxlProg(const iRenderState *a_pState)const;
		int CompareTexture(const iRenderState *a_pState)const;
		int CompareVtxBuff(const iRenderState *a_pState)const;
		int CompareMatrix(const iRenderState *a_pState)const;
		int CompareRender(const iRenderState *a_pState)const;
		//Set mode
		void SetSectorMode(cRenderSettings *a_pSettings);
		void SetPassMode(cRenderSettings *a_pSettings);
		void SetDepthTestMode(cRenderSettings *a_pSettings);
		void SetDepthMode(cRenderSettings *a_pSettings);
		void SetAlphaMode(cRenderSettings *a_pSettings);
		void SetBlendMode(cRenderSettings *a_pSettings);
		void SetVtxProgMode(cRenderSettings *a_pSettings);
		void SetPxlProgMode(cRenderSettings *a_pSettings);
		void SetTextureMode(cRenderSettings *a_pSettings);
		void SetSamplerMode(cRenderSettings *a_pSettings);
		void SetVtxBuffMode(cRenderSettings *a_pSettings);
		void SetMatrixMode(cRenderSettings *a_pSettings);
		void SetRenderMode(cRenderSettings *a_pSettings);
	};
};

#endif