#include "impl/DX11Texture.h"
#include "impl/DX11Bitmap2D.h"

#include "system/LowLevelSystem.h"
#include "math/Math.h"
#include "math.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cDX11Texture::cDX11Texture(const  tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics, ID3D11Device *a_pDevice, ID3D11DeviceContext *a_pContext,
					eTextureType a_Type, bool a_bUseMipMaps, eTextureTarget a_Target, eFormat a_Format, bool a_bSRGB)
	: iTexture(a_sName,"DX11",a_pLowLevelGraphics,a_Type,a_bUseMipMaps,a_Target,a_Format,a_bSRGB)
	{
		m_pDevice = a_pDevice;
		m_pContext = a_pContext;

		m_bContainsData = false;

		m_TexFormat = formats[a_Format];

		m_pPBuffer = NULL;

		if (a_Type == eTextureType_RenderTarget)
		{
			//CreateSRV(
		}

		if (a_Target == eTextureTarget_CubeMap) m_bUseMipMaps = false;

		m_pGfxDX = static_cast<cLowLevelGraphicsDX11*> (m_pLowLevelGraphics);

		m_lTextureIndex = 0;
		m_fTimeCount = 0;

		m_fTimeDir;

		m_lBpp = 0;

		m_pSRV = NULL;
		m_pRTV = NULL;
		m_pDSV = NULL;
	}

	cDX11Texture::~cDX11Texture()
	{
		if (m_pSRV) m_pSRV->Release();
		if (m_pRTV) m_pRTV->Release();
		if (m_pDSV) m_pDSV->Release();
	}

	bool cDX11Texture::CreateFromBitmap(iBitmap2D *a_pBmp)
	{
		cDX11Bitmap2D *pSrc = static_cast<cDX11Bitmap2D*>(a_pBmp);

		switch(pSrc->GetFormat())
		{
		case eFormat_RGB8:
			pSrc->Convert(eFormat_RGBA8);
			break;
		case eFormat_RGB16:
			pSrc->Convert(eFormat_RGBA16);
			break;
		case eFormat_RGB16F:
			pSrc->Convert(eFormat_RGBA16F);
			break;
		case eFormat_RGB32F:
			pSrc->Convert(eFormat_RGBA32F);
			break;
		}

		m_Format = pSrc->GetFormat();
		unsigned int lMipMapsNum = pSrc->GetMipMapsCount();
		unsigned int lSlicesNum = pSrc->IsCube() ? 6 : 1;
		unsigned int lArraySize = pSrc->GetArraySize();

		static D3D11_SUBRESOURCE_DATA vTexData[1024];
		D3D11_SUBRESOURCE_DATA *pDest = vTexData;
		for (unsigned int n = 0; n < lArraySize; n++)
		{
			for (unsigned int k = 0; k < lSlicesNum; k++)
			{
				for (unsigned int i = 0; i < lMipMapsNum; i++)
				{
					unsigned int lPitch, lSlicePitch;
					if (IsCompressedFormat(m_Format))
					{
						lPitch = ((pSrc->GetWidth(i) + 3) >> 2) * GetBytesPerBlock(m_Format);
						lSlicePitch = lPitch * ((pSrc->GetHeight(i) + 3) >> 2);
					}
					else
					{
						lPitch = pSrc->GetWidth(i) * GetBytesPerPixel(m_Format);
						lSlicePitch = lPitch * pSrc->GetHeight(i);
					}

					pDest->pSysMem = pSrc->GetPixels(i, n) + k * lSlicePitch;
					pDest->SysMemPitch = lPitch;
					pDest->SysMemSlicePitch = lSlicePitch;
					pDest++;
				}
			}
		}

		m_TexFormat = formats[m_Format];
		if (m_Type == eTextureType_Normal)
		{
			switch(m_TexFormat)
			{
			case DXGI_FORMAT_R8G8B8A8_UNORM: m_TexFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; break;
			case DXGI_FORMAT_BC1_UNORM: m_TexFormat = DXGI_FORMAT_BC1_UNORM_SRGB; break;
			case DXGI_FORMAT_BC2_UNORM: m_TexFormat = DXGI_FORMAT_BC2_UNORM_SRGB; break;
			case DXGI_FORMAT_BC3_UNORM: m_TexFormat = DXGI_FORMAT_BC3_UNORM_SRGB; break;
			}
		}

		HRESULT hr;
		if (pSrc->Is2D() || pSrc->IsCube())
		{
			D3D11_TEXTURE2D_DESC desc;
			desc.Width = pSrc->GetWidth();
			desc.Height = pSrc->GetHeight();
			desc.Format = m_TexFormat;
			desc.MipLevels = lMipMapsNum;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_IMMUTABLE;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			if (pSrc->IsCube())
			{
				desc.ArraySize = 6 * lArraySize;
				desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
			}
			else
			{
				desc.ArraySize = 1;
				desc.MiscFlags = 0;
			}

			hr = m_pDevice->CreateTexture2D(&desc, vTexData, (ID3D11Texture2D**)&m_pDxTexture);
		}


		if (FAILED(hr))
			FatalError("Could not create texture '%s'\n", m_sName);

		m_SRVFormat = m_TexFormat;
		m_pSRV = CreateSRV(m_pDxTexture, m_SRVFormat);
		return true;
	}

	bool cDX11Texture::CreateCubeMapFromBitmapVec(tBitmap2DVec *a_vBitmaps)
	{
		if (m_Target != eTextureTarget_CubeMap)
			return false;

		if (a_vBitmaps->size()<6)
		{
			Error("Only %d supplified for creation of cube map, 6 needed.",a_vBitmaps->size());
			return false;
		}

		cDX11Bitmap2D *pDest = efeNew(cDX11Bitmap2D, ());
		pDest->CreateFromBitmapVec(a_vBitmaps);

		CreateFromBitmap(pDest);

		return true;
	}

	bool cDX11Texture::Create(unsigned int a_lWidth, unsigned int a_lHeight, cColor a_Col)
	{
		//m_vTextureHandles.resize(1);

		if (m_Type == eTextureType_RenderTarget)
		{
			m_lWidth = a_lWidth;
			m_lHeight = a_lHeight;

			if ((!cMath::IsPow2(m_lHeight) || !cMath::IsPow2(m_lWidth)) && m_Target != eTextureTarget_Rect)
				Warning("Texture '%s' does not have a pow2 size!", m_sName.c_str());
		}
		return true;
	}

	void cDX11Texture::SetPixels2D(int a_lLevel, const cVector2l &a_vOffset, const cVector2l &a_vSize,
			eFormat a_Format, void *a_pPixelData)
	{
		if (m_Format != eFormat_RGBA8 && m_Format != eFormat_RGB8) return;

		D3D11_MAPPED_SUBRESOURCE MappedTex;
		//m_pGfxDX->GetContext()->Map(m_pDxTexture, 0, D3D11_MAP_WRITE, 0, &MappedTex);

		unsigned char *pBuffer = reinterpret_cast<unsigned char*>(MappedTex.pData);

		//int vStart = 

		//m_pGfxDX->GetContext()->Unmap(m_pDxTexture, 0);
	}

	bool cDX11Texture::CreateRenderTarget(const int a_lWidth, const int a_lHeight, const int a_lDepth, const int a_lMipMapCount,
										const int a_lArraySize, bool a_bSRGB, const int a_lMultisampling)
	{
		m_lWidth = a_lWidth;
		m_lHeight = a_lHeight;

		ID3D11Resource *pTex;

		if (a_bSRGB)
		{
			switch(m_TexFormat)
			{
			case DXGI_FORMAT_R8G8B8A8_UNORM: m_TexFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;break;
			case DXGI_FORMAT_BC1_UNORM: m_TexFormat = DXGI_FORMAT_BC1_UNORM_SRGB;break;
			case DXGI_FORMAT_BC2_UNORM: m_TexFormat = DXGI_FORMAT_BC2_UNORM_SRGB;break;
			case DXGI_FORMAT_BC3_UNORM: m_TexFormat = DXGI_FORMAT_BC3_UNORM_SRGB;break;
			}
		}

		if (a_lDepth == 1)
		{
			D3D11_TEXTURE2D_DESC desc;
			desc.Width = m_lWidth;
			desc.Height = m_lHeight;
			desc.Format = m_TexFormat;
			desc.MipLevels = m_lSizeLevel;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			desc.CPUAccessFlags = 0;
			if (m_Target == eTextureTarget_CubeMap)
			{
				desc.ArraySize = 6;
				desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
			}
			else
			{
				desc.ArraySize = 1;
				desc.MiscFlags = 0;
			}
			if (m_bUseMipMaps == true)
				desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

			//if (FAILED(m_pGfxDX->GetDevice()->CreateTexture2D(&desc, NULL, (ID3D11Texture2D**)&pTex)))
			{
				//pTex->Release();
				return false;
			}
		}
		else
		{
			D3D11_TEXTURE3D_DESC desc;
			desc.Width = m_lWidth;
			desc.Height = m_lHeight;
			desc.Depth = a_lDepth;
			desc.Format = m_TexFormat;
			desc.MipLevels = a_lMipMapCount;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			if (m_bUseMipMaps)
				desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

			//if (FAILED(m_pGfxDX->GetDevice()->CreateTexture3D(&desc, NULL, (ID3D11Texture3D**)&pTex)))
			{
				//pTex->Release();
				return false;
			}
		}

		


		m_SRVFormat = m_TexFormat;
		m_RTVFormat = m_TexFormat;
		m_pSRV = CreateSRV(pTex,DXGI_FORMAT_UNKNOWN, -1, -1);
		m_pRTV = CreateRTV(pTex, DXGI_FORMAT_UNKNOWN, -1,-1);

		int lSliceCount = (a_lDepth == 1) ? a_lArraySize : a_lDepth;

		pTex->Release();

		m_bContainsData = true;

		return true;
	}

	bool cDX11Texture::CreateRenderDepth(const int a_lWidth, const int a_lHeight,	const int a_lArraySize, const int a_lMultisampling, bool a_bSample)
	{
		m_lWidth = a_lWidth;
		m_lHeight = a_lHeight;
		m_lDepth = 1;
		//m_lArraySize = 
		m_DSVFormat = m_TexFormat;

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = m_lWidth;
		desc.Height = m_lHeight;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = a_lMultisampling;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		if (m_Target == eTextureTarget_CubeMap)
		{
			desc.ArraySize = 6;
			desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		}
		else
		{
			desc.ArraySize = a_lArraySize;
			desc.MiscFlags = 0;
		}

		if (a_bSample == true)
		{
			switch(m_DSVFormat)
			{
			case DXGI_FORMAT_D16_UNORM:
				m_TexFormat = DXGI_FORMAT_R16_TYPELESS;
				m_SRVFormat = DXGI_FORMAT_R16_UNORM;
				break;
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
				m_TexFormat = DXGI_FORMAT_R24G8_TYPELESS;
				m_SRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;
			case DXGI_FORMAT_D32_FLOAT:
				m_TexFormat = DXGI_FORMAT_R32_TYPELESS;
				m_SRVFormat = DXGI_FORMAT_R32_FLOAT;
				break;
			}
			desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		}
		desc.Format = m_TexFormat;

		ID3D11Resource *pTex = NULL;
		//if (FAILED(m_pGfxDX->GetDevice()->CreateTexture2D(&desc, NULL, (ID3D11Texture2D **)&pTex)))
			return false;

		m_pDSV = CreateDSV(pTex, m_DSVFormat);

		if (a_bSample == true)
		{
			m_pSRV = CreateSRV(pTex, m_SRVFormat);
			
		}

		return true;
	}

	ID3D11ShaderResourceView *cDX11Texture::CreateSRV(ID3D11Resource *a_pResource, DXGI_FORMAT a_Fmt, const int a_lFirstSlice, const int a_lSliceCount)
	{
		D3D11_RESOURCE_DIMENSION Type;
		a_pResource->GetType(&Type);

		D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;

		switch(Type)
		{
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			D3D11_TEXTURE2D_DESC desc2D;
			((ID3D11Texture2D *)a_pResource)->GetDesc(&desc2D);

			descSRV.Format = (m_SRVFormat != DXGI_FORMAT_UNKNOWN) ? m_SRVFormat : desc2D.Format;
			if (desc2D.ArraySize > 1)
			{
				if (desc2D.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
				{
					if (desc2D.ArraySize == 6)
					{
						descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
						descSRV.TextureCube.MostDetailedMip = 0;
						descSRV.TextureCube.MipLevels = desc2D.MipLevels;
					}
					else
					{
						descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
						descSRV.TextureCubeArray.MostDetailedMip = 0;
						descSRV.TextureCubeArray.MipLevels = desc2D.MipLevels;
						descSRV.TextureCubeArray.First2DArrayFace = 0;
						descSRV.TextureCubeArray.NumCubes = desc2D.ArraySize / 6;
					}
				}
				else
				{
					descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;

					if (a_lFirstSlice < 0)
					{
						descSRV.Texture2DArray.FirstArraySlice = 0;
						descSRV.Texture2DArray.ArraySize = desc2D.ArraySize;
					}
					else
					{
						descSRV.Texture2DArray.FirstArraySlice = a_lFirstSlice;
						if (a_lSliceCount < 0)
							descSRV.Texture2DArray.ArraySize = 1;
						else
							descSRV.Texture2DArray.ArraySize = a_lSliceCount;
					}
					descSRV.Texture2DArray.MostDetailedMip = 0;
					descSRV.Texture2DArray.MipLevels = desc2D.MipLevels;
				}


				descSRV.Texture2D.MostDetailedMip = 0;
				descSRV.Texture2D.MipLevels = desc2D.MipLevels;
			}
			else
			{
				descSRV.ViewDimension = (desc2D.SampleDesc.Count > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;

				descSRV.Texture2D.MostDetailedMip = 0;
				descSRV.Texture2D.MipLevels = desc2D.MipLevels;
			}
			break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			break;
		}

		ID3D11ShaderResourceView *pSRV = NULL;
		if (FAILED(m_pDevice->CreateShaderResourceView(a_pResource, &descSRV, &pSRV)))
		{
			FatalError("Could not create Shader resource view '%s'\n", m_sName);
			return NULL;
		}

		return pSRV;
	}

	ID3D11RenderTargetView *cDX11Texture::CreateRTV(ID3D11Resource *a_pResource, DXGI_FORMAT a_Fmt, const int a_lFirstSlice, const int a_lSliceCount)
	{
		D3D11_RESOURCE_DIMENSION Type;
		a_pResource->GetType(&Type);

		D3D11_RENDER_TARGET_VIEW_DESC descRTV;

		switch(Type)
		{
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			D3D11_TEXTURE2D_DESC desc2D;
			((ID3D11Texture2D*)a_pResource)->GetDesc(&desc2D);

			descRTV.Format = (m_RTVFormat != DXGI_FORMAT_UNKNOWN) ?  m_RTVFormat : desc2D.Format;
			if (desc2D.ArraySize > 1)
			{

			}
			else
			{
				descRTV.ViewDimension = (desc2D.SampleDesc.Count > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
				descRTV.Texture2D.MipSlice = 0;
			}
			break;
		}

		ID3D11RenderTargetView *pRTV = NULL;
		//if (FAILED(m_pGfxDX->GetDevice()->CreateRenderTargetView(a_pResource, &descRTV, &pRTV)))
		{
			FatalError("Could not create Shader resource view '%s'\n", m_sName);
			return NULL;
		}

		return pRTV;
	}

	ID3D11DepthStencilView *cDX11Texture::CreateDSV(ID3D11Resource *a_pResource, DXGI_FORMAT a_Fmt, const int a_lFirstSlice, const int a_lSliceCount)
	{
		D3D11_RESOURCE_DIMENSION Type;
		a_pResource->GetType(&Type);

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;

		switch(Type)
		{
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			D3D11_TEXTURE2D_DESC desc2D;
			((ID3D11Texture2D*)a_pResource)->GetDesc(&desc2D);

			descDSV.Format = (a_Fmt != DXGI_FORMAT_UNKNOWN) ? a_Fmt : desc2D.Format;
			if (desc2D.ArraySize > 1)
			{
				
			}
			else
			{
				descDSV.ViewDimension = (desc2D.SampleDesc.Count > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
				descDSV.Texture2D.MipSlice = 0;
			}
			break;

		default: break;
		}

		descDSV.Flags = 0;

		ID3D11DepthStencilView *pDSV = NULL;
		if (FAILED(m_pDevice->CreateDepthStencilView(a_pResource, &descDSV, &pDSV)))
		{
			return NULL;
		}

		return pDSV;
	}
}