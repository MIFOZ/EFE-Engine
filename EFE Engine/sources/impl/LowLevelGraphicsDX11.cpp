#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")

#include <assert.h>
#include <stdlib.h>

#include "graphics/FontData.h"
#include "graphics/GpuProgramParameterManager.h"
#include "impl/LowLevelGraphicsDX11.h"
#include "impl/RenderPipelineSystem/DX11PipelineManager.h"
#include "impl/DX11Bitmap2D.h"
#include "impl/DXFontData.h"
#include "impl/DX11Texture.h"
#include "impl/DX11Program.h"
#include "system/LowLevelSystem.h"
#include "impl/DX11VertexBuffer.h"
#include "impl/DX11ProgramBuffer.h"

#include "DXGI.h"

namespace efe
{
	#define WM_ACTIVE		0x8001
	#define WM_INACTIVE		0x8002
	LRESULT CALLBACK WindowEvent(HWND, UINT, WPARAM, LPARAM);

	bool FillSRV(ID3D11ShaderResourceView **a_pDest, int &a_lMin, int &a_lMax, iTexture *a_pSelectedTextures[], iTexture *a_pCurrentTexture[])
	{
		a_lMin = 0;
		do
		{
			if (a_pSelectedTextures[a_lMin] != a_pCurrentTexture[a_lMin])
			{
				a_lMax = MAX_TEXTUREUNITS;
				do
				{
					a_lMax--;
				}
				while(a_pSelectedTextures[a_lMax] == a_pCurrentTexture[a_lMax]);

				for (int i = a_lMin;i <= a_lMax; i++)
				{
					if (a_pSelectedTextures[i] != NULL)
					{
						cDX11Texture *pTex = static_cast<cDX11Texture *>(a_pSelectedTextures[i]);
						*a_pDest++ = pTex->GetSRV();
					}
					else
						*a_pDest++ = NULL;

					a_pSelectedTextures[i] = a_pCurrentTexture[i];
				}

				return true;
			}
			a_lMin++;
		}
		while(a_lMin < MAX_TEXTUREUNITS);

		return false;
	}

	bool FillSS(iSamplerState **a_pDest, int &a_lMin, int &a_lMax, iSamplerState *a_pSelectedSamplerStates[], iSamplerState *a_pCurrentSamplerStates[])
	{
		a_lMin = 0;
		do
		{
			if (a_pSelectedSamplerStates[a_lMin] != a_pCurrentSamplerStates[a_lMin])
			{
				a_lMax = MAX_SAMPLERSTATES;
				do
				{
					a_lMax--;
				}
				while(a_pSelectedSamplerStates[a_lMax] == a_pCurrentSamplerStates[a_lMax]);

				for (int i = a_lMin;i <= a_lMax; i++)
				{
					if (a_pSelectedSamplerStates[i] != NULL)
						*a_pDest++ = a_pSelectedSamplerStates[i];
					else
						*a_pDest++ = NULL;

					a_pCurrentSamplerStates[i] = a_pSelectedSamplerStates[i];
				}

				return true;
			}
			a_lMin++;
		}
		while(a_lMin < MAX_SAMPLERSTATES);

		return false;
	}

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMatrixf iLowLevelGraphics::s_mtxView = cMatrixf::Identity;
	cMatrixf iLowLevelGraphics::s_mtxProjection = cMatrixf::Identity;

	cLowLevelGraphicsDX11::cLowLevelGraphicsDX11(int a_hWindowHanlde)
		: m_ParameterContainer(NULL)
	{
		m_hWnd = (HWND)a_hWindowHanlde;

		m_lBatchArraySize = 2000;
		m_lVertexCount = 0;
		m_lIndexCount = 0;
		m_lMultisampling = 0;
		m_lVertexBatchOffset = 0;
		m_lIndexBatchOffset = 0;

		m_bCursorIsVisible = true;

		m_fClearDepth = 0;

		m_vVirtualSize.x = 800;
		m_vVirtualSize.y = 600;

		m_lBatchStride = 13;

		m_pVertexArray = (float*)efeMalloc(sizeof(float) * m_lBatchStride * m_lBatchArraySize);
		m_pIndexArray = (unsigned int *)efeMalloc(sizeof(unsigned int) * m_lBatchArraySize);

		m_pGpuParameterManager = efeNew(cGpuProgramParameterManager, ());
		m_ParameterContainer.SetManager(m_pGpuParameterManager);

		m_pPipelineManager = efeNew(cDX11PipelineManager, (this, m_pContext));
	}

	cLowLevelGraphicsDX11::~cLowLevelGraphicsDX11()
	{
		efeFree(m_pVertexArray);
		efeFree(m_pIndexArray);

		if (m_pBatchBuffer)m_pBatchBuffer->Release();
		if (m_pIndexBatchBuffer)m_pIndexBatchBuffer->Release();
		
		efeDelete(m_pBatchRS);

		m_pSwapChain->Release();
		m_pDevice->Release();
		m_pContext->Release();
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

    bool cLowLevelGraphicsDX11::Init(int a_lWidth, int a_lHeight, int a_iBpp, bool a_bFullscreen, int a_lMultisampling,
			const tString &a_sWindowCaption)
	{
        m_vScreenSize = cVector2l(a_lWidth, a_lHeight);
		m_lMultisampling = a_lMultisampling;

		m_pBackBufferRTV = NULL;
		m_pDepthBufferDSV = NULL;

		m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		m_DepthBufferFormat  = DXGI_FORMAT_UNKNOWN;

		m_lBpp = a_iBpp;

		switch(m_lBpp)
		{
		case 16:
			m_DepthBufferFormat = DXGI_FORMAT_D16_UNORM;
			break;
		case 24:
			m_DepthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		case 32:
			m_DepthBufferFormat = DXGI_FORMAT_D32_FLOAT;
			break;
		}
		//m_bCursorIsVisible = true;

		//_ShowCursor(false);
		
		//Factory stuff
		IDXGIFactory *pFactory;
		if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory)))
			return false;

		IDXGIAdapter *pAdapter;
		if (pFactory->EnumAdapters(0, &pAdapter)==DXGI_ERROR_NOT_FOUND)
			return false;

		IDXGIOutput *pAdapterOutput;
		if (pAdapter->EnumOutputs(0, &pAdapterOutput)==DXGI_ERROR_NOT_FOUND)
			return false;

		DXGI_OUTPUT_DESC descOut;
		pAdapterOutput->GetDesc(&descOut);

		DXGI_RATIONAL FullScreenRefresh;
		int lFSRefresh = 60;
		FullScreenRefresh.Numerator = lFSRefresh;
		FullScreenRefresh.Denominator = 1;

		unsigned int lNumModes = 0;
		pAdapterOutput->GetDisplayModeList(m_BackBufferFormat, DXGI_ENUM_MODES_INTERLACED, &lNumModes, NULL);
		DXGI_MODE_DESC *pDisplayModeList = efeNewArray(DXGI_MODE_DESC, lNumModes);
		pAdapterOutput->GetDisplayModeList(m_BackBufferFormat, DXGI_ENUM_MODES_INTERLACED, &lNumModes, pDisplayModeList);

		pAdapterOutput->Release();
		pAdapterOutput = 0;

		UINT lNumerator=0, lDenominator=0;
		for (int i=0; i < lNumModes; i++)
		{
			if (pDisplayModeList[i].Width == (unsigned int)m_vScreenSize.x)
				if (pDisplayModeList[i].Height == (unsigned int)m_vScreenSize.y)
				{
					FullScreenRefresh.Numerator = pDisplayModeList[i].RefreshRate.Numerator;
					FullScreenRefresh.Denominator = pDisplayModeList[i].RefreshRate.Denominator;
				}
		}

		efeDeleteArray(pDisplayModeList);

        _CreateWindow(a_lWidth, a_lHeight, a_bFullscreen, &descOut, a_sWindowCaption);

		//RECT rect;
		//GetClientRect(m_hWnd, &rect);

		DXGI_SWAP_CHAIN_DESC descSC;
		memset(&descSC, 0, sizeof(descSC));
		descSC.BufferDesc.Width = m_vScreenSize.x;
		descSC.BufferDesc.Height = m_vScreenSize.y;
		descSC.BufferDesc.Format = m_BackBufferFormat;
		descSC.BufferDesc.RefreshRate = FullScreenRefresh;
		descSC.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		descSC.BufferCount = 1;
		descSC.OutputWindow = m_hWnd;
		descSC.Windowed = (BOOL)(!a_bFullscreen);
		descSC.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		descSC.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		DWORD flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

#if defined(_DEBUG) | defined(DEBUG)
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL pFeatureLvl[] = {D3D_FEATURE_LEVEL_11_0};
		D3D_FEATURE_LEVEL pRetFeature;

		if (FAILED(D3D11CreateDevice(pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, flags, pFeatureLvl, 1, D3D11_SDK_VERSION,
										&m_pDevice, &pRetFeature, &m_pContext)))
			return false;

		pAdapter->Release();

		m_pPipelineManager->SetDeviceContext(m_pContext);

		while (a_lMultisampling > 1)
		{
			UINT lQuality;
			if (SUCCEEDED(m_pDevice->CheckMultisampleQualityLevels(m_BackBufferFormat, a_lMultisampling, &lQuality)) && lQuality > 0)
				break;
			else
				a_lMultisampling -= 2;
		}

		descSC.SampleDesc.Count = a_lMultisampling;
		descSC.SampleDesc.Quality = 0;

		if (FAILED(pFactory->CreateSwapChain(m_pDevice, &descSC, &m_pSwapChain)))
			return false;

		pFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
		
		pFactory->Release();

		if (CreateBuffers()==false)
			return false;

		CreatePlainShadersAndFormats();

		//Batch buffer
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = m_lBatchArraySize*m_lBatchStride*sizeof(float);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.MiscFlags = 0;
		desc.StructureByteStride = m_lBatchStride*sizeof(float);
		m_pDevice->CreateBuffer(&desc, NULL, &m_pBatchBuffer);

		desc.ByteWidth = m_lBatchArraySize*m_lBatchStride*sizeof(unsigned int);
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.StructureByteStride = sizeof(unsigned int);
		m_pDevice->CreateBuffer(&desc, NULL, &m_pIndexBatchBuffer);

		desc.ByteWidth = sizeof(cVector3f) * 24;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.StructureByteStride = sizeof(cVector3f);
		m_pDevice->CreateBuffer(&desc, NULL, &m_pLineBuffer);

		D3D11_VIEWPORT Viewport;
		Viewport.Width = GetScreenSize().x;
		Viewport.Height = GetScreenSize().y;
		Viewport.TopLeftX = Viewport.TopLeftY = 0.0f;
		Viewport.MaxDepth = 1.0f;
		Viewport.MinDepth = 0.0f;
		m_pContext->RSSetViewports(1, &Viewport);
		m_pPipelineManager->m_RasterizerStage.m_DesiredState.m_ViewportCount.SetState(1);
		m_pPipelineManager->m_RasterizerStage.m_DesiredState.m_Viewports.SetState(0, cRectl(0, 0, GetScreenSize().x, GetScreenSize().y));
		m_pBatchRS = CreateRasterizerState(eCullMode_None, eFillMode_Solid, false);

		m_pBatchSampler = CreateSamplerState(eTextureFilter_Linear, eTextureAddressMode_Border,eTextureAddressMode_Border,
			eTextureAddressMode_Border);

		return true;
	}

	iBitmap2D *cLowLevelGraphicsDX11::CreateBitmap2D(const cVector2l &a_vSize, eFormat a_Format)
	{
		cDX11Bitmap2D *pBmp = efeNew(cDX11Bitmap2D, ());
		pBmp->Create(a_vSize,a_Format);

		return pBmp;
	}

	//--------------------------------------------------------------

	iFontData *cLowLevelGraphicsDX11::CreateFontData(const tString &a_sName)
	{
		return efeNew(cDXFontData, (a_sName, this));
	}

	//--------------------------------------------------------------

	/*iBitmap2D *cLowLevelGraphicsDX::CreateBitmapFromSurface(cSurface *a_pSurface, const tString &a_sType)
	{
		cDXBitmap2D *pBmp = NULL;efeNew(cDXBitmap2D,(a_pSurface, m_pPixelFormat, a_sType));

		pBmp->m_sType = a_sType;

		return pBmp;
	}*/

	//--------------------------------------------------------------

	iGpuProgram *cLowLevelGraphicsDX11::CreateGpuProgram(const tString &a_sName, eGpuProgramType a_Type)
	{
		return efeNew(cDX11Program, (a_sName, this, m_pDevice, m_pContext, m_pGpuParameterManager, a_Type));
	}

	//--------------------------------------------------------------

	iSamplerState *cLowLevelGraphicsDX11::CreateSamplerState(eTextureFilter a_Filter, eTextureAddressMode a_WrapS,
						eTextureAddressMode a_WrapT, eTextureAddressMode a_WrapR, float a_fLod, unsigned int a_fMaxAniso,
						int a_lCompareFunc, float *a_pBorderColor)
	{
		D3D11_SAMPLER_DESC desc;
		desc.Filter = GetDX11TextureFilterEnum(a_Filter);
		if (a_lCompareFunc)
			desc.Filter = (D3D11_FILTER)(desc.Filter | 0x80);

		desc.ComparisonFunc = (D3D11_COMPARISON_FUNC)a_lCompareFunc;
		desc.AddressU = GetDX11TextureWrapEnum(a_WrapS);
		desc.AddressV = GetDX11TextureWrapEnum(a_WrapT);
		desc.AddressW = GetDX11TextureWrapEnum(a_WrapR);
		desc.MipLODBias = a_fLod;
		desc.MaxAnisotropy = HasAniso(a_Filter) ? a_fMaxAniso : 1;
		if (a_pBorderColor)
		{
			desc.BorderColor[0] = a_pBorderColor[0];
			desc.BorderColor[1] = a_pBorderColor[1];
			desc.BorderColor[2] = a_pBorderColor[2];
			desc.BorderColor[3] = a_pBorderColor[3];
		}
		else
		{
			desc.BorderColor[0] = 0;
			desc.BorderColor[1] = 0;
			desc.BorderColor[2] = 0;
			desc.BorderColor[3] = 0;
		}

		desc.MinLOD = 0;
		desc.MaxLOD = HasMipMaps(a_Filter)? D3D11_FLOAT32_MAX : 0;

		cDXSamplerState *pSS = efeNew(cDXSamplerState, ());

		if (FAILED(m_pDevice->CreateSamplerState(&desc, &pSS->m_pSamplerState)))
		{
			Error("Could not create sampler state\n");
			return NULL;
		}

		return pSS;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetVertexBuffer(iVertexBuffer *a_pBuffer)
	{
	}

	//--------------------------------------------------------------

	iTexture *cLowLevelGraphicsDX11::CreateTexture(bool a_bUseMipMaps, eTextureType a_Type, eTextureTarget a_Target)
	{
		return efeNew(cDX11Texture,("", this, m_pDevice, m_pContext, a_Type, a_bUseMipMaps, a_Target));
	}

	iTexture *cLowLevelGraphicsDX11::CreateTexture(const tString &a_sName, bool a_bUseMipMaps, eTextureType a_Type, eTextureTarget a_Target)
	{
		return efeNew(cDX11Texture,(a_sName, this, m_pDevice, m_pContext, a_Type, a_bUseMipMaps, a_Target));
	}

	iTexture *cLowLevelGraphicsDX11::CreateTexture(iBitmap2D *a_pBmp, bool a_bUseMipMaps, eTextureType a_Type,
										eTextureTarget a_Target)
	{
		cDX11Texture *pTex = efeNew(cDX11Texture,("", this, m_pDevice, m_pContext, a_Type, a_bUseMipMaps, a_Target));
		pTex->CreateFromBitmap(a_pBmp);
		return pTex;
	}

	iTexture *cLowLevelGraphicsDX11::CreateTexture(const cVector2l &a_vSize, eFormat a_Format, cColor a_FillCol,
										bool a_bUseMipMaps, eTextureType a_Type,  eTextureTarget a_Target)
	{
		cDX11Texture *pTex=NULL;

		if (a_Type == eTextureType_RenderTarget)
		{
			pTex = efeNew(cDX11Texture,("", this, m_pDevice, m_pContext, a_Type, a_bUseMipMaps, a_Target));
			pTex->Create(a_vSize.x, a_vSize.y, a_FillCol);
		}
		else
		{
			iBitmap2D *pBmp = CreateBitmap2D(a_vSize, a_Format);
			pBmp->FillRect(cRectl(0,0,0,0), a_FillCol);

			pTex = efeNew(cDX11Texture, ("", this, m_pDevice, m_pContext, a_Type, a_bUseMipMaps, a_Target));
		}

		return pTex;
	}

	//--------------------------------------------------------------

	iTexture *cLowLevelGraphicsDX11::CreateRenderTarget(const cVector2l &a_vSize, const int a_lDepth, const int a_lMipMapCount, const int a_lArraySize,
										const eFormat a_Format, const int a_lMultisampling, bool a_bSRGB, eTextureTarget a_Target)
	{
		cDX11Texture *pTex = efeNew(cDX11Texture, ("", this, m_pDevice, m_pContext, eTextureType_RenderTarget, false, a_Target, a_Format));

		if (pTex->CreateRenderTarget(a_vSize.x, a_vSize.y, a_lDepth, a_lMipMapCount, a_lArraySize, a_bSRGB, a_lMultisampling)==false)
		{
			efeDelete(pTex);
			Error("Could not create render target texture\n");
			return NULL;
		}

		return pTex;
	}

	iTexture *cLowLevelGraphicsDX11::CreateRenderDepth(const cVector2l &a_vSize, const int a_lArraySize, const eFormat a_Format, bool a_bSample, int a_lMultisampling, eTextureTarget a_Target)
	{
		cDX11Texture *pTex = efeNew(cDX11Texture, ("", this, m_pDevice, m_pContext, eTextureType_RenderDepth, false, a_Target, a_Format));

		if (pTex->CreateRenderDepth(a_vSize.x, a_vSize.y, a_lArraySize, a_lMultisampling, a_bSample)==false)
		{
			efeDelete(pTex);
			Error("Could not create render depth texture\n");
			return NULL;
		}

		return pTex;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetMatrix(eMatrix a_MtxType, const cMatrixf &a_mtxA)
	{
		switch (a_MtxType)
		{
		case efe::eMatrix_Model: m_pGpuParameterManager->SetWorldMatrixParameter(a_mtxA);
			break;
		case efe::eMatrix_View: m_pGpuParameterManager->SetViewMatrixParameter(a_mtxA);
			break;
		case efe::eMatrix_ModelView:
			break;
		case efe::eMatrix_Projection: m_pGpuParameterManager->SetProjMatrixParameter(a_mtxA);
			break;
		case efe::eMatrix_Texture:
			break;
		case efe::eMatrix_LastEnum:
			break;
		default:
			break;
		}
	}

	void cLowLevelGraphicsDX11::SetIdentityMatrix(eMatrix a_MtxType)
	{
		SetMatrixMode(a_MtxType);
	}

	void cLowLevelGraphicsDX11::Set2DMode()
	{
		m_vScaleBias2D.r = 2.0f / (m_vViewportSize.x);
		m_vScaleBias2D.g = 2.0f / (-m_vViewportSize.y);
		m_vScaleBias2D.b = -1.0f;
		m_vScaleBias2D.a = 1.0f;
	}

	//--------------------------------------------------------------



	void cLowLevelGraphicsDX11::SetTexture(const tString &a_sName, iTexture *a_pTexture)
	{
		m_pGpuParameterManager->SetTextureParameter(a_sName, a_pTexture);
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetTextureSamplerParams(const tString &a_sSamplerName, eTextureAddressMode a_WrapS, eTextureAddressMode a_WrapT, eTextureAddressMode a_WrapR,
		eTextureFilter a_Filter, float a_fAnisotropyDegree)
	{
		m_TempSamplerDesc.AddressU = GetDX11TextureWrapEnum(a_WrapS);
		m_TempSamplerDesc.AddressV = GetDX11TextureWrapEnum(a_WrapT);
		m_TempSamplerDesc.AddressW = GetDX11TextureWrapEnum(a_WrapR);
		m_TempSamplerDesc.Filter = GetDX11TextureFilterEnum(a_Filter);
		m_TempSamplerDesc.MaxAnisotropy = a_fAnisotropyDegree;
		
		ID3D11SamplerState *pSampler;
		std::hash_map<D3D11_SAMPLER_DESC, ID3D11SamplerState*, cSamplerDescHashConfig>::iterator it = m_hshSamplerStatesByDesc.find(m_TempSamplerDesc);
		if (it != m_hshSamplerStatesByDesc.end())
			pSampler = it->second;
		else
		{
			m_pDevice->CreateSamplerState(&m_TempSamplerDesc, &pSampler);
			m_hshSamplerStatesByDesc[m_TempSamplerDesc] = pSampler;
		}

		m_pGpuParameterManager->SetSamplerStateParameter(a_sSamplerName, (int)pSampler);
	}

	void cLowLevelGraphicsDX11::SetTextureAddressMode(eTextureAddressMode a_Mode)
	{
// 		m_SamplerDesc.AddressU = GetDX11TextureWrapEnum(a_Mode);
// 		m_SamplerDesc.AddressV = GetDX11TextureWrapEnum(a_Mode);
// 		m_SamplerDesc.AddressW = GetDX11TextureWrapEnum(a_Mode);
	}

	void cLowLevelGraphicsDX11::SetTextureFilter(eTextureFilter a_Filter)
	{
		/*m_SamplerDesc.Filter = GetDX11TextureFilterEnum(a_Filter);*/
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetBlendActive(bool a_bX)
	{
		m_BlendDesc.RenderTarget[0].BlendEnable = false;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetBlendFunc(eBlendFunc a_SrcFactor, eBlendFunc a_DestFactor)
	{
		m_BlendDesc.RenderTarget[0].SrcBlend = GetDX11BlendFuncEnum(a_SrcFactor);
		m_BlendDesc.RenderTarget[0].SrcBlendAlpha = GetDX11BlendFuncEnum(a_SrcFactor);
		m_BlendDesc.RenderTarget[0].DestBlend = GetDX11BlendFuncEnum(a_DestFactor);
		m_BlendDesc.RenderTarget[0].DestBlendAlpha = GetDX11BlendFuncEnum(a_DestFactor);
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::ClearRenderTarget(iTexture *a_pRenderTarget, const cColor &a_Col)
	{
		cDX11Texture *pTex = static_cast<cDX11Texture*>(a_pRenderTarget);
		m_pContext->ClearRenderTargetView(pTex->GetRTV(), (float*)&a_Col.r);
	}

	void cLowLevelGraphicsDX11::ClearDepthTarget(iTexture *a_pDepthTarget, const float a_fDepth)
	{
		cDX11Texture *pTex = static_cast<cDX11Texture*>(a_pDepthTarget);
		m_pContext->ClearDepthStencilView(pTex->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, a_fDepth, m_lClearStencil);
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::ClearScreen()
	{
		m_pPipelineManager->ClearBuffers(m_ClearColor, m_fClearDepth, m_lClearStencil, m_bClearColor, m_bClearDepth, m_bClearStencil);
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetClearColor(const cColor &a_Col)
	{
		m_ClearColor = a_Col;
	}
	void cLowLevelGraphicsDX11::SetClearDepth(float a_fDepth)
	{
		m_fClearDepth = a_fDepth;
	}
	void cLowLevelGraphicsDX11::SetClearStencil(int a_lVal)
	{
		m_lClearStencil = a_lVal;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetClearColorActive(bool a_bX)
	{
		m_bClearColor = a_bX;
	}
	void cLowLevelGraphicsDX11::SetClearDepthActive(bool a_bX)
	{
		m_bClearDepth = a_bX;
	}
	void cLowLevelGraphicsDX11::SetClearStencilActive(bool a_bX)
	{
		m_bClearStencil = a_bX;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetCullActive(bool a_bX)
	{
		//m_pPipelineManager->m_RasterizerStage.m_DesiredState.m_RasterizerState
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetCullMode(eCullMode a_Mode)
	{
		m_RasterierDesc.CullMode = GetDX11CullEnum(a_Mode);
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetFillMode(eFillMode a_Mode)
	{
		m_RasterierDesc.FillMode = GetDX11FillEnum(a_Mode);
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetDepthTestActive(bool a_bX)
	{
		m_DepthStencilDesc.DepthEnable = a_bX;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetDepthTestFunc(eDepthTestFunc a_Func)
	{
		m_DepthStencilDesc.DepthFunc = GetDX11DepthTestFuncEnum(a_Func);
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetDepthState(iDepthStencilState *a_pState, unsigned int a_lStencilRef)
	{
		m_pSelectedDepthState = a_pState;
		m_lSelectedStencilRef = a_lStencilRef;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetAlphaTestActive(bool a_bX)
	{
		// It'll be hard
		//m_BlendDesc.
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetAlphaTestFunc(eAlphaTestFunc a_Func, float a_fRef)
	{
		// This too
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetStencilActive(bool a_bX)
	{
		if (a_bX) m_pContext->OMSetDepthStencilState(m_pMainDepthStencilState, 1);
		else m_pContext->OMSetDepthStencilState(m_pStencilDisabledState, 1);
	}

	//--------------------------------------------------------------

	iDepthStencilState *cLowLevelGraphicsDX11::CreateDepthStencilState(bool a_bDepthTest, bool a_bDepthWrite, eDepthTestFunc a_DepthFunc,
		bool a_bStencilTest, uint8 a_lStencilMask, eStencilFunc a_StencilFunc, eStencilOp a_StencilFail,
		eStencilOp a_DepthFail, eStencilOp a_StencilPass)
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = (BOOL)a_bDepthTest;
		desc.DepthWriteMask = a_bDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = GetDX11DepthTestFuncEnum(a_DepthFunc);
		desc.StencilEnable = (BOOL)a_bStencilTest;
		desc.StencilReadMask = a_lStencilMask;
		desc.StencilWriteMask = a_lStencilMask;
		desc.BackFace.StencilFunc = GetDXStencilFuncEnum(a_StencilFunc);
		desc.FrontFace.StencilFunc = GetDXStencilFuncEnum(a_StencilFunc);
		desc.BackFace.StencilDepthFailOp = GetDXStencilOpEnum(a_DepthFail);
		desc.FrontFace.StencilDepthFailOp = GetDXStencilOpEnum(a_DepthFail);
		desc.BackFace.StencilFailOp = GetDXStencilOpEnum(a_StencilFail);
		desc.FrontFace.StencilFailOp = GetDXStencilOpEnum(a_StencilFail);
		desc.BackFace.StencilPassOp = GetDXStencilOpEnum(a_StencilPass);
		desc.FrontFace.StencilPassOp = GetDXStencilOpEnum(a_StencilPass);

		ID3D11DepthStencilState *pDXState;
		m_pDevice->CreateDepthStencilState(&desc, &pDXState);
		if (!pDXState)
		{
			FatalError("Couldn't create depth stencil state!");
			return NULL;
		}

		cDX11DepthStencilState *pState = efeNew(cDX11DepthStencilState, (pDXState));
// 		std::map<ID3D11DepthStencilState*, iDepthStencilState*>::iterator it = m_mapDepthStencilStates.find(pDXState);
// 		if (it == m_mapDepthStencilStates.end())
// 		{
// 			pState = efeNew(cDX11DepthStencilState, (pDXState));
// 			m_mapDepthStencilStates.insert(std::map<ID3D11DepthStencilState*, iDepthStencilState*>::value_type(pDXState, pState));
// 		}
// 		else
// 		{
// 			pState = static_cast<cDX11DepthStencilState*>(it->second);
// 		}

		return pState;
	}

	//--------------------------------------------------------------

	//void cLowLevelGraphicsDX::SetDepthTestActive(bool a_bX)
	//{
	//	if (a_bX) m_pDevice->SetRenderState(DXRS_ZENABLE, DXZB_TRUE);
	//	else m_Device->SetRenderState(DXRS_ZENABLE, DXZB_FALSE);
	//}

	////--------------------------------------------------------------

	//void cLowLevelGraphicsDX::SetDepthTestFunc(eDepthTestFunc a_Func)
	//{
	//	m_Device->SetRenderState(DXRS_ZFUNC,GetDXDepthTestFuncEnum(a_Func));
	//}

	////--------------------------------------------------------------

	//void cLowLevelGraphicsDX::SetCullActive(bool a_bX)
	//{
	//	if (a_bX) m_Device->SetRenderState(DXRS_CULLMODE, DXCULL_CCW);
	//	else m_Device->SetRenderState(DXRS_CULLMODE, DXCULL_NONE);
	//}

	//void cLowLevelGraphicsDX::SetCullMode(eCullMode a_Mode)
	//{
	//	if (a_Mode == eCullMode_Clockwise) m_Device->SetRenderState(DXRS_CULLMODE, DXCULL_CW);
	//	else								m_Device->SetRenderState(DXRS_CULLMODE, DXCULL_CCW);
	//}

	////--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetScissorsActive(bool a_bX)
	{
		if (a_bX) m_pContext->RSSetState(m_pScissorState);
	}

	void cLowLevelGraphicsDX11::SetScissorRect(const cRectl &a_Rect)
	{
		D3D11_RECT rect;
		rect.left = a_Rect.x;
		rect.top = a_Rect.y;
		rect.right = a_Rect.x + a_Rect.w;
		rect.bottom = a_Rect.y + a_Rect.h;
		m_pContext->RSSetScissorRects(1, &rect);
	}
	
	/*void cLowLevelGraphicsDX::FlushRendering()
	{
		
	}*/

	ID3D11InputLayout *cLowLevelGraphicsDX11::GetInputLayout(tVertexFlag a_VertexElements, iGpuProgram *a_pProgram)
	{
		std::map<tVertexFlag, std::map<iGpuProgram*, ID3D11InputLayout*>>::iterator progIt = m_mapInputLayouts.find(a_VertexElements);
		if (progIt != m_mapInputLayouts.end())
		{
			std::map<iGpuProgram*, ID3D11InputLayout*>::iterator it = progIt->second.find(a_pProgram);
			if (it != progIt->second.end())
			{
				return it->second;
			}
		}

		// Not found by program
		std::map<tVertexFlag, tInputLayoutBySignatureVecMap>::iterator signatureIt = m_mapInputLayoutsByInputSignature.find(a_VertexElements);
		if (signatureIt != m_mapInputLayoutsByInputSignature.end())
		{
			cDX11Program *pDX11Prog = static_cast<cDX11Program*>(a_pProgram);
			tInputLayoutBySignatureVecMapIt it = signatureIt->second.find(pDX11Prog->GetInputSignatureVec());
			if (it != signatureIt->second.end())
			{
				m_mapInputLayouts[a_VertexElements][a_pProgram] = it->second; // for a faster look up next time
				return it->second;
			}
		}

		// Not found neither by input signature
		GenerateInputLayout(a_VertexElements, a_pProgram);

		return m_mapInputLayouts[a_VertexElements][a_pProgram];
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::GenerateInputLayout(tVertexFlag a_VertexElementFlags, iGpuProgram *a_pProgram)
	{
		std::map<tVertexFlag, std::map<iGpuProgram*, ID3D11InputLayout*>>::iterator it = m_mapInputLayouts.find(a_VertexElementFlags);
		if (it != m_mapInputLayouts.end())
		{
			if (it->second.find(a_pProgram) != it->second.end())
				return;
		}

		static const DXGI_FORMAT formats[][4] = 
		{
			DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT, 
			DXGI_FORMAT_R16_FLOAT, DXGI_FORMAT_R16G16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT, 
			DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_R8G8_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R8G8B8A8_UNORM
		};

		static const char *semantics[] = 
		{
			"Normal",
			"Position",
			"Color",
			"Color",
			"TexCoords",
			"Tangent",
			"TexCoords",
			"TexCoords"
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> vElements;
		unsigned int vSemanticIndices[klNumOfVertexFlags] = {0};
		unsigned int lInputSlot = 0;
		int lOffset = 0;
		for (int i = 0; i < klNumOfVertexFlags; i++)
		{
			if (a_VertexElementFlags & kvVertexFlags[i])
			{
				D3D11_INPUT_ELEMENT_DESC desc;
				desc.SemanticName = semantics[cMath::Log2ToInt(kvVertexFlags[i])];
				desc.SemanticIndex = vSemanticIndices[i]++;
				desc.Format = formats[0][kvVertexElements[i]-1];
				desc.InputSlot = 0;
				desc.AlignedByteOffset = lOffset;
				desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				desc.InstanceDataStepRate = 0;
				vElements.push_back(desc);

				lOffset += 4 * kvVertexElements[i];
			}
		}

		ID3D11InputLayout *pInputLayout;
		cDX11Program *pDX11Prog = static_cast<cDX11Program*>(a_pProgram);
		HRESULT hr = m_pDevice->CreateInputLayout(vElements.data(), vElements.size(), pDX11Prog->GetCompiledProgram()->GetBufferPointer(),
			pDX11Prog->GetCompiledProgram()->GetBufferSize(), &pInputLayout);

		if (FAILED(hr))
		{
			FatalError("Couldn't create input layout for program '%s'\n", a_pProgram->GetName());
		}

		m_mapInputLayouts[a_VertexElementFlags][a_pProgram] = pInputLayout;
		m_mapInputLayoutsByInputSignature[a_VertexElementFlags][pDX11Prog->GetInputSignatureVec()] = pInputLayout;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::ConfigurePipeline()
	{
		 std::hash_map<D3D11_RASTERIZER_DESC, ID3D11RasterizerState*>::iterator rasterIt = m_hshRasterizerStates.find(m_RasterierDesc);
		 if (rasterIt != m_hshRasterizerStates.end())
		 {
			 m_pPipelineManager->m_RasterizerStage.m_DesiredState.m_RasterizerState.SetState((int)rasterIt->second);
		 }
		 else
		 {
			 ID3D11RasterizerState *pState;
			 m_pDevice->CreateRasterizerState(&m_RasterierDesc, &pState);
			 if (pState == NULL)
			 {
				 Error("Couldn't set raster state!\n");
			 }
			 else
			 {
				 //m_hshRasterizerStates.insert(std::hash_map<D3D11_RASTERIZER_DESC, ID3D11RasterizerState*>::value_type(m_BlendDesc, pState));
				 m_hshRasterizerStates.insert(std::pair<D3D11_RASTERIZER_DESC, ID3D11RasterizerState*>(m_RasterierDesc, pState));
				 m_pPipelineManager->m_RasterizerStage.m_DesiredState.m_RasterizerState.SetState((int)pState);
			 }
		 }

		 std::hash_map<D3D11_BLEND_DESC, ID3D11BlendState*>::iterator blendIt = m_hshBlendStates.find(m_BlendDesc);
		 if (blendIt != m_hshBlendStates.end())
		 {
			 m_pPipelineManager->m_OutputMergerStage.m_DesiredState.m_BlendState.SetState((int)blendIt->second);
		 }
		 else
		 {
			 ID3D11BlendState *pState;
			 m_pDevice->CreateBlendState(&m_BlendDesc, &pState);
			 if (pState == NULL)
			 {
				 Error("Couldn't set blend state!\n");
			 }
			 else
			 {
				 //m_hshRasterizerStates.insert(std::hash_map<D3D11_RASTERIZER_DESC, ID3D11RasterizerState*>::value_type(m_BlendDesc, pState));
				 m_hshBlendStates.insert(std::pair<D3D11_BLEND_DESC, ID3D11BlendState*>(m_BlendDesc, pState));
				 m_pPipelineManager->m_OutputMergerStage.m_DesiredState.m_BlendState.SetState((int)pState);
			 }
		 }
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::_CreateWindow(int a_lWidth, int a_lHeight, bool a_bFullscreen, DXGI_OUTPUT_DESC *a_pOutDesc, tString a_sWindowCaption)
	{
		if (m_hWnd != 0)
			return;

		WNDCLASS wcWndClass;

		wcWndClass.cbClsExtra = 0;
		wcWndClass.cbWndExtra = 0;
		wcWndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wcWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcWndClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
		wcWndClass.hInstance = GetModuleHandle(NULL);
		wcWndClass.lpfnWndProc = WindowEvent;
		wcWndClass.lpszClassName = "Engine Window Class";
		wcWndClass.lpszMenuName = NULL;
		wcWndClass.style = 0;

		RegisterClass(&wcWndClass);

		DWORD wndFlags = 0;

		int lX, lY, lW, lH;
		if (a_bFullscreen)
		{
			wndFlags |= WS_POPUP;
			lX = lY = 0;
			lW = m_vScreenSize.x;
			lH = m_vScreenSize.y;
		}
		else
		{
			wndFlags |= WS_OVERLAPPEDWINDOW;

			RECT rect;
			rect.left = 0;
			rect.right = m_vScreenSize.x;
			rect.top = 0;
			rect.bottom = m_vScreenSize.y;
			AdjustWindowRect(&rect, wndFlags, FALSE);

			MONITORINFO monInfo;
			monInfo.cbSize = sizeof(monInfo);
			GetMonitorInfo(a_pOutDesc->Monitor, &monInfo);

			lW = cMath::Min(rect.right - rect.left, monInfo.rcWork.right - monInfo.rcWork.left);
			lH = cMath::Min(rect.bottom - rect.top, monInfo.rcWork.bottom - monInfo.rcWork.top);
			lX = (monInfo.rcWork.left + monInfo.rcWork.right - lW) / 2;
			lY = (monInfo.rcWork.top + monInfo.rcWork.bottom - lH) / 2;
		}

		m_hWnd = CreateWindow("Engine Window Class", a_sWindowCaption.c_str(), wndFlags,
								lX, lY, lW, lH, NULL, NULL, wcWndClass.hInstance, NULL);

		if (!m_hWnd) MessageBox(0, "Can't create window!!!", "Error", IDI_WARNING || MB_OK);
		else MessageBox(0, "Window created!!!", "Good", IDI_WARNING || MB_OK);

		ShowWindow(m_hWnd, SW_SHOW);
		UpdateWindow(m_hWnd);
	}

	void cLowLevelGraphicsDX11::_ShowCursor(bool a_bX)
	{
		if (a_bX && !m_bCursorIsVisible)
		{
			ShowCursor(true);
			m_bCursorIsVisible = true;
		}
		else if (!a_bX && m_bCursorIsVisible)
		{
			ShowCursor(false);
			m_bCursorIsVisible = false;
		}
	}

	void cLowLevelGraphicsDX11::SetVsyncActive(bool a_bX)
	{
	}

	void cLowLevelGraphicsDX11::SetMultisamplingActive(bool a_bX)
	{
		if (m_lMultisampling <= 0) return;
	}

	iVertexBuffer *cLowLevelGraphicsDX11::CreateVertexBuffer(tVertexFlag a_Flags, eVertexBufferDrawType a_DrawType,
											eVertexBufferUsageType a_UsageType,
											int a_lReserveVtxSize,int a_lReserveIdxSize)
	{
		return efeNew(cDX11VertexBuffer, (this, a_Flags, a_DrawType, a_UsageType, a_lReserveVtxSize, a_lReserveIdxSize, m_pPipelineManager));
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::ChangeVertexFormat(iVertexFormat *a_pVF)
	{
// 		if (a_pVF != m_pCurrentVertexFormat)
// 		{
// 			if (a_pVF == NULL)
// 				m_pContext->IASetInputLayout(NULL);
// 			else
// 			{
// 				cDX11VertexFormat *pVF = static_cast<cDX11VertexFormat*>(a_pVF);
// 				m_pContext->IASetInputLayout(pVF->m_pInputLayout);
// 			}
// 
// 			m_pCurrentVertexFormat = a_pVF;
// 		}
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::AddVertexToBatch(const cVertex *a_pVtx)
	{
		//Coord
		m_pVertexArray[m_lVertexCount + 0] = a_pVtx->pos.x;
		m_pVertexArray[m_lVertexCount + 1] = a_pVtx->pos.y;
		m_pVertexArray[m_lVertexCount + 2] = a_pVtx->pos.z;
		//Color
		m_pVertexArray[m_lVertexCount + 3] = a_pVtx->col.r;
		m_pVertexArray[m_lVertexCount + 4] = a_pVtx->col.g;
		m_pVertexArray[m_lVertexCount + 5] = a_pVtx->col.b;
		m_pVertexArray[m_lVertexCount + 6] = a_pVtx->col.a;
		//Texture coord
		m_pVertexArray[m_lVertexCount + 7] = a_pVtx->tex.x;
		m_pVertexArray[m_lVertexCount + 8] = a_pVtx->tex.y;
		m_pVertexArray[m_lVertexCount + 9] = a_pVtx->tex.z;
		//Normal coord
		m_pVertexArray[m_lVertexCount + 10] = a_pVtx->norm.x;
		m_pVertexArray[m_lVertexCount + 11] = a_pVtx->norm.y;
		m_pVertexArray[m_lVertexCount + 12] = a_pVtx->norm.z;

		m_lVertexCount = m_lVertexCount + m_lBatchStride;

		//if (
	}

	void cLowLevelGraphicsDX11::AddVertexToBatch(const cVertex *a_pVtx, const cVector3f *a_vTransform)
	{
		//Coord
		m_pVertexArray[m_lVertexCount + 0] = a_pVtx->pos.x + a_vTransform->x;
		m_pVertexArray[m_lVertexCount + 1] = a_pVtx->pos.y + a_vTransform->y;
		m_pVertexArray[m_lVertexCount + 2] = a_pVtx->pos.z + a_vTransform->z;
		//Color
		m_pVertexArray[m_lVertexCount + 3] = a_pVtx->col.r;
		m_pVertexArray[m_lVertexCount + 4] = a_pVtx->col.g;
		m_pVertexArray[m_lVertexCount + 5] = a_pVtx->col.b;
		m_pVertexArray[m_lVertexCount + 6] = a_pVtx->col.a;
		//Texture coord
		m_pVertexArray[m_lVertexCount + 7] = a_pVtx->tex.x;
		m_pVertexArray[m_lVertexCount + 8] = a_pVtx->tex.y;
		m_pVertexArray[m_lVertexCount + 9] = a_pVtx->tex.z;
		//Normal coord
		m_pVertexArray[m_lVertexCount + 10] = a_pVtx->norm.x;
		m_pVertexArray[m_lVertexCount + 11] = a_pVtx->norm.y;
		m_pVertexArray[m_lVertexCount + 12] = a_pVtx->norm.z;

		m_lVertexCount = m_lVertexCount + m_lBatchStride;
	}

	void cLowLevelGraphicsDX11::AddVertexToBatch_Size2D(const cVertex *a_pVtx, const cVector3f *a_vTransform, 
			const cColor *a_pCol, const float &m_fW, const float &m_fH)
	{
		//Coord
		m_pVertexArray[m_lVertexCount + 0] = a_vTransform->x + m_fW;
		m_pVertexArray[m_lVertexCount + 1] = a_vTransform->y + m_fH;
		m_pVertexArray[m_lVertexCount + 2] = a_vTransform->z;
		//Color
		m_pVertexArray[m_lVertexCount + 3] = a_pCol->r;
		m_pVertexArray[m_lVertexCount + 4] = a_pCol->g;
		m_pVertexArray[m_lVertexCount + 5] = a_pCol->b;
		m_pVertexArray[m_lVertexCount + 6] = a_pCol->a;
		//Texture coord
		m_pVertexArray[m_lVertexCount + 7] = a_pVtx->tex.x;
		m_pVertexArray[m_lVertexCount + 8] = a_pVtx->tex.y;
		m_pVertexArray[m_lVertexCount + 9] = a_pVtx->tex.z;

		m_lVertexCount = m_lVertexCount + m_lBatchStride;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::AddVertexToBatch_Raw(	const cVector3f &a_vPos, const cColor &a_Color,
													const cVector3f &a_vTex)
	{
		//Pos
		m_pVertexArray[m_lVertexCount + 0] = a_vPos.x;
		m_pVertexArray[m_lVertexCount + 1] = a_vPos.y;
		m_pVertexArray[m_lVertexCount + 2] = a_vPos.z;

		//Color
		m_pVertexArray[m_lVertexCount + 3] = a_Color.r;
		m_pVertexArray[m_lVertexCount + 4] = a_Color.g;
		m_pVertexArray[m_lVertexCount + 5] = a_Color.b;
		m_pVertexArray[m_lVertexCount + 6] = a_Color.a;

		//Tex coord
		m_pVertexArray[m_lVertexCount + 7] = a_vTex.x;
		m_pVertexArray[m_lVertexCount + 8] = a_vTex.y;
		m_pVertexArray[m_lVertexCount + 9] = a_vTex.z;

		m_lVertexCount = m_lVertexCount + m_lBatchStride;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::AddIndexToBatch(int a_lIndex)
	{
		m_pIndexArray[m_lIndexCount] = a_lIndex;
		m_lIndexCount++;

		if (m_lIndexCount>=m_lBatchArraySize)
		{
			
		}
	}

	void cLowLevelGraphicsDX11::SetBatchTexture(iTexture *a_pTex)
	{
		m_pBatchTexture = a_pTex;
	}

	void cLowLevelGraphicsDX11::FlushQuadBatch(bool a_bAutoClear = true)
	{
		D3D11_MAPPED_SUBRESOURCE pData;
		D3D11_MAP flag = D3D11_MAP_WRITE_NO_OVERWRITE;
		unsigned int lSize = m_lVertexCount*sizeof(float);
		/*if (m_lVertexBatchOffset + lSize > sizeof(float) * m_lBatchStride * m_lBatchArraySize)
		{*/
		//	Log("DISCARD!!!!!!!!!!!!!!!\n");
			flag = D3D11_MAP_WRITE_DISCARD;
			m_lVertexBatchOffset = 0;
		//}

		unsigned int lBatchOffset = m_lVertexBatchOffset;
		m_pContext->Map(m_pBatchBuffer, 0, flag, NULL, &pData);
		memcpy((unsigned char*)pData.pData + lBatchOffset, (unsigned char *)m_pVertexArray, lSize);
		m_pContext->Unmap(m_pBatchBuffer, 0);

		m_lVertexBatchOffset += lSize;

		Set2DMode();

		lSize = m_lIndexCount*sizeof(unsigned int);

		//if (m_lIndexBatchOffset + lSize > 64*1024)
		//{
			flag = D3D11_MAP_WRITE_DISCARD;
			m_lIndexBatchOffset = 0;
		//}

		lBatchOffset = m_lIndexBatchOffset;

		m_pContext->Map(m_pIndexBatchBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, NULL, &pData);
		memcpy((unsigned char*)pData.pData + lBatchOffset, (unsigned char*)m_pIndexArray, lSize);
		m_pContext->Unmap(m_pIndexBatchBuffer, 0);

		m_lIndexBatchOffset += lSize;

		//SetVertexFormat(m_pTexVertexFormat);
		if (m_pBatchTexture)
		{
			BindProgram(m_pPlainTexProgram, eGpuProgramType_Vertex);
			SetTexture("Base", m_pBatchTexture);
			SetSamplerState("Sampler", m_pBatchSampler);
		}
		else
			BindProgram(m_pPlainNoTexProgram, eGpuProgramType_Vertex);

		SetShaderConstantColor4f("scaleBias", cColor(m_vScaleBias2D.r,m_vScaleBias2D.g,
													m_vScaleBias2D.b,m_vScaleBias2D.a));

		SetRasterizerState(m_pBatchRS);
		FlushRendering();


		m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ID3D11Buffer *pBuffer[] = {m_pBatchBuffer};
		unsigned int lStride = m_lBatchStride*sizeof(float);
		unsigned int lOffset = 0;
		m_pContext->IASetVertexBuffers(0, 1, pBuffer, &lStride, &lOffset);
		m_pContext->IASetIndexBuffer(m_pIndexBatchBuffer, DXGI_FORMAT_R32_UINT, 0);

		m_pContext->DrawIndexed(m_lIndexCount, 0, 0);

		if (a_bAutoClear)
		{
			m_lIndexCount = 0;
			m_lVertexCount = 0;
		}
	}

	void cLowLevelGraphicsDX11::ClearBatch()
	{
		m_lVertexCount = 0;
		m_lIndexCount = 0;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::DrawLine(const cVector3f &a_vBegin, const cVector3f &a_vEnd, cColor a_Col)
	{
		BindProgram(m_pLinePogram, eGpuProgramType_Vertex);
		cMatrixf mtxViewProj = cMath::MatrixMul(iLowLevelGraphics::s_mtxProjection, iLowLevelGraphics::s_mtxView);
		SetShaderConstantColor4f("Color", a_Col);
		SetShaderConstantMatrixf("ViewProj", mtxViewProj);
		//SetVertexFormat(m_pLineFormat);
		FlushRendering();

		D3D11_MAPPED_SUBRESOURCE Line;
		m_pContext->Map(m_pLineBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Line);
		cVector3f *pVecs = static_cast<cVector3f*>(Line.pData);
		pVecs[0] = a_vBegin;
		pVecs[1] = a_vEnd;
		m_pContext->Unmap(m_pLineBuffer, 0);

		unsigned int lStride = sizeof(cVector3f);
		unsigned int lOffset = 0;
		m_pContext->IASetVertexBuffers(0, 1, &m_pLineBuffer, &lStride, &lOffset);
		
		m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		m_pContext->Draw(2, 0);
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::DrawBoxMaxMin(const cVector3f &a_vMax, const cVector3f &a_vMin, cColor a_Col)
	{
		BindProgram(m_pLinePogram, eGpuProgramType_Vertex);
		cMatrixf mtxViewProj = cMath::MatrixMul(iLowLevelGraphics::s_mtxProjection, iLowLevelGraphics::s_mtxView);
		SetShaderConstantColor4f("Color", a_Col);
		SetShaderConstantMatrixf("ViewProj", mtxViewProj);
		//SetVertexFormat(m_pLineFormat);
		FlushRendering();

		D3D11_MAPPED_SUBRESOURCE Line;
		m_pContext->Map(m_pLineBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Line);
		cVector3f *pVecs = static_cast<cVector3f*>(Line.pData);

		//Pos Z Quad
		pVecs[0] = cVector3f(a_vMax.x, a_vMax.y, a_vMax.z);
		pVecs[1] = cVector3f(a_vMin.x, a_vMax.y, a_vMax.z);

		pVecs[2] = cVector3f(a_vMax.x, a_vMax.y, a_vMax.z);
		pVecs[3] = cVector3f(a_vMax.x, a_vMin.y, a_vMax.z);

		pVecs[4] = cVector3f(a_vMin.x, a_vMax.y, a_vMax.z);
		pVecs[5] = cVector3f(a_vMin.x, a_vMin.y, a_vMax.z);

		pVecs[6] = cVector3f(a_vMin.x, a_vMin.y, a_vMax.z);
		pVecs[7] = cVector3f(a_vMax.x, a_vMin.y, a_vMax.z);

		//Neg Z Quad
		pVecs[8] = cVector3f(a_vMax.x, a_vMax.y, a_vMin.z);
		pVecs[9] = cVector3f(a_vMin.x, a_vMax.y, a_vMin.z);

		pVecs[10] = cVector3f(a_vMax.x, a_vMax.y, a_vMin.z);
		pVecs[11] = cVector3f(a_vMax.x, a_vMin.y, a_vMin.z);

		pVecs[12] = cVector3f(a_vMin.x, a_vMax.y, a_vMin.z);
		pVecs[13] = cVector3f(a_vMin.x, a_vMin.y, a_vMin.z);

		pVecs[14] = cVector3f(a_vMin.x, a_vMin.y, a_vMin.z);
		pVecs[15] = cVector3f(a_vMax.x, a_vMin.y, a_vMin.z);

		//Lines between
		pVecs[16] = cVector3f(a_vMax.x, a_vMax.y, a_vMax.z);
		pVecs[17] = cVector3f(a_vMax.x, a_vMax.y, a_vMin.z);

		pVecs[18] = cVector3f(a_vMin.x, a_vMax.y, a_vMax.z);
		pVecs[19] = cVector3f(a_vMin.x, a_vMax.y, a_vMin.z);

		pVecs[20] = cVector3f(a_vMin.x, a_vMin.y, a_vMax.z);
		pVecs[21] = cVector3f(a_vMin.x, a_vMin.y, a_vMin.z);

		pVecs[22] = cVector3f(a_vMax.x, a_vMin.y, a_vMax.z);
		pVecs[23] = cVector3f(a_vMax.x, a_vMin.y, a_vMin.z);

		m_pContext->Unmap(m_pLineBuffer, 0);

		unsigned int lStride = sizeof(cVector3f);
		unsigned int lOffset = 0;
		m_pContext->IASetVertexBuffers(0, 1, &m_pLineBuffer, &lStride, &lOffset);
		
		m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		m_pContext->Draw(24, 0);
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::BindProgram(iGpuProgram *a_pProg, eGpuProgramType a_Type)
	{
		m_pPipelineManager->BindProgram(a_Type, a_pProg, m_pGpuParameterManager);
	}

	//--------------------------------------------------------------

	bool cLowLevelGraphicsDX11::SetShaderConstantInt(const tString &a_sName, const int a_lX)
	{
// 		if (m_pSelectedShader == NULL)
// 			return false;

		return false;// m_pSelectedShader->SetRaw(a_sName, &a_lX, sizeof(a_lX));
	}

	bool cLowLevelGraphicsDX11::SetShaderConstantFloat(const tString &a_sName, const float a_fX)
	{
// 		if (m_pSelectedShader == NULL)
// 			return false;

		return false;// m_pSelectedShader->SetRaw(a_sName, &a_fX, sizeof(a_fX));
	}

	bool cLowLevelGraphicsDX11::SetShaderConstantVec2f(const tString &a_sName, const cVector2f &a_vVec)
	{
// 		if (m_pSelectedShader == NULL)
// 			return false;

		return false;//m_pSelectedShader->SetRaw(a_sName, &a_vVec, sizeof(a_vVec));
	}

	bool cLowLevelGraphicsDX11::SetProgramVec3f(const tString &a_sName, const cVector3f &a_vVec)
	{
		m_ParameterContainer.SetVectorParameter(a_sName, cVector4f(a_vVec,0.0f));

		return true;
	}

	bool cLowLevelGraphicsDX11::SetProgramVec4f(const tString &a_sName, const cVector4f &a_vVec)
	{
		m_ParameterContainer.SetVectorParameter(a_sName, a_vVec);

		return true;
	}

	bool cLowLevelGraphicsDX11::SetShaderConstantColor4f(const tString &a_sName, const cColor &a_Col)
	{
		//m_ParameterContainer.SetVectorParameter(a_sName, cVector4f(a_Col));

		return true;
	}

	bool cLowLevelGraphicsDX11::SetShaderConstantMatrixf(const tString &a_sName, const cMatrixf &a_mMtx)
	{

		return false;
	}

	bool cLowLevelGraphicsDX11::SetShaderConstantArrayFloat(const tString &a_sName, const float *a_pX, int a_lCount)
	{

		return false;// m_pSelectedShader->SetRaw(a_sName, a_pX, a_lCount * sizeof(float));
	}

	bool cLowLevelGraphicsDX11::SetShaderConstantArrayVec2f(const tString &a_sName, const cVector2f *a_vVec, int a_lCount)
	{
// 		if (m_pSelectedShader == NULL)
// 			return false;

		return false;// m_pSelectedShader->SetRaw(a_sName, a_vVec, a_lCount * sizeof(cVector2f));
	}

	bool cLowLevelGraphicsDX11::SetShaderConstantArrayVec3f(const tString &a_sName, const cVector3f *a_vVec, int a_lCount)
	{
// 		if (m_pSelectedShader == NULL)
// 			return false;

		return false;// m_pSelectedShader->SetRaw(a_sName, a_vVec, a_lCount * sizeof(cVector3f));
	}

	bool cLowLevelGraphicsDX11::SetShaderConstantArrayColor4f(const tString &a_sName, const cColor *a_Col, int a_lCount)
	{
// 		if (m_pSelectedShader == NULL)
// 			return false;

		return false;// m_pSelectedShader->SetRaw(a_sName, a_Col, a_lCount * sizeof(cColor));
	}

	bool cLowLevelGraphicsDX11::SetShaderConstantArrayMatrixf(const tString &a_sName, const cMatrixf *a_mMtx, int a_lCount)
	{
// 		if (m_pSelectedShader == NULL)
// 			return false;

		return false;// m_pSelectedShader->SetRaw(a_sName, a_mMtx, a_lCount * sizeof(cMatrixf));
	}

	iGpuProgramBuffer *cLowLevelGraphicsDX11::CreateGpuProgramBuffer(eGpuProgramBufferType a_Type, iGpuProgramBufferConfig *a_pConfig, void *a_pData, bool a_bAutoUpdate)
	{
		ID3D11Buffer *pDXBuffer;
		cDX11ProgramBufferConfig *pDX11Config = static_cast<cDX11ProgramBufferConfig*>(a_pConfig);

		if (a_pData)
		{
			D3D11_SUBRESOURCE_DATA Data;
			Data.pSysMem = a_pData;
			Data.SysMemPitch = 0;
			Data.SysMemSlicePitch = 0;
			m_pDevice->CreateBuffer(&pDX11Config->m_State, &Data, &pDXBuffer);
		}
		else
			m_pDevice->CreateBuffer(&pDX11Config->m_State, NULL, &pDXBuffer);

		iGpuProgramBuffer *pBuffer = NULL;

		if (pDXBuffer)
		{
			switch (a_Type)
			{
			case efe::eGpuProgramBufferType_Constant:
				pBuffer = efeNew(cDX11ProgramConstantBuffer, (pDXBuffer));
				static_cast<cDX11ProgramConstantBuffer*>(pBuffer)->SetAutoUpdate(a_bAutoUpdate);
				break;
			case efe::eGpuProgramBufferType_VertexIndex: 
				pBuffer = efeNew(cDX11ProgramBuffer, (eGpuProgramBufferType_VertexIndex, pDXBuffer));
				break;
			default:
				break;
			}

			static_cast<cDX11ProgramBuffer*>(pBuffer)->SetDesiredDescription(pDX11Config->m_State);
			
		}

		return pBuffer;
	}

	void cLowLevelGraphicsDX11::ClearRenderTarget()
	{
		float Col[] = {0.1f, 1.0f, 0.1f, 1.0f};
		m_pContext->ClearRenderTargetView(m_pBackBufferRTV, Col);
		m_pContext->ClearDepthStencilView(m_pDepthBufferDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void cLowLevelGraphicsDX11::SetViewportSize(const cVector2l &a_vViewportSize)
	{
		//m_pSwapChain->ResizeBuffers();
		m_vViewportSize = a_vViewportSize;

		ChangeViewport();
	}

	cVector2l cLowLevelGraphicsDX11::GetViewportSize()
	{
		return m_vViewportSize;
	}

	//--------------------------------------------------------------

	bool cLowLevelGraphicsDX11::SetSamplerState(const tString &a_sName, iSamplerState *a_pSamplerState)
	{
		return false;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::FlushRendering()
	{
// 		if (m_pSelectedShader != m_pCurrentShader)
// 		{
// 			if (m_pSelectedShader == NULL)
// 			{
// 				m_pContext->VSSetShader(NULL, NULL, 0);
// 				m_pContext->GSSetShader(NULL, NULL, 0);
// 				m_pContext->PSSetShader(NULL, NULL, 0);
// 			}
// 			else
// 			{
// 				m_pSelectedShader->Bind();
// 				//m_pSelectedShader->UpdateConstants();
// 			}
// 
// 			m_pCurrentShader = m_pSelectedShader;
// 		}
// 		else if (m_pSelectedShader!=NULL)
// 			//m_pSelectedShader->UpdateConstants();

		FlushTextures();

		ApplySamplerStates();

		//ChangeVertexFormat(m_pSelectedVertexFormat);

		ChangeDepthState(m_pSelectedDepthState, m_lSelectedStencilRef);

		ChangeBlendState(m_pSelectedBlendState);
		
		ChangeRasterizerState(m_pSelectedRasterizerState);
	}

	void cLowLevelGraphicsDX11::FlushTextures()
	{
// 		ID3D11ShaderResourceView *pSRViews[MAX_TEXTUREUNITS];
// 
// 		int lMin, lMax;
// 		if (FillSRV(pSRViews, lMin, lMax, m_pSelectedTexturesVS, m_pCurrentTexturesVS))
// 			m_pContext->VSSetShaderResources(lMin, lMax - lMin + 1, pSRViews);
// 
// 		if (FillSRV(pSRViews, lMin, lMax, m_pSelectedTexturesGS, m_pCurrentTexturesGS))
// 			m_pContext->GSSetShaderResources(lMin, lMax - lMin + 1, pSRViews);
// 
// 		if (FillSRV(pSRViews, lMin, lMax, m_pSelectedTexturesPS, m_pCurrentTexturesPS))
// 			m_pContext->PSSetShaderResources(lMin, lMax - lMin + 1, pSRViews);
	}

	void cLowLevelGraphicsDX11::FlushConstants()
	{
		//if (m_pCurrentShader != NULL)
		{
			//m_pCurrentShader->UpdateConstants();
		}
	}


	/*void cLowLevelGraphicsDX::SetRenderTarget(iTexture *a_pTex)
	{
		
	}*/

	void cLowLevelGraphicsDX11::SwapBuffers()
	{
		m_pSwapChain->Present(0,0);
	}

	void cLowLevelGraphicsDX11::Reset(tResetFlag  a_Flags)
	{
// 		if (a_Flags & eResetFlag_Shader)
// 			m_pSelectedShader = NULL;

// 		if (a_Flags & eResetFlag_VertexFormat)
// 			m_pSelectedVertexFormat = NULL;

		if (a_Flags & eResetFlag_VertexBuffer)
		{}

		if (a_Flags & eResetFlag_IndexBuffer)
		{}

		if (a_Flags & eResetFlag_DepthState)
		{
			m_pSelectedDepthState = NULL;
			m_lSelectedStencilRef = 0;
		}

		if (a_Flags & eResetFlag_BlendState)
		{
			m_pSelectedBlendState = NULL;
			m_lSelectedSampleMask = ~0;
		}

		if (a_Flags & eResetFlag_RasterizerState)
			m_pSelectedRasterizerState = NULL;
	}

	void cLowLevelGraphicsDX11::FlushRenderTarget()
	{
		
	}

	cVector2f cLowLevelGraphicsDX11::GetScreenSize()
	{
		return cVector2f((float)m_vScreenSize.x,(float)m_vScreenSize.y);
	}

	cVector2f cLowLevelGraphicsDX11::GetVirtualSize()
	{
		return m_vVirtualSize;
	}

	void cLowLevelGraphicsDX11::SetVirtualSize(cVector2f a_vSize)
	{
		m_vVirtualSize = a_vSize;
	}

	//--------------------------------------------------------------

	D3D11_COMPARISON_FUNC cLowLevelGraphicsDX11::GetDX11DepthTestFuncEnum(eDepthTestFunc a_Type)
	{
		switch(a_Type)
		{
		case eDepthTestFunc_Never:			return D3D11_COMPARISON_NEVER;
		case eDepthTestFunc_Less:			return D3D11_COMPARISON_LESS;
		case eDepthTestFunc_LessOrEqual:	return D3D11_COMPARISON_LESS_EQUAL;
		case eDepthTestFunc_Greater:		return D3D11_COMPARISON_GREATER;
		case eDepthTestFunc_GreaterOrEqual:	return D3D11_COMPARISON_GREATER_EQUAL;
		case eDepthTestFunc_Equal:			return D3D11_COMPARISON_EQUAL;
		case eDepthTestFunc_NotEqual:		return D3D11_COMPARISON_NOT_EQUAL;
		case eDepthTestFunc_Always:			return D3D11_COMPARISON_ALWAYS;
		}
		return D3D11_COMPARISON_NEVER;
	}

	//--------------------------------------------------------------

	D3D11_COMPARISON_FUNC cLowLevelGraphicsDX11::GetDXStencilFuncEnum(eStencilFunc a_Type)
	{
		switch(a_Type)
		{
		case eStencilFunc_Never:			return D3D11_COMPARISON_NEVER;
		case eStencilFunc_Less:				return D3D11_COMPARISON_LESS;
		case eStencilFunc_LessOrEqual:		return D3D11_COMPARISON_LESS_EQUAL;
		case eStencilFunc_Greater:			return D3D11_COMPARISON_GREATER;
		case eStencilFunc_GreaterOrEqual:	return D3D11_COMPARISON_GREATER_EQUAL;
		case eStencilFunc_Equal:			return D3D11_COMPARISON_EQUAL;
		case eStencilFunc_NotEqual:			return D3D11_COMPARISON_NOT_EQUAL;
		case eStencilFunc_Always:			return D3D11_COMPARISON_ALWAYS;
		}
		return D3D11_COMPARISON_NEVER;
	}

	//--------------------------------------------------------------

	D3D11_STENCIL_OP cLowLevelGraphicsDX11::GetDXStencilOpEnum(eStencilOp a_Type)
	{
		switch(a_Type)
		{
		case eStencilOp_Keep: return D3D11_STENCIL_OP_KEEP;
		case eStencilOp_Zero: return D3D11_STENCIL_OP_ZERO;
		case eStencilOp_Replace: return D3D11_STENCIL_OP_REPLACE;
		case eStencilOp_Increment: return D3D11_STENCIL_OP_INCR;
		case eStencilOp_Decrement: return D3D11_STENCIL_OP_DECR;
		case eStencilOp_Invert: return D3D11_STENCIL_OP_INVERT;
		case eStencilOp_IncrementSat: return D3D11_STENCIL_OP_INCR_SAT;
		case eStencilOp_DecrementSat: return D3D11_STENCIL_OP_DECR_SAT;
		}
		return D3D11_STENCIL_OP_ZERO;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::SetMatrixMode(eMatrix m_Type)
	{
		
	}

	//--------------------------------------------------------------

	D3D11_BLEND cLowLevelGraphicsDX11::GetDX11BlendFuncEnum(eBlendFunc a_Type)
	{
		switch(a_Type)
		{
		case eBlendFunc_Zero:					return D3D11_BLEND_ZERO;
		case eBlendFunc_One:					return D3D11_BLEND_ONE;
		case eBlendFunc_SrcColor:				return D3D11_BLEND_SRC_COLOR;
		case eBlendFunc_OneMinusSrcColor:		return D3D11_BLEND_INV_SRC_COLOR;
		case eBlendFunc_DestColor:				return D3D11_BLEND_DEST_COLOR;
		case eBlendFunc_OneMinusDestColor:		return D3D11_BLEND_INV_DEST_COLOR;
		case eBlendFunc_SrcAlpha:				return D3D11_BLEND_SRC_ALPHA;
		case eBlendFunc_OneMinusSrcAlpha:		return D3D11_BLEND_INV_SRC_ALPHA;
		case eBlendFunc_DestAlpha:				return D3D11_BLEND_DEST_ALPHA;
		case eBlendFunc_OneMinusDestAlpha:		return D3D11_BLEND_INV_DEST_ALPHA;
		case eBlendFunc_SrcAlphaSaturate:		return D3D11_BLEND_SRC_ALPHA_SAT;
		case eBlendFunc_BlendFactor:			return D3D11_BLEND_BLEND_FACTOR;
		case eBlendFunc_OneMinusBlendFactor:	return D3D11_BLEND_INV_BLEND_FACTOR;
		case eBlendFunc_Src1Color:				return D3D11_BLEND_SRC1_COLOR;
		case eBlendFunc_OneMinusSrc1Color:		return D3D11_BLEND_INV_SRC1_COLOR;
		case eBlendFunc_Src1Alpha:				return D3D11_BLEND_SRC1_ALPHA;
		case eBlendFunc_OneMinusSrc1Alpha:		return D3D11_BLEND_INV_SRC1_ALPHA;
		}
		return D3D11_BLEND_ZERO;
	}

	D3D11_BLEND_OP cLowLevelGraphicsDX11::GetDXBlendOpEnum(eBlendOp a_Type)
	{
		switch(a_Type)
		{
		case eBlendOp_Add:					return D3D11_BLEND_OP_ADD;
		case eBlendOp_Subtract:				return D3D11_BLEND_OP_SUBTRACT;
		case eBlendOp_RevSubtract:			return D3D11_BLEND_OP_REV_SUBTRACT;
		case eBlendOp_Max:					return D3D11_BLEND_OP_MAX;
		case eBlendOp_Min:					return D3D11_BLEND_OP_MIN;
		}
		return D3D11_BLEND_OP_ADD;
	}

	//--------------------------------------------------------------

	D3D11_CULL_MODE cLowLevelGraphicsDX11::GetDX11CullEnum(eCullMode a_Mode)
	{
		switch(a_Mode)
		{
		case eCullMode_None: return D3D11_CULL_NONE;
		case eCullMode_Back: return D3D11_CULL_BACK;
		case eCullMode_Front: return D3D11_CULL_FRONT;
		}
		return D3D11_CULL_BACK;
	}

	//--------------------------------------------------------------

	D3D11_FILL_MODE	cLowLevelGraphicsDX11::GetDX11FillEnum(eFillMode a_Mode)
	{
		switch(a_Mode)
		{
		case eFillMode_Wireframe: return D3D11_FILL_WIREFRAME;
		case eFillMode_Solid: return D3D11_FILL_SOLID;
		}
		return D3D11_FILL_SOLID;
	}

	//--------------------------------------------------------------

	cGpuProgramParameterManager *cLowLevelGraphicsDX11::GetGpuParameterManager()
	{
		return m_pGpuParameterManager;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::EvaluateConstantBufferMappings(iGpuProgramBuffer *a_pBuffer)
	{
		static_cast<cDX11ProgramConstantBuffer*>(a_pBuffer)->EvaluateMapping(m_pPipelineManager, m_pGpuParameterManager);
	}

	//--------------------------------------------------------------

// 	ID3D11Device *cLowLevelGraphicsDX11::GetDevice()
// 	{
// 		return m_pDevice;
// 	}
// 
// 	ID3D11DeviceContext *cLowLevelGraphicsDX11::GetContext()
// 	{
// 		return m_pContext;
// 	}
	
	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cLowLevelGraphicsDX11::SetupDX(bool a_bFullscreen)
	{
		//Swap chain
		DXGI_SWAP_CHAIN_DESC descSwapChain;
		ZeroMemory(&descSwapChain,sizeof(descSwapChain));

		descSwapChain.BufferCount = 1;
		descSwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		descSwapChain.OutputWindow = m_hWnd;
		descSwapChain.SampleDesc.Count = 1;
		descSwapChain.SampleDesc.Quality = 0;
		descSwapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		if (a_bFullscreen)
		{
			descSwapChain.BufferDesc.RefreshRate.Numerator = 0;
			descSwapChain.BufferDesc.RefreshRate.Denominator = 1;
			descSwapChain.BufferDesc.Width = GetSystemMetrics(SM_CXSCREEN);
			descSwapChain.BufferDesc.Height = GetSystemMetrics(SM_CYSCREEN);
			descSwapChain.Windowed = false;
		}
		else	//windowed
		{	
			//descSwapChain.BufferDesc.RefreshRate.Numerator = lNumerator;
			//descSwapChain.BufferDesc.RefreshRate.Denominator = lDenominator;
			descSwapChain.BufferDesc.Width = m_vScreenSize.x;
			descSwapChain.BufferDesc.Height = m_vScreenSize.y;
			descSwapChain.Windowed = true;
		}

		UINT flags = 0;
#if defined(_DEBUG) | defined(DEBUG)
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL pFeatureLvl = D3D_FEATURE_LEVEL_11_0;

		D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, &pFeatureLvl, 1, D3D11_SDK_VERSION,
										&descSwapChain, &m_pSwapChain, &m_pDevice, NULL, &m_pContext);

		//Back buffer
		ID3D11Texture2D *pBackBuffer = 0;
		if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&pBackBuffer)))
			return false;

		m_pDevice->CreateRenderTargetView(pBackBuffer, 0, &m_pMainRenderTarget);
		pBackBuffer->Release();
		pBackBuffer = 0;

		//depth stencil state
		D3D11_DEPTH_STENCIL_DESC descDS;
		ZeroMemory(&descDS, sizeof(descDS));

		descDS.DepthEnable = true;
		descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDS.DepthFunc = D3D11_COMPARISON_LESS;

		descDS.StencilEnable = false;

		if (FAILED(m_pDevice->CreateDepthStencilState(&descDS, &m_pStencilDisabledState)))
			return false;

		descDS.StencilEnable = true;
		descDS.StencilReadMask = 0xFF;
		descDS.StencilWriteMask = 0xFF;

		descDS.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDS.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		descDS.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDS.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		descDS.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDS.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		descDS.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDS.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		if (FAILED(m_pDevice->CreateDepthStencilState(&descDS, &m_pMainDepthStencilState)))
			return false;

		m_pContext->OMSetDepthStencilState(m_pMainDepthStencilState, 1); 

		//Depth stencil
		D3D11_TEXTURE2D_DESC descBuffer;
		descBuffer.Width = m_vScreenSize.x;
		descBuffer.Height = m_vScreenSize.y;
		descBuffer.MipLevels = 1;
		descBuffer.ArraySize = 1;
		descBuffer.Format = DXGI_FORMAT_R32_TYPELESS;
		descBuffer.SampleDesc.Count = 1;
		descBuffer.SampleDesc.Quality = 0;
		descBuffer.Usage = D3D11_USAGE_DEFAULT;
		descBuffer.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		descBuffer.CPUAccessFlags = 0;
		descBuffer.MiscFlags = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV,sizeof(descDSV));
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		
		ID3D11Texture2D *pDepthStencilBuffer = NULL;
		if (FAILED(m_pDevice->CreateTexture2D(&descBuffer, 0, &pDepthStencilBuffer)))
			return false;

		if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilBuffer, &descDSV, &m_pMainDSV)))
			return false;

		m_pContext->OMSetRenderTargets(1, &m_pMainRenderTarget, m_pMainDSV);

		D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
		descSRV.Format = DXGI_FORMAT_R32_FLOAT;
		descSRV.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2D.MostDetailedMip = 0;
		descSRV.Texture2D.MipLevels = 1;
		m_pDevice->CreateShaderResourceView(pDepthStencilBuffer, &descSRV, &m_pMainDepthStencilSRV);

		pDepthStencilBuffer->Release();
		pDepthStencilBuffer = 0;

		//Raster state
		D3D11_RASTERIZER_DESC descRaster;
		descRaster.FillMode = D3D11_FILL_SOLID;
		descRaster.CullMode = D3D11_CULL_BACK;
		descRaster.FrontCounterClockwise = true;
		descRaster.DepthBias = false;
		descRaster.DepthBiasClamp = 0;
		descRaster.SlopeScaledDepthBias = 0;
		descRaster.DepthClipEnable = true;
		descRaster.ScissorEnable = false;
		descRaster.MultisampleEnable = false;
		descRaster.AntialiasedLineEnable = false;
		m_pDevice->CreateRasterizerState(&descRaster, &m_pMainRasterState);

		descRaster.ScissorEnable = true;
		m_pDevice->CreateRasterizerState(&descRaster, &m_pScissorState);

		descRaster.ScissorEnable = false;
		descRaster.CullMode = D3D11_CULL_NONE;
		m_pDevice->CreateRasterizerState(&descRaster, &m_pShadowRasterState);

		descRaster.DepthBias = true;
		descRaster.DepthBiasClamp = -1.0f;
		descRaster.SlopeScaledDepthBias = -1.0f;
		m_pDevice->CreateRasterizerState(&descRaster, &m_pDBRasterState);

		//Viewport
		D3D11_VIEWPORT viewport;
		viewport.Width = m_vScreenSize.x;
		viewport.Height = m_vScreenSize.y;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		m_pContext->RSSetViewports(1, &viewport);
		/*D3D10_TEXTURE2D_DESC descTexture;
		descTexture.Width = m_vScreenSize.x;
		descTexture.Height = m_vScreenSize.y;
		descTexture.MipLevels = 1;
		descTexture.ArraySize = 1;
		descTexture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		descTexture.SampleDesc.Count = 1;
		descTexture.SampleDesc.Quality = 0;
		descTexture.Usage = D3D10_USAGE_DEFAULT;
		descTexture.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		descTexture.CPUAccessFlags = 0;
		descTexture.MiscFlags = 0;

		ID3D10Texture2D *pBackBuffer = NULL;
		m_pDevice->CreateTexture2D(&descTexture, 0, &pBackBuffer);

		D3D10_RENDER_TARGET_VIEW_DESC descRTV;
		descRTV.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		descRTV.Texture2D.MipSlice = 0;
		descRTV.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;

		m_pDevice->CreateRenderTargetView(pTargetTexture, &descRTV, &m_pMainRenderTarget);

		//Back buffer shader resource view
		D3D10_SHADER_RESOURCE_VIEW_DESC descSRV;
		descSRV.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		descSRV.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2D.MostDetailedMip = 0;
		descSRV.Texture2D.MipLevels = 1;
		//m_pDevice->CreateShaderResourceView(pTargetTexture, &descSRV, &m_pShaderResourceView);*/



		

		//SetDepthStencilSRV(&m_pShadowDSV, &m_pShadowDepthSRV);
		//SetDepthStencil(&m_pMirrorTargetDV);
		//SetDepthStencil(&m_pPostPassDV);
		//SetDepthStencilSRV(&m_pMainPassDV, &m_pMainDepthSRV);

		//SetUpRenderTargetView(&m_pPostProcessTV, &m_pPostProcessSRV);

		//m_pDevice->OMSetRenderTargets(

		//SetViewPort();
		//SetRasterState();

		float fClearColor[4] = {1,1,1,1};

		m_pContext->ClearRenderTargetView(m_pMainRenderTarget, fClearColor);
		m_pContext->ClearDepthStencilView(m_pMainDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		return true;
	}

	void cLowLevelGraphicsDX11::ApplySamplerStates()
	{
// 		iSamplerState *pSamplers[MAX_SAMPLERSTATES];
// 
// 		int lMin, lMax;
// 		if (FillSS(pSamplers, lMin, lMax, m_pSelectedSamplerStatesVS, m_pCurrentSamplerStatesVS))
// 		{
// 			ID3D11SamplerState *pSS[MAX_SAMPLERSTATES];
// 			int lNum = lMax - lMin + 1;
// 			for (int i=0;i<lNum;i++)
// 			{
// 				cDXSamplerState *pDXSS = static_cast<cDXSamplerState*>(pSamplers[i]);
// 				pSS[i] = pDXSS->m_pSamplerState;
// 			}
// 			m_pContext->VSSetSamplers(lMin, lNum, pSS);
// 		}
// 
// 		if (FillSS(pSamplers, lMin, lMax, m_pSelectedSamplerStatesGS, m_pCurrentSamplerStatesGS))
// 		{
// 			ID3D11SamplerState *pSS[MAX_SAMPLERSTATES];
// 			int lNum = lMax - lMin + 1;
// 			for (int i=0;i<lNum;i++)
// 			{
// 				cDXSamplerState *pDXSS = static_cast<cDXSamplerState*>(pSamplers[i]);
// 				pSS[i] = pDXSS->m_pSamplerState;
// 			}
// 			m_pContext->GSSetSamplers(lMin, lNum, pSS);
// 		}
// 
// 		if (FillSS(pSamplers, lMin, lMax, m_pSelectedSamplerStatesPS, m_pCurrentSamplerStatesPS))
// 		{
// 			ID3D11SamplerState *pSS[MAX_SAMPLERSTATES];
// 			int lNum = lMax - lMin + 1;
// 			for (int i=0;i<lNum;i++)
// 			{
// 				cDXSamplerState *pDXSS = static_cast<cDXSamplerState*>(pSamplers[i]);
// 				pSS[i] = pDXSS->m_pSamplerState;
// 			}
// 			m_pContext->PSSetSamplers(lMin, lNum, pSS);
// 		}
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::ChangeDepthState(iDepthStencilState *a_pState, unsigned int a_lStencilRef)
	{
		if (a_pState != m_pCurrentDepthState || a_lStencilRef != m_lCurrentStencilRef)
		{
			cDX11DepthStencilState *pDS = static_cast<cDX11DepthStencilState*>(a_pState);
			//m_pContext->OMSetDepthStencilState(pDS->m_pDepthStencilState, a_lStencilRef);
		}

		m_pCurrentDepthState = a_pState;
		m_lCurrentStencilRef = a_lStencilRef;
	}

	//--------------------------------------------------------------

	void cLowLevelGraphicsDX11::ChangeRasterizerState(iRasterizerState *a_pRasterState)
	{
		if (m_pCurrentRasterizerState != a_pRasterState)
		{
			if (a_pRasterState == NULL)
				m_pContext->RSSetState(NULL);
			else
			{
				cDX11RasterizerState *pRS = static_cast<cDX11RasterizerState*>(a_pRasterState);
				//m_pContext->RSSetState(pRS->m_pRasterState);

			}

			m_pCurrentRasterizerState = a_pRasterState;
		}
	}

	void cLowLevelGraphicsDX11::ChangeBlendState(iBlendState *a_pBlendState, unsigned int a_lSamlpeMask)
	{
		if (a_pBlendState != m_pCurrentBlendState || a_lSamlpeMask != m_lCurrentSampleMask)
		{
			float col[4] = {0,0,0,0};
			if (a_pBlendState == NULL)
				m_pContext->OMSetBlendState(NULL, col, a_lSamlpeMask);
			else
			{
				cDXBlendState *pBS = static_cast<cDXBlendState*>(a_pBlendState);
				//m_pContext->OMSetBlendState(pBS->m_pBlendState, col, a_lSamlpeMask);
			}

			m_pCurrentBlendState = a_pBlendState;
			m_lCurrentSampleMask = a_lSamlpeMask;
		}
	}

	void cLowLevelGraphicsDX11::ChangeViewport()
	{
		m_Viewport.TopLeftX = (m_vScreenSize.x - m_vViewportSize.x) / 2;
		m_Viewport.TopLeftY = (m_vScreenSize.y - m_vViewportSize.y) / 2;
		m_Viewport.Width = m_vViewportSize.x;
		m_Viewport.Height = m_vViewportSize.y;
		m_Viewport.MaxDepth = 1.0f;
		m_Viewport.MinDepth = 0.0f;
	}

	void cLowLevelGraphicsDX11::ChangeRenderTargets(iTexture **a_pColorRT, const unsigned int a_lRTNum, iTexture *a_pDepthRT)
	{
// 		for (int i = 0;i < MAX_TEXTUREUNITS; i++)
// 		{
// 			m_pSelectedTexturesVS[i] = NULL;
// 			m_pSelectedTexturesPS[i] = NULL;
// 			m_pSelectedTexturesGS[i] = NULL;
// 		}

		FlushTextures();

		ID3D11RenderTargetView *pRTV[16];
		ID3D11DepthStencilView *pDSV;

		if (a_pDepthRT == NULL)
			pDSV = NULL;
		else
		{
			cDX11Texture *pTex = static_cast<cDX11Texture*>(a_pDepthRT);

			pDSV = pTex->GetDSV();
		}

		for (int i=0;i<a_lRTNum;i++)
		{
			cDX11Texture *pTex = static_cast<cDX11Texture*>(a_pColorRT[i]);

			pRTV[i] = pTex->GetRTV();
		}

		m_pContext->OMSetRenderTargets(a_lRTNum, pRTV, pDSV);

		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;

		if (a_lRTNum > 0)
		{
			vp.Width = m_vViewportSize.x;
			vp.Height = m_vViewportSize.y;
			/*cDXTexture *pRT = static_cast<cDXTexture*>(a_pColorRT[0]);
			if (pRT->GetRTV() == m_pBackBufferRTV)
			{
				vp.Width = m_vViewportSize.x;
				vp.Height = m_vViewportSize.y;
			}
			else
			{
				vp.Width = pRT->GetWidth();
				vp.Height = pRT->GetHeight();
			}*/
		}
		else
		{
			vp.Width = a_pDepthRT->GetWidth();
			vp.Height = a_pDepthRT->GetHeight();
		}
		m_pContext->RSSetViewports(1, &m_Viewport);
	}

	void cLowLevelGraphicsDX11::ChangeToMainFramebuffer(bool a_bChangeToDefaultDepth, iTexture *a_pCustomDepth)
	{
// 		for (int i=0;i<MAX_TEXTUREUNITS;i++)
// 		{
// 			m_pSelectedTexturesVS[i] = NULL;
// 			m_pSelectedTexturesPS[i] = NULL;
// 			m_pSelectedTexturesGS[i] = NULL;
// 		}

		FlushTextures();

		if (a_bChangeToDefaultDepth == true)
			m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDepthBufferDSV);
		else
		{
			ID3D11DepthStencilView *pDSV = a_pCustomDepth ? static_cast<cDX11Texture*>(a_pCustomDepth)->GetDSV() : NULL;
			m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, pDSV);
		}

		D3D11_VIEWPORT vp;
		vp.Width = m_vViewportSize.x;
		vp.Height = m_vViewportSize.y;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.MaxDepth = 1;
		vp.MinDepth = 0;
		m_pContext->RSSetViewports(1, &m_Viewport);

		for (int i=1;i<MAX_MRTS;i++)
		{
		}
	}

	iRasterizerState *cLowLevelGraphicsDX11::CreateRasterizerState(eCullMode a_Cull, eFillMode a_Fill, bool a_bMultiSampling,
			bool a_bScissor, float a_fDepthBias, float a_fSlopedDepthBias)
	{
		D3D11_RASTERIZER_DESC desc;
		desc.CullMode = GetDX11CullEnum(a_Cull);
		desc.FillMode = GetDX11FillEnum(a_Fill);
		desc.FrontCounterClockwise = FALSE;
		desc.DepthBias = (INT)a_fDepthBias;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = a_fSlopedDepthBias;
		desc.AntialiasedLineEnable = FALSE;
		desc.DepthClipEnable = FALSE;
		desc.MultisampleEnable = (BOOL)a_bMultiSampling;
		desc.ScissorEnable = (BOOL)a_bScissor;

		ID3D11RasterizerState *pDXState;
		m_pDevice->CreateRasterizerState(&desc, &pDXState);
		if (!pDXState)
		{
			FatalError("Couldn't create rasterizer state!");
			return NULL;
		}

		cDX11RasterizerState *pState = NULL;
		//std::map<ID3D11RasterizerState*, iRasterizerState*>::iterator it = m_mapRasterizerStates.find(pDXState);
// 		if (it == m_mapRasterizerStates.end())
// 		{
// 			pState = efeNew(cDX11RasterizerState, (pDXState));
// 			m_mapRasterizerStates.insert(std::map<ID3D11RasterizerState*, iRasterizerState*>::value_type(pDXState, pState));
// 		}
// 		else
// 		{
// 			pState = static_cast<cDX11RasterizerState*>(it->second);
// 		}

		return pState;
	}

	void cLowLevelGraphicsDX11::CreatePlainShadersAndFormats()
	{
		//m_pPlainTexProgram = efeNew(cDX11Program, ("TexDX", this,));

		cDX11Program *pProg = static_cast<cDX11Program*>(m_pPlainTexProgram);

		const char *pVertexDefs = 
		"struct VsIn{"
		"float4 position : Position;"
		"float4 color : Color;"
		"float3 texCoord : TexCoord;"
		"};"
		"struct PsIn{"
		"float4 position : SV_Position;"
		"float4 color : Color;"
		"float3 texCoord : TexCoord;"
		"};\n";

		const char *pVertexVS = 
		"float4 scaleBias;"
		"PsIn main(VsIn In){"
		"	PsIn Out;"
		"	Out.position = In.position;"
		"	Out.position.xy = Out.position.xy * scaleBias.xy + scaleBias.zw;"
		"	Out.texCoord = In.texCoord;"
		"	Out.color = In.color;"
		"	return Out;"
		"};";

		const char *pVertexTexPS = 
		"Texture2D Base : register(t0);"
		"SamplerState Sampler : register(s0);"
		"float4 main(PsIn In) : SV_Target{"
		"	return Base.Sample(Sampler, In.texCoord) * In.color;"
		"};";

		const char *pVertexNoTexPS = 
		"float4 main(PsIn In) : SV_Target{"
		"	return In.color;"
		"};";

		//pProg->CreateFromStrings(pVertexVS, NULL, pVertexTexPS, 0, 0, 0, pVertexDefs);

		//m_pPlainNoTexProgram = efeNew(cDX11Program, ("TexDX", this));
		pProg = static_cast<cDX11Program*>(m_pPlainNoTexProgram);
		//pProg->CreateFromStrings(pVertexVS, NULL, pVertexNoTexPS, 0, 0, 0, pVertexDefs);

		//m_pTexVertexFormat = CreateVertexFormat(format, sizeof(format)/sizeof(format[0]), m_pPlainTexProgram);

		//m_pLinePogram = efeNew(cDX11Program, ("LineDX", this));
		pProg = static_cast<cDX11Program*>(m_pLinePogram);

		const char *pLineVertex = 
			"struct VsIn{\n"
			"float4 position : Position;\n"
			"};\n"
			"struct PsIn{\n"
			"float4 position : SV_Position;\n"
			"};\n";

		const char *pLineVS =
			"float4x4 ViewProj;\n"
			"PsIn main(VsIn In){\n"
			"PsIn Out;\n"
			"Out.position = mul(ViewProj, In.position);\n"
			"return Out;\n"
			"};\n";

		const char *pLinePS = 
			"float4 Color : register(b0);\n"
			"float4 main(PsIn In) : SV_Target{\n"
			"	return Color;\n"
			"};\n";

		//pProg->CreateFromStrings(pLineVS, NULL, pLinePS, 0, 0, 0, pLineVertex);

		//m_pLineFormat = CreateVertexFormat(format, sizeof(format)/sizeof(format[0]), m_pLinePogram);
	}

	void cLowLevelGraphicsDX11::SetRasterizerState(iRasterizerState *a_pRasterState)
	{
		m_pSelectedRasterizerState = a_pRasterState;
	}

	iBlendState *cLowLevelGraphicsDX11::CreateBlendState(eBlendFunc a_SrcFactorColor, eBlendFunc a_DestFactorColor, eBlendFunc a_SrcFactorAlpha, eBlendFunc a_DestFactorAlpha, eBlendOp a_BlendModeColor,
			eBlendOp a_BlendModeAlpha, int a_lMask, bool a_bAlphaToCoverage)
	{
		BOOL bBlendEnable = 
			a_SrcFactorColor != eBlendFunc_One || a_DestFactorColor != eBlendFunc_Zero ||
			a_SrcFactorAlpha != eBlendFunc_One || a_DestFactorAlpha != eBlendFunc_Zero;

		D3D11_BLEND_DESC desc;
		memset(desc.RenderTarget, 0, sizeof(desc.RenderTarget));

		desc.AlphaToCoverageEnable = a_bAlphaToCoverage;

		D3D11_RENDER_TARGET_BLEND_DESC descRT;
		descRT.BlendOp = GetDXBlendOpEnum(a_BlendModeColor);
		descRT.SrcBlend = GetDX11BlendFuncEnum(a_SrcFactorColor);
		descRT.DestBlend = GetDX11BlendFuncEnum(a_DestFactorColor);
		descRT.BlendOpAlpha = GetDXBlendOpEnum(a_BlendModeAlpha);
		descRT.SrcBlendAlpha = GetDX11BlendFuncEnum(a_SrcFactorAlpha);
		descRT.DestBlendAlpha = GetDX11BlendFuncEnum(a_DestFactorAlpha);
		descRT.BlendEnable = bBlendEnable;
		descRT.RenderTargetWriteMask = a_lMask;
		desc.RenderTarget[0] = descRT;

		ID3D11BlendState *pDXState;
		m_pDevice->CreateBlendState(&desc, &pDXState);
		if (!pDXState)
		{
			FatalError("Couldn't create blend state!");
			return NULL;
		}

		cDXBlendState *pState = NULL;
// 		std::map<ID3D11BlendState*, iBlendState*>::iterator it = m_mapBlendStates.find(pDXState);
// 		if (it == m_mapBlendStates.end())
// 		{
// 			pState = efeNew(cDXBlendState, (pDXState));
// 			m_mapBlendStates.insert(std::map<ID3D11BlendState*, iBlendState*>::value_type(pDXState, pState));
// 		}
// 		else
// 		{
// 			pState = static_cast<cDXBlendState*>(it->second);
// 		}

		return pState;
	}

	void cLowLevelGraphicsDX11::SetBlendState(iBlendState *a_pBlendState, unsigned int a_lSampleMask)
	{
		//m_BlendDesc.
	}

	bool cLowLevelGraphicsDX11::CreateBuffers()
	{
		ID3D11Texture2D *pTex;
		if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *) &pTex))) return false;

		if (FAILED(m_pDevice->CreateRenderTargetView(pTex, NULL, &m_pBackBufferRTV))) return false;

		pTex->Release();
		pTex = NULL;
		
		if (m_DepthBufferFormat != DXGI_FORMAT_UNKNOWN)
		{
			D3D11_TEXTURE2D_DESC descDepth;
			descDepth.Width = m_vScreenSize.x;
			descDepth.Height = m_vScreenSize.y;
			descDepth.MipLevels = 1;
			descDepth.ArraySize = 1;
			descDepth.Format = m_DepthBufferFormat;
			descDepth.SampleDesc.Count = m_lMultisampling;
			descDepth.SampleDesc.Quality = 0;
			descDepth.Usage = D3D11_USAGE_DEFAULT;
			descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			descDepth.CPUAccessFlags = 0;
			descDepth.MiscFlags = 0;
			if (FAILED(m_pDevice->CreateTexture2D(&descDepth, NULL, &pTex)))
				return false;
		

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = m_lMultisampling > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		descDSV.Flags = 0;
		if (FAILED(m_pDevice->CreateDepthStencilView(pTex, &descDSV, &m_pDepthBufferDSV)))
			return false;
		}

		m_pPipelineManager->m_OutputMergerStage.m_DesiredState.m_RenderTargetsViews.SetState(0, (int)m_pBackBufferRTV);
		m_pPipelineManager->m_OutputMergerStage.m_DesiredState.m_DepthTargetView.SetState((int)m_pDepthBufferDSV);

		m_vViewportSize = m_vScreenSize;
		ChangeViewport();

		m_pContext->RSSetViewports(1, &m_Viewport);

		D3D11_RASTERIZER_DESC desc;
		desc.CullMode = D3D11_CULL_BACK;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.FrontCounterClockwise = FALSE;
		desc.DepthBias = 0.0f;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.AntialiasedLineEnable = FALSE;
		desc.DepthClipEnable = FALSE;
		desc.MultisampleEnable = FALSE;
		desc.ScissorEnable = FALSE;
		ID3D11RasterizerState *pRasterState;
		m_pDevice->CreateRasterizerState(&desc, &pRasterState);
		m_pContext->RSSetState(pRasterState);

		iDepthStencilState *pDepthState = CreateDepthStencilState(true, true, eDepthTestFunc_LessOrEqual);
		m_pContext->OMSetDepthStencilState(static_cast<cDX11DepthStencilState*>(pDepthState)->m_pDepthStencilState, 0);

		m_TempSamplerDesc.MinLOD = -FLT_MAX;
		m_TempSamplerDesc.MaxLOD = FLT_MAX;
		m_TempSamplerDesc.MipLODBias = 0.0f;
		m_TempSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		m_TempSamplerDesc.BorderColor[0] = 0.0f;
		m_TempSamplerDesc.BorderColor[1] = 0.0f;
		m_TempSamplerDesc.BorderColor[2] = 0.0f;
		m_TempSamplerDesc.BorderColor[3] = 0.0f;

		return true;
	}

	void cLowLevelGraphicsDX11::RegisterWndClass()
	{
		WNDCLASS wcWndClass;

		wcWndClass.cbClsExtra = 0;
		wcWndClass.cbWndExtra = 0;
		wcWndClass.hbrBackground = NULL;
		wcWndClass.hCursor = LoadCursor(NULL, IDI_APPLICATION);
		wcWndClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
		wcWndClass.hInstance = GetModuleHandle(NULL);
		wcWndClass.lpfnWndProc = WindowEvent;
		wcWndClass.lpszClassName = "Engine Window Class";
		wcWndClass.lpszMenuName = NULL;
		wcWndClass.style = 0;

		RegisterClass(&wcWndClass);
	}

	//--------------------------------------------------------------

	bool cLowLevelGraphicsDX11::CreateDepthStencilState()
	{

		return false;
	}

	bool cLowLevelGraphicsDX11::SetUpRenderTargetView(ID3D10RenderTargetView **a_ppRenderTargetView, ID3D10ShaderResourceView **a_ppShaderResourceView)
	{
		D3D11_TEXTURE2D_DESC descTexture;
		descTexture.Width = m_vScreenSize.x;
		descTexture.Height = m_vScreenSize.y;
		descTexture.MipLevels = 1;
		descTexture.ArraySize = 1;
		descTexture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		descTexture.SampleDesc.Count = 1;
		descTexture.SampleDesc.Quality = 0;
		descTexture.Usage = D3D11_USAGE_DEFAULT;
		descTexture.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		descTexture.CPUAccessFlags = 0;
		descTexture.MiscFlags = 0;

		ID3D11Texture2D *pTargetTexture = NULL;
		m_pDevice->CreateTexture2D(&descTexture, 0, &pTargetTexture);

		D3D11_RENDER_TARGET_VIEW_DESC descRTV;
		descRTV.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		descRTV.Texture2D.MipSlice = 0;
		descRTV.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		//m_pDevice->CreateRenderTargetView(pTargetTexture, &descRTV, a_ppRenderTargetView);

		D3D10_SHADER_RESOURCE_VIEW_DESC descSRV;
		descSRV.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		descSRV.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2D.MostDetailedMip = 0;
		descSRV.Texture2D.MipLevels = 1;
		//m_pDevice->CreateShaderResourceView(pTargetTexture, &descSRV, a_ppShaderResourceView);

		return true;
	}

	bool cLowLevelGraphicsDX11::SetDepthStencil(ID3D10DepthStencilView **a_ppDepthView)
	{
		HRESULT hr;
	
		return true;
	}

	bool cLowLevelGraphicsDX11::SetDepthStencilSRV(ID3D10DepthStencilView **a_ppDepthView, ID3D10ShaderResourceView **a_ppShaderResourceView)
	{
		HRESULT hr;
		D3D11_TEXTURE2D_DESC bufferDesc;
		bufferDesc.Width = m_vScreenSize.x;
		bufferDesc.Height = m_vScreenSize.y;
		bufferDesc.MipLevels = 1;
		bufferDesc.ArraySize = 1;
		bufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		ID3D11Texture2D *pDepthStencilBuffer = NULL;
		hr = m_pDevice->CreateTexture2D(&bufferDesc, 0, &pDepthStencilBuffer);
		if (hr != S_OK) return false;

		D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		//hr = m_pDevice->CreateDepthStencilView(pDepthStencilBuffer, &descDSV, a_ppDepthView);
		if (hr != S_OK) return false;
		
		return true;
	}

	void cLowLevelGraphicsDX11::SetViewPort()
	{
		
	}

	void cLowLevelGraphicsDX11::SetRasterState()
	{
	
	}

	LRESULT CALLBACK WindowEvent(HWND a_hWnd, UINT a_Msg, WPARAM a_wParam, LPARAM a_lParam)
	{
		switch(a_Msg)
		{
		case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}
			break;
		case WM_INPUT:
			break;
			/*case WM_ACTIVATE:
			{
			if (a_wParam == WA_ACTIVE || a_wParam == WA_CLICKACTIVE)
			PostMessage(a_hWnd, WM_ACTIVE, a_wParam, a_lParam);
			else if (a_wParam == WA_INACTIVE)
			PostMessage(a_hWnd, WM_INACTIVE, a_wParam, a_lParam);
			return 0;
			}
			break;
			}*/
		}

		return DefWindowProc(a_hWnd, a_Msg, a_wParam, a_lParam);
	}

	D3D11_FILTER cLowLevelGraphicsDX11::GetDX11TextureFilterEnum(eTextureFilter a_Filter)
	{
		switch(a_Filter)
		{
		case eTextureFilter_Nearest: return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case eTextureFilter_Linear: return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		case eTextureFilter_Bilinear: return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		case eTextureFilter_Trilinear: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		case eTextureFilter_BilinearAniso: return D3D11_FILTER_ANISOTROPIC;
		case eTextureFilter_TrilinearAniso: return D3D11_FILTER_ANISOTROPIC;
		}

		return D3D11_FILTER_MIN_MAG_MIP_POINT;
	}

	D3D11_TEXTURE_ADDRESS_MODE cLowLevelGraphicsDX11::GetDX11TextureWrapEnum(eTextureAddressMode a_Wrap)
	{
		switch(a_Wrap)
		{
		case eTextureAddressMode_Wrap: return D3D11_TEXTURE_ADDRESS_WRAP;
		case eTextureAddressMode_Clamp: return D3D11_TEXTURE_ADDRESS_CLAMP;
		case eTextureAddressMode_Border: return D3D11_TEXTURE_ADDRESS_BORDER;
		}
		return D3D11_TEXTURE_ADDRESS_WRAP;
	}
}
