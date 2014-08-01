#ifndef EFE_DX_TEXTURE_H
#define EFE_DX_TEXTURE_H

#include "graphics/Texture.h"
#include "impl/PBuffer.h"

#include "impl/LowLevelGraphicsDX11.h"

namespace efe
{
	static DXGI_FORMAT formats[] = 
	{
		DXGI_FORMAT_UNKNOWN,

		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8G8_UNORM,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R8G8B8A8_UNORM,

		DXGI_FORMAT_R16_UNORM,
		DXGI_FORMAT_R16G16_UNORM,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R16G16B16A16_UNORM,

		DXGI_FORMAT_R8_SNORM,
		DXGI_FORMAT_R8G8_SNORM,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R8G8B8A8_SNORM,

		DXGI_FORMAT_R16_SNORM,
		DXGI_FORMAT_R16G16_SNORM,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R16G16B16A16_SNORM,

		DXGI_FORMAT_R16_FLOAT,
		DXGI_FORMAT_R16G16_FLOAT,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R16G16B16A16_FLOAT,

		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,

		DXGI_FORMAT_R16_SINT,
		DXGI_FORMAT_R16G16_SINT,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R16G16B16A16_SINT,

		DXGI_FORMAT_R32_SINT,
		DXGI_FORMAT_R32G32_SINT,
		DXGI_FORMAT_R32G32B32_SINT,
		DXGI_FORMAT_R32G32B32A32_SINT,

		DXGI_FORMAT_R16_UINT,
		DXGI_FORMAT_R16G16_UINT,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R16G16B16A16_UINT,

		DXGI_FORMAT_R32_UINT,
		DXGI_FORMAT_R32G32_UINT,
		DXGI_FORMAT_R32G32B32_UINT,
		DXGI_FORMAT_R32G32B32A32_UINT,

		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
		DXGI_FORMAT_R11G11B10_FLOAT,
		DXGI_FORMAT_B5G6R5_UNORM,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R10G10B10A2_UNORM,

		DXGI_FORMAT_D16_UNORM,
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		DXGI_FORMAT_D32_FLOAT,

		DXGI_FORMAT_BC1_UNORM,
		DXGI_FORMAT_BC2_UNORM,
		DXGI_FORMAT_BC3_UNORM,
		DXGI_FORMAT_BC4_UNORM,
		DXGI_FORMAT_BC5_UNORM
	};


	class cDX11Texture : public iTexture
	{
	public:
		cDX11Texture(const  tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics, ID3D11Device *a_pDevice, ID3D11DeviceContext *a_pContext,
					eTextureType a_Type, bool a_bUseMipMaps, eTextureTarget a_Target, eFormat a_Format = eFormat_None, bool a_bSRGB = false);
					
		~cDX11Texture();

		bool CreateFromBitmap(iBitmap2D *a_pBmp);

		bool CreateRenderTarget(const int a_lWidth, const int a_lHeight, const int a_lDepth, const int a_lMipMapCount,
									const int a_lArraySize, bool a_bSRGB, const int a_lMultisampling);
		bool CreateRenderDepth(const int a_lWidth, const int a_lHeight,	const int a_lArraySize, const int a_lMultisampling, bool a_bSample);

		bool CreateAnimFromBitmapVec(tBitmap2DVec *a_vBitmaps);

		bool CreateCubeMapFromBitmapVec(tBitmap2DVec *a_vBitmaps);
		bool Create(unsigned int a_lWidth, unsigned int a_lHeight, cColor a_Col);

		void SetPixels2D(int a_lLevel, const cVector2l &a_vOffset, const cVector2l &a_vSize,
			eFormat a_Format, void *a_pPixelData);

		ID3D11ShaderResourceView *GetSRV(){return m_pSRV;}
		ID3D11RenderTargetView *GetRTV(){return m_pRTV;}
		ID3D11DepthStencilView *GetDSV(){return m_pDSV;}

	private:
		bool CreateFromBitmapToHandle(iBitmap2D *pBmp, int a_lHandleIdx);

		ID3D11ShaderResourceView *CreateSRV(ID3D11Resource *a_pResource, DXGI_FORMAT a_Fmt = DXGI_FORMAT_UNKNOWN, const int a_lFirstSlice = -1, const int a_lSliceCount = -1);
		ID3D11RenderTargetView *CreateRTV(ID3D11Resource *a_pResource, DXGI_FORMAT a_Fmt = DXGI_FORMAT_UNKNOWN, const int a_lFirstSlice = -1, const int a_lSliceCount = -1);
		ID3D11DepthStencilView *CreateDSV(ID3D11Resource *a_pResource, DXGI_FORMAT a_Fmt = DXGI_FORMAT_UNKNOWN, const int a_lFirstSlice = -1, const int a_lSliceCount = -1);

		ID3D11Device *m_pDevice;
		ID3D11DeviceContext *m_pContext;

		ID3D11Resource *m_pDxTexture;

		ID3D11ShaderResourceView *m_pSRV;
		ID3D11ShaderResourceView *m_pSRVArray;

		ID3D11RenderTargetView *m_pRTV;
		ID3D11DepthStencilView *m_pDSV;

		bool m_bContainsData;
		cLowLevelGraphicsDX11 *m_pGfxDX;

		//void SetWrapS(eTextureWrap a_Mode);
		//void SetWrapT(eTextureWrap a_Mode);
		//void SetWrapR(eTextureWrap a_Mode);

		float m_fTimeCount;
		int m_lTextureIndex;
		float m_fTimeDir;

		DXGI_FORMAT m_SRVFormat;
		DXGI_FORMAT m_RTVFormat;
		DXGI_FORMAT m_DSVFormat;
		DXGI_FORMAT m_TexFormat;

		cPBuffer *m_pPBuffer;
	};
};
#endif