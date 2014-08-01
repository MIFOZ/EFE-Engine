#ifndef EFE_LOWLEVELGRAPHICS_H
#define EFE_LOWLEVELGRAPHICS_H

#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"

#include "graphics/Texture.h"
#include "graphics/GPUProgram.h"
#include "graphics/VertexBuffer.h"
#include "graphics/GpuProgramBuffer.h"

namespace efe
{
	enum eBlendFunc
	{
		eBlendFunc_Zero,
		eBlendFunc_One,
		eBlendFunc_SrcColor,
		eBlendFunc_OneMinusSrcColor,
		eBlendFunc_DestColor,
		eBlendFunc_OneMinusDestColor,
		eBlendFunc_SrcAlpha,
		eBlendFunc_OneMinusSrcAlpha,
		eBlendFunc_DestAlpha,
		eBlendFunc_OneMinusDestAlpha,
		eBlendFunc_SrcAlphaSaturate,
		eBlendFunc_BlendFactor,
		eBlendFunc_OneMinusBlendFactor,
		eBlendFunc_Src1Color,
		eBlendFunc_OneMinusSrc1Color,
		eBlendFunc_Src1Alpha,
		eBlendFunc_OneMinusSrc1Alpha,
		eBlendFunc_LastEnum
	};

	enum eBlendOp
	{
		eBlendOp_Add,
		eBlendOp_Subtract,
		eBlendOp_RevSubtract,
		eBlendOp_Max,
		eBlendOp_Min,
		eBlendOp_LastEnum
	};

	enum eStencilFunc
	{
		eStencilFunc_Never,
		eStencilFunc_Less,
		eStencilFunc_LessOrEqual,
		eStencilFunc_Greater,
		eStencilFunc_GreaterOrEqual,
		eStencilFunc_Equal,
		eStencilFunc_NotEqual,
		eStencilFunc_Always,
		eStencilFunc_LastEnum
	};

	enum eDepthTestFunc
	{
		eDepthTestFunc_Never,
		eDepthTestFunc_Less,
		eDepthTestFunc_LessOrEqual,
		eDepthTestFunc_Greater,
		eDepthTestFunc_GreaterOrEqual,
		eDepthTestFunc_Equal,
		eDepthTestFunc_NotEqual,
		eDepthTestFunc_Always,
		eDepthTestFunc_LastEnum
	};

	enum eAlphaTestFunc
	{
		eAlphaTestFunc_Never,
		eAlphaTestFunc_Less,
		eAlphaTestFunc_LessOrEqual,
		eAlphaTestFunc_Greater,
		eAlphaTestFunc_GreaterOrEqual,
		eAlphaTestFunc_Equal,
		eAlphaTestFunc_NotEqual,
		eAlphaTestFunc_Always,
		eAlphaTestFunc_LastEnum
	};

	enum eStencilOp
	{
		eStencilOp_Keep,
		eStencilOp_Zero,
		eStencilOp_Replace,
		eStencilOp_Increment,
		eStencilOp_Decrement,
		eStencilOp_Invert,
		eStencilOp_IncrementSat,
		eStencilOp_DecrementSat,
		eStencilOp_LastEnum
	};

	/*enum eCullMode
	{
	eCullMode_Clockwise,
	eCullMode_CounterClockwise,
	eCullMode_LastEnum
	};*/

	enum eFilter
	{
		eFilter_Nearest,
		eFilter_Linear,
		eFilter_Bilinear,
		eFilter_Trilinear,
		eFilter_BilinearAniso,
		eFilter_TrilinearAniso,
	};

	enum eCullMode
	{
		eCullMode_None,
		eCullMode_Back,
		eCullMode_Front
	};

	enum eFillMode
	{
		eFillMode_Solid,
		eFillMode_Wireframe
	};

	typedef tFlag tVtxBatchFlag;

#define eVtxBatchFlag_Normal		(0x00000001)
#define eVtxBatchFlag_Position		(0x00000002)
#define eVtxBatchFlag_Color0		(0x00000004)
#define eVtxBatchFlag_Texture0		(0x00000008)
#define eVtxBatchFlag_Texture1		(0x00000010)
#define eVtxBatchFlag_Texture2		(0x00000020)

	typedef tFlag tResetFlag;

#define eResetFlag_All				(0x0000FFFF)
#define eResetFlag_Shader			(0x00000001)
#define eResetFlag_VertexFormat		(0x00000002)
#define eResetFlag_VertexBuffer		(0x00000004)
#define eResetFlag_IndexBuffer		(0x00000008)
#define eResetFlag_DepthState		(0x00000010)
#define eResetFlag_BlendState		(0x00000020)
#define eResetFlag_RasterizerState	(0x00000020)

#define kMaxClipPlanes (6)

	class iFontData;
	class iOcclusionQuery;
	class cGpuProgramParameterManager;

	struct cSampler;

	class iRasterizerState
	{
	public:
		virtual ~iRasterizerState(){}
	};

	class iBlendState
	{
	public:
		virtual ~iBlendState(){}
	};

	class iSamplerState
	{
	public:
		virtual ~iSamplerState(){}
	};

	class iDepthStencilState
	{
	public:
		virtual ~iDepthStencilState(){}
	};

	class iLowLevelGraphics
	{
	public:
		virtual ~iLowLevelGraphics(){}

		virtual bool Init(int a_iWidth, int a_iHeight, int a_iBpp, bool a_bFullscreen, int a_iMultisampling,
			const tString &a_sWindowCaption)=0;

		virtual void _ShowCursor(bool a_bX)=0;

		virtual int GetMultisampling()=0;

		virtual cVector2f GetVirtualSize()=0;
		virtual cVector2f GetScreenSize()=0;

		virtual void SetVirtualSize(cVector2f a_vSize)=0;

		virtual void SetVsyncActive(bool a_bX)=0;

		virtual void SetMultisamplingActive(bool a_bX)=0;

		virtual iBitmap2D *CreateBitmap2D(const cVector2l &a_vSize, eFormat a_Format)=0;
		virtual iFontData *CreateFontData(const tString &a_sName)=0;

		//virtual cGpuProgramParameterManager *

		//VERTEX BUFFER
		virtual void SetVertexBuffer(iVertexBuffer *a_pBuffer)=0;
		//virtual void GenerateInputLayout()

		//TEXTURE
		virtual iTexture *CreateTexture(bool a_bUseMipMaps, eTextureType a_Type, eTextureTarget a_Target)=0;
		virtual iTexture *CreateTexture(const tString &a_sName, bool a_bUseMipMaps, eTextureType a_Type, eTextureTarget a_Target)=0;
		virtual iTexture *CreateTexture(iBitmap2D *a_pBmp, bool a_bUseMipMaps, eTextureType a_Type,
			eTextureTarget a_Target)=0;
		virtual iTexture *CreateTexture(const cVector2l &a_vSize, eFormat a_Format, cColor a_FillCol,
			bool a_bUseMipMaps, eTextureType a_Type,  eTextureTarget a_Target)=0;

		virtual iTexture *CreateRenderTarget(const cVector2l &a_vSize, const int a_lDepth, const int a_lMipMapCount, const int a_lArraySize,
			const eFormat a_Format, const int a_lMultisampling = 1, bool a_bSRGB = false, eTextureTarget a_Target = eTextureTarget_2D)=0;

		virtual iTexture *CreateRenderDepth(const cVector2l &a_vSize, const int a_lArraySize, const eFormat a_Format, bool a_bSample = false, int a_lMultisampling = 1, eTextureTarget a_Target = eTextureTarget_2D)=0;


		virtual iSamplerState *CreateSamplerState(eTextureFilter a_Filter, eTextureAddressMode a_WrapS,
			eTextureAddressMode a_WrapT, eTextureAddressMode a_WrapR, float a_fLod = 0, unsigned int a_fMaxAniso = 16,
			int a_lCompareFunc = 0, float *a_pBorderColor = NULL)=0;

		//MATRIX METHODS
		virtual void SetIdentityMatrix(eMatrix a_MtxType)=0;

		virtual void SetMatrix(eMatrix a_MtxType, const cMatrixf &a_mtxA)=0;

		virtual void Set2DMode()=0; 

		//CLEARING THE FRAMEBUFFER
		virtual void ClearRenderTarget(iTexture *a_pRenderTarget, const cColor &a_Col)=0;
		virtual void ClearDepthTarget(iTexture *a_pDepthTarget, float a_fDepth = 0.0f)=0;

		virtual void ClearScreen()=0;

		virtual void SetClearColor(const cColor &a_Col)=0;
		virtual void SetClearDepth(float a_fDepth)=0;
		virtual void SetClearStencil(int a_lVal)=0;

		virtual void SetClearColorActive(bool a_bX)=0;
		virtual void SetClearDepthActive(bool a_bX)=0;
		virtual void SetClearStencilActive(bool a_bX)=0;

		//RASTERIZER
		virtual void SetCullActive(bool a_bX)=0;
		virtual void SetCullMode(eCullMode a_Mode)=0;

		virtual void SetFillMode(eFillMode a_Mode)=0;

		//DEPTH
		virtual void SetDepthTestActive(bool a_bX)=0;
		virtual void SetDepthTestFunc(eDepthTestFunc a_Func)=0;
		virtual void SetDepthState(iDepthStencilState *a_pState, unsigned int a_lStencilRef = 0)=0;

		//ALPHA
		virtual void SetAlphaTestActive(bool a_bX)=0;
		virtual void SetAlphaTestFunc(eAlphaTestFunc a_Func, float a_fRef)=0;

		//STENCIL
		virtual void SetStencilActive(bool a_bX)=0;
		virtual iDepthStencilState *CreateDepthStencilState(bool a_bDepthTest, bool a_bDepthWrite,
			eDepthTestFunc a_DepthFunc = eDepthTestFunc_LessOrEqual, bool a_bStencilTest = false,
			uint8 a_lStencilMask = 0xFF, eStencilFunc a_StencilFunc = eStencilFunc_Always, eStencilOp a_StencilFail = eStencilOp_Keep,
			eStencilOp a_DepthFail = eStencilOp_Keep, eStencilOp a_StencilPass = eStencilOp_Keep)=0;



		//SCISSORS
		virtual void SetScissorsActive(bool a_bX)=0;
		virtual void SetScissorRect(const cRectl &a_Rect)=0;

		//TEXTURE
		virtual void SetTexture(const tString &a_sName, iTexture *a_pTexture)=0;

		//TEXTURE SAMPLER
		virtual void SetTextureSamplerParams(const tString &a_sSamplerName, eTextureAddressMode a_WrapS, eTextureAddressMode a_WrapT, eTextureAddressMode a_WrapR,
			eTextureFilter a_Filter, float a_fAnisotropyDegree)=0;
		//virtual void SetTextureAddressMode(const tString &a_sSamplerName, eTextureAddressMode a_Mode)=0;
		//virtual void SetTextureFilter(const tString &a_sSamplerName, eTextureFilter a_Filter)=0;

		//BLEND
		virtual void SetBlendActive(bool a_bX)=0;
		virtual void SetBlendFunc(eBlendFunc a_SrcFactor, eBlendFunc a_DestFactor)=0;

		//POLYGONS
		virtual iVertexBuffer *CreateVertexBuffer(tVertexFlag a_Flags, eVertexBufferDrawType a_DrawType,
			eVertexBufferUsageType a_UsageType,
			int alReserveVtxSize=0,int alReserveIdxSize=0)=0;

		//VERTEX BATCHER
		virtual void AddVertexToBatch(const cVertex *a_pVtx)=0;
		virtual void AddVertexToBatch(const cVertex *a_pVtx, const cVector3f *a_vTransform)=0;

		virtual void AddVertexToBatch_Size2D(const cVertex *a_pVtx, const cVector3f *a_vTransform, 
			const cColor *a_pCol, const float &m_fW, const float &m_fH)=0;

		virtual void AddVertexToBatch_Raw(	const cVector3f &a_vPos, const cColor &a_Color,
			const cVector3f &a_vTex)=0;

		virtual void AddIndexToBatch(int a_lIndex)=0;

		virtual void SetBatchTexture(iTexture *a_pTex)=0;
		virtual void FlushQuadBatch(bool a_bAutoClear = true)=0;

		virtual void ClearBatch()=0;

		//some primitive:
		virtual void DrawLine(const cVector3f &a_vBegin, const cVector3f &a_vEnd, cColor a_Col)=0;
		virtual void DrawBoxMaxMin(const cVector3f &a_vMax, const cVector3f &a_vMin, cColor a_Col)=0;

		//PROGRAMS
		virtual iGpuProgram *CreateGpuProgram(const tString &a_sName, eGpuProgramType a_Type)=0;

		virtual void BindProgram(iGpuProgram *a_pProgram, eGpuProgramType a_Type)=0;

		virtual bool SetShaderConstantInt(const tString &a_sName, const int a_lX)=0;
		virtual bool SetShaderConstantFloat(const tString &a_sName, const float a_fX)=0;
		virtual bool SetShaderConstantVec2f(const tString &a_sName, const cVector2f &a_vVec)=0;
		virtual bool SetProgramVec3f(const tString &a_sName, const cVector3f &a_vVec)=0;
		virtual bool SetShaderConstantColor4f(const tString &a_sName, const cColor &a_Col)=0;
		virtual bool SetShaderConstantMatrixf(const tString &a_sName, const cMatrixf &a_mMtx)=0;
		virtual bool SetShaderConstantArrayFloat(const tString &a_sName, const float *a_pX, int a_lCount)=0;
		virtual bool SetShaderConstantArrayVec2f(const tString &a_sName, const cVector2f *a_vVec, int a_lCount)=0;
		virtual bool SetShaderConstantArrayVec3f(const tString &a_sName, const cVector3f *a_vVec, int a_lCount)=0;
		virtual bool SetShaderConstantArrayColor4f(const tString &a_sName, const cColor *a_Col, int a_lCount)=0;
		virtual bool SetShaderConstantArrayMatrixf(const tString &a_sName, const cMatrixf *a_mMtx, int a_lCount)=0;

		virtual bool SetSamplerState(const tString &a_sName, iSamplerState *a_pSamplerState)=0;

		//BUFFERS
		virtual iGpuProgramBuffer *CreateGpuProgramBuffer(eGpuProgramBufferType a_Type, iGpuProgramBufferConfig *a_pConfig, void *a_pData, bool a_bAutoUpdate = true)=0;

		//FRAME BUFFER
		virtual void ClearRenderTarget()=0;
		virtual void SetViewportSize(const cVector2l &a_vViewportSize)=0;
		virtual cVector2l GetViewportSize()=0;
		//virtual void SetClearColor(cColor a_Color)=0;

		void ChangeRenderTarget(iTexture *a_pColorRT, iTexture *a_pDepthRT = NULL)
		{
			ChangeRenderTargets(&a_pColorRT, 1, a_pDepthRT);
		}
		virtual void ChangeRenderTargets(iTexture **a_pColorRT, const unsigned int a_lRTNum, iTexture *a_pDepthRT = NULL)=0;
		virtual void ChangeToMainFramebuffer(bool a_bChangeToDefaultDepth, iTexture *a_pCustomDepth = NULL)=0;
		virtual iRasterizerState *CreateRasterizerState(eCullMode a_Cull, eFillMode a_Fill = eFillMode_Solid, bool a_bMultiSampling = true,
			bool a_bScissor = false, float a_fDepthBias = 0.0f, float a_fSlopedDepthBias = 0.0f)=0;
		virtual void SetRasterizerState(iRasterizerState *a_pRasterState)=0;

		virtual iBlendState *CreateBlendState(eBlendFunc a_SrcFactorColor, eBlendFunc a_DestFactorColor, eBlendFunc a_SrcFactorAlpha, eBlendFunc a_DestFactorAlpha, eBlendOp a_BlendModeColor,
			eBlendOp a_BlendModeAlpha, int a_lMask = 0xF, bool a_bAlphaToCoverage = false)=0;
		iBlendState *CreateBlendState(eBlendFunc a_SrcFactor, eBlendFunc a_DestFactor, eBlendOp a_BlendMode = eBlendOp_Add, int a_lMask = 0xF, bool a_bAlphaToCoverage = false)
		{
			return CreateBlendState(a_SrcFactor, a_DestFactor, a_SrcFactor, a_DestFactor, a_BlendMode, a_BlendMode, a_lMask, a_bAlphaToCoverage);
		}
		virtual void SetBlendState(iBlendState *a_pBlendState, unsigned int a_lSampleMask = ~0)=0;
		virtual void FlushRenderTarget()=0;

		virtual void FlushRendering()=0;
		virtual void FlushTextures()=0;
		virtual void FlushConstants()=0;

		virtual void SwapBuffers()=0;

		virtual void Reset(tResetFlag  a_Flags = eResetFlag_All)=0;

		//MY IMPL
		virtual cGpuProgramParameterManager *GetGpuParameterManager()=0;
		virtual void EvaluateConstantBufferMappings(iGpuProgramBuffer *a_pBuffer)=0;

		virtual int GetWndHandle()=0;

		static cMatrixf s_mtxView;
		static cMatrixf s_mtxProjection;
	};
};

#endif