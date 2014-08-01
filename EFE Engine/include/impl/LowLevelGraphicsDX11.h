#ifndef EFE_LOWLEVELGRAPHICS_DX_H
#define EFE_LOWLEVELGRAPHICS_DX_H

#include "graphics/LowLevelGraphics.h"
#include "graphics/GpuParameterContainer.h"
#include "math/MathTypes.h"

#include <hash_map>


#include <D3D11.h>
#include <D3D11Shader.h>

namespace efe
{
#define SAFE_RELEASE(p) {if (p) {p->Release(); p = NULL;}}

	struct cRasterDescHashConfig
	{
		enum
		{              
			bucket_size = 4,
			min_buckets = 8
		};

		size_t operator()(const D3D11_RASTERIZER_DESC &key_value) const
		{
			return 1;
		}

		bool operator()(const D3D11_RASTERIZER_DESC &left, const D3D11_RASTERIZER_DESC &right) const
		{
			return false;
		}
	};

	struct cBlendDescHashConfig
	{
		enum
		{              
			bucket_size = 4,
			min_buckets = 8
		};

		size_t operator()(const D3D11_BLEND_DESC &key_value) const
		{
			return 1;
		}

		bool operator()(const D3D11_BLEND_DESC &left, const D3D11_BLEND_DESC &right) const
		{
			return false;
		}
	};

	struct cSamplerDescHashConfig
	{
		enum
		{              
			bucket_size = 4,
			min_buckets = 8
		};

		size_t operator()(const D3D11_SAMPLER_DESC &key_value) const
		{
			return 1;
		}

		bool operator()(const D3D11_SAMPLER_DESC &left, const D3D11_SAMPLER_DESC &right) const
		{
			return false;
		}
	};

	struct cInputSignatureVecHashConfig
	{
		enum
		{              
			bucket_size = 4,
			min_buckets = 8
		};

		size_t operator()(const std::vector<D3D11_SIGNATURE_PARAMETER_DESC> &key_value) const
		{
			return 1;
		}

		bool operator()(const std::vector<D3D11_SIGNATURE_PARAMETER_DESC> &left, const std::vector<D3D11_SIGNATURE_PARAMETER_DESC> &right) const
		{
			return false;
		}
	};

	class cDXPixelFormat;
	class cSurface;
	class cDX11PipelineManager;

	inline bool HasMipMaps(eTextureFilter a_Filter){return a_Filter >= eTextureFilter_Bilinear;}
	inline bool HasAniso(eTextureFilter a_Filter){return a_Filter >= eTextureFilter_BilinearAniso;}

	class cDX11RasterizerState : public iRasterizerState
	{
		friend class cDX11RasterizerStage;
	public:
		cDX11RasterizerState(ID3D11RasterizerState *a_pState):m_pRasterState(a_pState){}
		~cDX11RasterizerState()
		{
			if (m_pRasterState) m_pRasterState->Release();
		}
	private:
		ID3D11RasterizerState *m_pRasterState;
	};

	class cDXBlendState : public iBlendState
	{
		friend class cDX11OutputMergerStage;
	public:
		cDXBlendState(ID3D11BlendState *a_pState):m_pBlendState(a_pState){}
		~cDXBlendState()
		{
			if (m_pBlendState) m_pBlendState->Release();
		}
	private:
		ID3D11BlendState *m_pBlendState;
	};

	class cDXSamplerState : public iSamplerState
	{
	public:
		cDXSamplerState():m_pSamplerState(NULL){}
		~cDXSamplerState()
		{
			if (m_pSamplerState) m_pSamplerState->Release();
		}
		ID3D11SamplerState *m_pSamplerState;
	};

	class cDX11DepthStencilState : public iDepthStencilState
	{
		friend class cLowLevelGraphicsDX11;
		friend class cDX11OutputMergerStage;
	public:
		cDX11DepthStencilState(ID3D11DepthStencilState *a_pState):m_pDepthStencilState(a_pState){}

		~cDX11DepthStencilState()
		{
			if (m_pDepthStencilState) m_pDepthStencilState->Release();
		}
	private:
		ID3D11DepthStencilState *m_pDepthStencilState;
	};

	class cGpuProgramParameterManager;

	class cLowLevelGraphicsDX11 : public iLowLevelGraphics
	{
	public:
		cLowLevelGraphicsDX11(int a_hWindowHanlde);
		~cLowLevelGraphicsDX11();

		bool Init(int a_iWidth, int a_iHeight, int a_iBpp, bool a_bFullscreen, int a_lMultisampling,
			const tString &a_sWindowCaption);

		void _ShowCursor(bool a_bX);

		void SetVsyncActive(bool a_bX);

		void SetMultisamplingActive(bool a_bX);

		void SetGammaCorrect(float a_fX);
		float GetGammaCorrection();

		int GetMultisampling(){return m_lMultisampling;}

		void SetClipPlane(int a_lIdx, const cPlanef &a_Plane);
		cPlanef GetClipPlane(int a_lIdx, const cPlanef &a_Plane);
		void SetClipPlaneActive(int a_lIdx, bool a_bX);

		cVector2f GetScreenSize();
		cVector2f GetVirtualSize();
		void SetVirtualSize(cVector2f a_vSize);

		iBitmap2D *CreateBitmap2D(const cVector2l &a_vSize, eFormat a_Format);
		iFontData *CreateFontData(const tString &a_sName);

		//VERTEX BUFFER
		void SetVertexBuffer(iVertexBuffer *a_pBuffer);

		iTexture *CreateTexture(bool a_bUseMipMaps, eTextureType a_Type, eTextureTarget a_Target);
		iTexture *CreateTexture(const tString &a_sName, bool a_bUseMipMaps, eTextureType a_Type, eTextureTarget a_Target);
		iTexture *CreateTexture(iBitmap2D *a_pBmp, bool a_bUseMipMaps, eTextureType a_Type,
										eTextureTarget a_Target);
		iTexture *CreateTexture(const cVector2l &a_vSize, eFormat a_Format, cColor a_FillCol,
										bool a_bUseMipMaps, eTextureType a_Type,  eTextureTarget a_Target);

		iTexture *CreateRenderTarget(const cVector2l &a_vSize, const int a_lDepth, const int a_lMipMapCount, const int a_lArraySize,
										const eFormat a_Format, const int a_lMultisampling = 1, bool a_bSRGB = false, eTextureTarget a_Target = eTextureTarget_2D);

		iTexture *CreateRenderDepth(const cVector2l &a_vSize, const int a_lArraySize, const eFormat a_Format, bool a_bSample = false, int a_lMultisampling = 1, eTextureTarget a_Target = eTextureTarget_2D);

		iGpuProgram *CreateGpuProgram(const tString &a_sName, eGpuProgramType a_Type);

		iSamplerState *CreateSamplerState(eTextureFilter a_Filter, eTextureAddressMode a_WrapS,
						eTextureAddressMode a_WrapT, eTextureAddressMode a_WrapR, float a_fLod = 0, unsigned int a_fMaxAniso = 16,
						int a_lCompareFunc = 0, float *a_pBorderColor = NULL);

		//MATRIX METHODS
		void SetIdentityMatrix(eMatrix a_MtxType);

		void SetMatrix(eMatrix a_MtxType, const cMatrixf &a_mtxA);

		void Set2DMode();

		//CLEARING THE FRAMEBUFFER
		void ClearRenderTarget(iTexture *a_pRenderTarget, const cColor &a_Col);
		void ClearDepthTarget(iTexture *a_pDepthTarget, float a_fDepth = 0.0f);

		void ClearScreen();

		void SetClearColor(const cColor &a_Col);
		void SetClearDepth(float a_fDepth);
		void SetClearStencil(int a_lVal);

		void SetClearColorActive(bool a_bX);
		void SetClearDepthActive(bool a_bX);
		void SetClearStencilActive(bool a_bX);

		//RASTERIZER
		void SetCullActive(bool a_bX);
		void SetCullMode(eCullMode a_Mode);

		void SetFillMode(eFillMode a_Mode);

		//DEPTH
		void SetDepthTestActive(bool a_bX);
		void SetDepthTestFunc(eDepthTestFunc a_Func);
		void SetDepthState(iDepthStencilState *a_pState, unsigned int a_lStencilRef = 0);

		//ALPHA
		void SetAlphaTestActive(bool a_bX);
		void SetAlphaTestFunc(eAlphaTestFunc a_Func, float a_fRef);

		//STENCIL
		void SetStencilActive(bool a_bX);
		iDepthStencilState *CreateDepthStencilState(bool a_bDepthTest, bool a_bDepthWrite,
			eDepthTestFunc a_DepthFunc = eDepthTestFunc_LessOrEqual, bool a_bStencilTest = false,
			uint8 a_lStencilMask = 0xFF, eStencilFunc a_StencilFunc = eStencilFunc_Always, eStencilOp a_StencilFail = eStencilOp_Keep,
			eStencilOp a_DepthFail = eStencilOp_Keep, eStencilOp a_StencilPass = eStencilOp_Keep);

		//SCISSORS
		void SetScissorsActive(bool a_bX);
		void SetScissorRect(const cRectl &a_Rect);

		//TEXTURE
		void SetTexture(const tString &a_sName, iTexture *a_pTexture);

		//TEXTURE SAMPLER
		void SetTextureSamplerParams(const tString &a_sSamplerName, eTextureAddressMode a_WrapS, eTextureAddressMode a_WrapT, eTextureAddressMode a_WrapR,
			eTextureFilter a_Filter, float a_fAnisotropyDegree);
		void SetTextureAddressMode(eTextureAddressMode a_Mode);
		void SetTextureFilter(eTextureFilter a_Filter);

		//BLEND
		void SetBlendActive(bool a_bX);
		void SetBlendFunc(eBlendFunc a_SrcFactor, eBlendFunc a_DestFactor);

		//POLYGONS
		iVertexBuffer *CreateVertexBuffer(tVertexFlag a_Flags, eVertexBufferDrawType a_DrawType,
											eVertexBufferUsageType a_UsageType,
											int a_lReserveVtxSize = 0,int a_lReserveIdxSize=0);

		void ChangeVertexFormat(iVertexFormat *a_pVF);

		//VERTEX BATCHER
		void AddVertexToBatch(const cVertex *a_pVtx);
		void AddVertexToBatch(const cVertex *a_pVtx, const cVector3f *a_vTransform);

		void AddVertexToBatch_Size2D(const cVertex *a_pVtx, const cVector3f *a_vTransform, 
			const cColor *a_pCol, const float &m_fW, const float &m_fH);

		void AddVertexToBatch_Raw(	const cVector3f &a_vPos, const cColor &a_Color,
									const cVector3f &a_vTex);

		void AddIndexToBatch(int a_lIndex);

		void SetBatchTexture(iTexture *a_pTex);
		void FlushQuadBatch(bool a_bAutoClear);

		void ClearBatch();

		void DrawLine(const cVector3f &a_vBegin, const cVector3f &a_vEnd, cColor a_Col);
		void DrawBoxMaxMin(const cVector3f &a_vMax, const cVector3f &a_vMin, cColor a_Col);

		//SHADER
		void BindProgram(iGpuProgram *a_pShader, eGpuProgramType a_Type);

		bool SetShaderConstantInt(const tString &a_sName, const int a_lX);
		bool SetShaderConstantFloat(const tString &a_sName, const float a_fX);
		bool SetShaderConstantVec2f(const tString &a_sName, const cVector2f &a_vVec);
		bool SetProgramVec3f(const tString &a_sName, const cVector3f &a_vVec);
		bool SetProgramVec4f(const tString &a_sName, const cVector4f &a_vVec);
		bool SetShaderConstantColor4f(const tString &a_sName, const cColor &a_Col);
		bool SetShaderConstantMatrixf(const tString &a_sName, const cMatrixf &a_mMtx);
		bool SetShaderConstantArrayFloat(const tString &a_sName, const float *a_pX, int a_lCount);
		bool SetShaderConstantArrayVec2f(const tString &a_sName, const cVector2f *a_vVec, int a_lCount);
		bool SetShaderConstantArrayVec3f(const tString &a_sName, const cVector3f *a_vVec, int a_lCount);
		bool SetShaderConstantArrayColor4f(const tString &a_sName, const cColor *a_Col, int a_lCount);
		bool SetShaderConstantArrayMatrixf(const tString &a_sName, const cMatrixf *a_mMtx, int a_lCount);

		//PROGRAM BUFFERS
		iGpuProgramBuffer *CreateGpuProgramBuffer(eGpuProgramBufferType a_Type, iGpuProgramBufferConfig *a_pConfig, void *a_pData, bool a_bAutoUpdate = true);

		bool SetSamplerState(const tString &a_sName, iSamplerState *a_pSamplerState);

		//FRAMEBUFFER
		void ClearRenderTarget();
		void SetViewportSize(const cVector2l &a_vViewportSize);
		cVector2l GetViewportSize();
		//void SetClearColor(cColor a_Color);

		void ChangeRenderTargets(iTexture **a_pColorRT, const unsigned int a_lRTNum, iTexture *a_pDepthRT = NULL);
		void ChangeToMainFramebuffer(bool a_bChangeToDefaultDepth = true, iTexture *a_pCustomDepth = NULL);
		iRasterizerState *CreateRasterizerState(eCullMode a_Cull, eFillMode a_Fill = eFillMode_Solid, bool a_bMultiSampling = true,
			bool a_bScissor = false, float a_fDepthBias = 0.0f, float a_fSlopedDepthBias = 0.0f);
		void SetRasterizerState(iRasterizerState *a_pRasterState);

		iBlendState *CreateBlendState(eBlendFunc a_SrcFactorColor, eBlendFunc a_DestFactorColor, eBlendFunc a_SrcFactorAlpha, eBlendFunc a_DestFactorAlpha, eBlendOp a_BlendModeColor,
			eBlendOp a_BlendModeAlpha, int a_lMask = 0xF, bool a_bAlphaToCoverage = false);
		void SetBlendState(iBlendState *a_pBlendState, unsigned int a_lSampleMask = ~0);
		
		void FlushRenderTarget();

		void FlushRendering();
		void FlushTextures();
		void FlushConstants();

		void SwapBuffers();

		void Reset(tResetFlag  a_Flags);

		unsigned int GetDXTextureTargetEnum(eTextureTarget a_Type);

		//MY IMPL
		cGpuProgramParameterManager *GetGpuParameterManager();
		void EvaluateConstantBufferMappings(iGpuProgramBuffer *a_pBuffer);

		int GetWndHandle(){return *(int*)reinterpret_cast<void*>(m_hWnd);}

		iTexture *CreateRenderTarget(const cVector2l &a_vSize, const int a_lDepth, const int a_lMipMapCount, 
									const int a_lArraySize, const eFormat a_Fmt, const int a_lMultisampling, unsigned int a_Flags);

		ID3D11InputLayout *GetInputLayout(tVertexFlag a_VertexElements, iGpuProgram *a_pProgram);

	private:
		void GenerateInputLayout(tVertexFlag a_VertexElements, iGpuProgram *a_pProgram);

		void ConfigurePipeline();

		ID3D11Device *m_pDevice;
		ID3D11DeviceContext *m_pContext;
		IDXGISwapChain *m_pSwapChain;
		ID3D11RenderTargetView *m_pBackBufferRTV;
		ID3D11DepthStencilView *m_pDepthBufferDSV;

		cGpuProgramParameterManager *m_pGpuParameterManager;
		cDX11PipelineManager *m_pPipelineManager;

		D3D11_DEPTH_STENCIL_DESC m_DepthStencilDesc;

		std::hash_map<D3D11_RASTERIZER_DESC, ID3D11RasterizerState*, cRasterDescHashConfig> m_hshRasterizerStates;
		D3D11_RASTERIZER_DESC m_RasterierDesc;

		std::hash_map<D3D11_BLEND_DESC, ID3D11BlendState*, cBlendDescHashConfig> m_hshBlendStates;
		D3D11_BLEND_DESC m_BlendDesc;

		std::hash_map<D3D11_SAMPLER_DESC, ID3D11SamplerState*, cSamplerDescHashConfig> m_hshSamplerStatesByDesc;
		D3D11_SAMPLER_DESC m_TempSamplerDesc;

		cVector2l m_vViewportSize;
		D3D11_VIEWPORT m_Viewport;

		cGpuProgramParameterContainer m_ParameterContainer;

		DXGI_FORMAT m_BackBufferFormat;
		DXGI_FORMAT m_DepthBufferFormat;

		ID3D11RenderTargetView *m_pMainRenderTarget;
		ID3D11DepthStencilView *m_pMainDSV;
		ID3D11DepthStencilState *m_pMainDepthStencilState;
		ID3D11ShaderResourceView *m_pMainDepthStencilSRV;

		ID3D11DepthStencilState *m_pStencilDisabledState;

		ID3D11DepthStencilView *m_pShadowDSV;
		ID3D11ShaderResourceView *m_pShadowDepthSRV;
		ID3D11RenderTargetView *m_pShadowTV;
		ID3D11ShaderResourceView *m_pShadowShaderResourceView;

		ID3D11DepthStencilView *m_pMirrorTargetDV;

		ID3D11DepthStencilView *m_pPostPassDV;

		ID3D11RenderTargetView *m_pPostProcessTV;
		ID3D11ShaderResourceView *m_pPostProcessSRV;

		ID3D11BlendState *m_pSourceAlphaBlend;

		ID3D11RasterizerState *m_pMainRasterState;
		ID3D11RasterizerState *m_pShadowRasterState;
		ID3D11RasterizerState *m_pDBRasterState;
		ID3D11RasterizerState *m_pScissorState;

		cVector2l m_vScreenSize;
		cVector2f m_vVirtualSize;
		int m_lMultisampling;
		int m_lBpp;

		bool m_bCursorIsVisible;

#if defined(WIN32)
		HWND m_hWnd;
		HINSTANCE m_hKeytrapper;
		HINSTANCE m_hInstance;
#endif
		void _CreateWindow(int a_lWidth, int a_lHeight, bool a_bFullscreen, DXGI_OUTPUT_DESC *a_pOutDesc, tString a_sWindowCaption);

		DXGI_GAMMA_CONTROL m_vStartGammaRamp;
		float m_fGammaCorrection;

		std::map<tVertexFlag, std::map<iGpuProgram*, ID3D11InputLayout*>> m_mapInputLayouts;
		typedef std::hash_map<std::vector<D3D11_SIGNATURE_PARAMETER_DESC>, ID3D11InputLayout*, cInputSignatureVecHashConfig> tInputLayoutBySignatureVecMap;
		typedef tInputLayoutBySignatureVecMap::iterator tInputLayoutBySignatureVecMapIt;
		std::map<tVertexFlag, tInputLayoutBySignatureVecMap> m_mapInputLayoutsByInputSignature;

		//Vertex Array variables
		float *m_pVertexArray;
		unsigned int m_lVertexCount;
		unsigned int *m_pIndexArray;
		unsigned int m_lIndexCount;

		iTexture *m_pBatchTexture;
		iSamplerState *m_pBatchSampler;

		ID3D11Buffer *m_pBatchBuffer;
		ID3D11Buffer *m_pIndexBatchBuffer;
		iRasterizerState *m_pBatchRS;

		ID3D11Buffer *m_pLineBuffer;
		iGpuProgram *m_pLinePogram;
		iVertexFormat *m_pLineFormat;

		unsigned int m_lBatchStride;

		unsigned int m_lVertexBatchOffset;
		unsigned int m_lIndexBatchOffset;

		float *m_pTexCoordArraw[MAX_TEXTUREUNITS];
		bool m_bTexCoordArrawActive[MAX_TEXTUREUNITS];
		unsigned int m_lTexCoordArrawActive[MAX_TEXTUREUNITS];

		unsigned int m_lBatchArraySize;

		//Clearing
		cColor m_ClearColor;
		float m_fClearDepth;
		int m_lClearStencil;

		bool m_bClearColor;
		bool m_bClearDepth;
		bool m_bClearStencil;

		cColor m_vScaleBias2D;

		iTexture *m_pRenderTarget;

		iDepthStencilState *m_pCurrentDepthState, *m_pSelectedDepthState;
		unsigned int m_lCurrentStencilRef, m_lSelectedStencilRef;
		iBlendState *m_pCurrentBlendState, *m_pSelectedBlendState;
		unsigned int m_lCurrentSampleMask, m_lSelectedSampleMask;
		iRasterizerState *m_pCurrentRasterizerState, *m_pSelectedRasterizerState;
		
		//Depth Helper
		D3D11_COMPARISON_FUNC GetDX11DepthTestFuncEnum(eDepthTestFunc a_Type);
		//D3DCMPFUNC GetDXDepthTestFuncEnum(eDepthTestFunc a_Type);

		//Stencil Helper
		D3D11_COMPARISON_FUNC GetDXStencilFuncEnum(eStencilFunc a_Type);
		D3D11_STENCIL_OP GetDXStencilOpEnum(eStencilOp a_Type);

		//Matrix helper
		void SetMatrixMode(eMatrix m_Type);

		//Blend helper
		D3D11_BLEND GetDX11BlendFuncEnum(eBlendFunc a_Type);
		D3D11_BLEND_OP GetDXBlendOpEnum(eBlendOp a_Type);

		// Rasterizer helper
		D3D11_CULL_MODE GetDX11CullEnum(eCullMode a_Mode);
		D3D11_FILL_MODE	GetDX11FillEnum(eFillMode a_Mode);

		bool SetupDX(bool a_bFullscreen);

		void ApplySamplerStates();

		void ChangeDepthState(iDepthStencilState *a_pState, unsigned int a_lStencilRef = 0);
		void ChangeRasterizerState(iRasterizerState *a_pRasterState);
		void ChangeBlendState(iBlendState *a_pBlendState, unsigned int a_lSamlpeMask = ~0);

		void ChangeViewport();

		void CreatePlainShadersAndFormats();

		iGpuProgram *m_pPlainTexProgram;
		iGpuProgram *m_pPlainNoTexProgram;
		iVertexFormat *m_pTexVertexFormat;
		

		bool CreateBuffers();
		void RegisterWndClass();
		//bool SetUpSwapChainRenderTarView(bool a_bFullscreen, unsigned int a_lNum, unsigned int a_lDen);
		bool CreateDepthStencilState();
		bool SetUpRenderTargetView(ID3D10RenderTargetView **a_ppRenderTargetView, ID3D10ShaderResourceView **a_ppShaderResourceView);
		bool SetDepthStencil(ID3D10DepthStencilView **a_ppDepthView);
		bool SetDepthStencilSRV(ID3D10DepthStencilView **a_ppDepthView, ID3D10ShaderResourceView **a_ppShaderResourceView);
		void SetViewPort();
		void SetRasterState();

		//New awesome stuff

		//Sampler Helper
		D3D11_FILTER GetDX11TextureFilterEnum(eTextureFilter a_Filter);
		D3D11_TEXTURE_ADDRESS_MODE GetDX11TextureWrapEnum(eTextureAddressMode a_Wrap);
	};
};
#endif