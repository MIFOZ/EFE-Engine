#include "impl/DX11Bitmap2D.h"
#include "graphics/LowLevelGraphics.h"


//extern "C"
//{
//#include "impl/jpeglib.h"
//}
//#pragma comment(lib, "../libjpeg.lib")

namespace efe
{
	unsigned int RGBToRGBE8(const cVector3f &a_vRGB)
	{
		float fV = cMath::Max(a_vRGB.x, a_vRGB.y);
		fV = cMath::Max(fV, a_vRGB.z);

		if (fV < 1e-32f)
			return 0;
		else
		{
			int lEx;
			float fM = frexpf(fV, &lEx) * 256.0f / fV;

			unsigned int lR = (unsigned int) (fM * a_vRGB.x);
			unsigned int lG = (unsigned int) (fM * a_vRGB.y);
			unsigned int lB = (unsigned int) (fM * a_vRGB.z);
			unsigned int lE = (unsigned int) (lEx + 128);

			return lR | (lG << 8) | (lB << 16) | (lE << 24);
		}
	}

	unsigned int RGBToRGB9E5(const cVector3f &a_vRGB)
	{
		float fV = cMath::Max(a_vRGB.x, a_vRGB.y);
		fV = cMath::Max(fV, a_vRGB.z);

		if (fV < 1.52587890625e-5f)
			return 0;
		else if (fV < 65536)
		{
			int lEx;
			float fM = frexpf(fV, &lEx) * 512.0f / fV;

			unsigned int lR = (unsigned int) (fM * a_vRGB.x);
			unsigned int lG = (unsigned int) (fM * a_vRGB.y);
			unsigned int lB = (unsigned int) (fM * a_vRGB.z);
			unsigned int lE = (unsigned int) (lEx + 15);

			return lR | (lG << 9) | (lB << 18) | (lE << 27);
		}
		else
		{
			unsigned int lR = (a_vRGB.x < 65536)? (unsigned int) (a_vRGB.x * (1.0f / 128.0f)) : 0x1FF;
			unsigned int lG = (a_vRGB.y < 65536)? (unsigned int) (a_vRGB.y * (1.0f / 128.0f)) : 0x1FF;
			unsigned int lB = (a_vRGB.z < 65536)? (unsigned int) (a_vRGB.z * (1.0f / 128.0f)) : 0x1FF;
			unsigned int lE = 31;

			return lR | (lG << 9) | (lB << 18) | (lE << 27);
		}
	}

#pragma pack (push, 1)

	struct cBMPHeader
	{
		uint16  bmpIdentifier;
		uint8 junk[16];
		uint32 width;
		uint32 height;
		uint16 junk2;
		uint16 bpp;
		uint16 compression;
		uint8 junk3[22];
	};

#pragma pack (pop)

	template <typename DATA_TYPE>
	inline void SwapChannels(DATA_TYPE *a_pPixels, int a_lNumPixels, const int a_lChannels, const int a_lCh0, const int a_lCh1)
	{
		do
		{
			DATA_TYPE tmp = a_pPixels[a_lCh1];
			a_pPixels[a_lCh1] = a_pPixels[a_lCh0];
			a_pPixels[a_lCh0] = tmp;
			a_pPixels += a_lChannels;
		}
		while (--a_lNumPixels);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cDX11Bitmap2D::cDX11Bitmap2D() : iBitmap2D("DX")
	{
		m_lHeight = 0;
		m_lWidth = 0;

		m_pPixels = NULL;
		m_lWidth = 0;
		m_lHeight = 0;
		m_lDepth = 0;
		m_lArraySize = 0;
	}

	cDX11Bitmap2D::cDX11Bitmap2D(tString a_sType) : iBitmap2D("DX")
	{
		
	}

	cDX11Bitmap2D::~cDX11Bitmap2D()
	{
		delete[] m_pPixels;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cDX11Bitmap2D::SaveToFile(const tString &a_sFile)
	{
		return false;
	}

	//--------------------------------------------------------------

	void cDX11Bitmap2D::DrawToBitmap(iBitmap2D *a_pDest, const cVector2l &a_vDestPos)
	{
		cDX11Bitmap2D *pDest = static_cast<cDX11Bitmap2D*>(a_pDest);

		if (m_Format != eFormat_RGB8 && m_Format != eFormat_RGBA8)
			return;
		else if (m_Format == eFormat_RGB8)
			Convert(eFormat_RGBA8);

		int lDestW = pDest->GetWidth();
		int lDestH = pDest->GetHeight();

		int lDummy = lDestW * a_vDestPos.y + a_vDestPos.x;
		int lStart = lDummy>=0 ? lDummy : 0;
		lDummy = lStart + (lDestW * (m_lHeight - 1)) + m_lWidth;
		int lEnd = lDummy > lDestW*lDestH ? lDestW*lDestH : lDummy;

		int j = 0;

		int lOffset =  a_vDestPos.x;
		int lReach = a_vDestPos.x + m_lWidth;
		int lFiller  = lDestW - lReach + a_vDestPos.x - 1;

		unsigned int *pDestPxls = (unsigned int *)pDest->GetPixels();
		unsigned int *pSrcPxls = (unsigned int *)m_pPixels;

		for (int i = lStart; i < lEnd; i++)
		{
			int lMod = i%lDestW;
			if (lMod >= lOffset && lMod < lReach)
			{
				pDestPxls[i] = pSrcPxls[j];
				j++;
			}
			else
				i += lFiller;
		}

		//pDest->SaveBmp("Blitted.bmp");
	}

	//--------------------------------------------------------------

	void cDX11Bitmap2D::FillRect(const cRectl &a_Rect, const cColor &a_Color)
	{
		if (m_Format != eFormat_RGBA8)
			return;

		cRectl rect;
		rect.x = a_Rect.x<=0?0:a_Rect.x;
		rect.y = a_Rect.y<=0?0:a_Rect.y;
		rect.w = a_Rect.w<=0?m_lWidth:a_Rect.w;
		rect.h = a_Rect.h<=0?m_lHeight:a_Rect.h;

		char color[4] = {(char)(a_Color.r*255.0f), (char)(a_Color.g*255.0f), (char)(a_Color.b*255.0f), (char)(a_Color.r*255.0f)};
		unsigned int lColor = *reinterpret_cast<unsigned*>(color);

		int lStart = m_lWidth * rect.y + rect.x;
		int lEnd = lStart + (m_lWidth * (rect.h - 1)) + rect.w;

		int lOffset = rect.x;
		int lReach = rect.x + rect.w;

		unsigned int *pPixels = (unsigned int*)m_pPixels;

		for (int i = lStart;i < lEnd; i++)
		{
			int lMod = i%m_lWidth;
			if (lMod >= lOffset && lMod < lReach)
				pPixels[i] = lColor;
		}
		
		//SaveBmp("Filled.bmp");
	}

	//--------------------------------------------------------------

	bool cDX11Bitmap2D::HasAlpha()
	{
		return true;
	}

	//--------------------------------------------------------------

	bool cDX11Bitmap2D::Create(cVector2l a_vSize, eFormat a_Format)
	{
		//m_lHeight = a_vSize.x;
		//m_lWidth = a_vSize.y;
		//m_Format = a_Format;

		m_pPixels = Create(a_Format, a_vSize.x, a_vSize.y, 1, 1);

		return true;
	}

	//--------------------------------------------------------------

	int cDX11Bitmap2D::GetNumChannels()
	{
		return 4;//m_pSurface->
	}

	//--------------------------------------------------------------

	bool cDX11Bitmap2D::SaveBmp(const tString &a_sFileName)
	{
		if (m_Format != eFormat_I8 && m_Format != eFormat_RGB8 && m_Format != eFormat_RGBA8)
			return false;

		FILE *pFile = fopen(a_sFileName.c_str(), "wb");

		int lChannelsNum = GetChannelCount(m_Format);
		cBMPHeader header = 
		{
			MCHAR2('B','M'),
			{0x36, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00},
			m_lWidth,
			m_lHeight,
			0x0001,
			lChannelsNum * 8,
			0,
			{0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00}
		};

		fwrite(&header, sizeof(header), 1, pFile);

		if (m_Format == eFormat_I8)
		{
		
		}
		else
		{
			bool bUseAlpha = (lChannelsNum == 4);
			int lPadding = (m_lWidth * lChannelsNum) & 3;
			if (lPadding) lPadding = 4 - lPadding;

			int lLength, lSize = (m_lWidth * lChannelsNum + lPadding) * m_lHeight;
			unsigned char *pDest, *pSrc, *pBuffer = new unsigned char[lSize];

			for (int y=0;y < m_lHeight;y++)
			{
				pDest = pBuffer + (m_lHeight - y - 1) * (m_lWidth * lChannelsNum + lPadding);
				pSrc = m_pPixels + y * m_lWidth * lChannelsNum;
				lLength = m_lWidth;
				do
				{
					*pDest++ = pSrc[2];
					*pDest++ = pSrc[1];
					*pDest++ = pSrc[0];
					if (bUseAlpha) *pDest++ = pSrc[3];
					pSrc += lChannelsNum;
				}
				while(--lLength);
			}
			fwrite(pBuffer, lSize, 1, pFile);
			efeDeleteArray(pBuffer);
		}

		fclose(pFile);

		return true;
	}

	//--------------------------------------------------------------

	bool cDX11Bitmap2D::Convert(eFormat a_NewFmt)
	{
		unsigned char *pNewPixels = NULL;
		unsigned int lPixelsNum = GetPixelCount(0, m_lMipMapsNum);

		if (m_Format == eFormat_RGBE8 && (a_NewFmt == eFormat_RGB32F || a_NewFmt == eFormat_RGBA32F))
		{
			pNewPixels = new ubyte[GetMipMappedSize(0, m_lMipMapsNum, a_NewFmt) * m_lArraySize];
			float *pDest = (float *)pNewPixels;

			bool bWriteAlpha = (a_NewFmt == eFormat_RGBA32F);
			ubyte *pSrc = m_pPixels;
			do
			{
				*((cVector3f *)pDest) = cMath::RGBEToVector3f(pSrc);
				if (bWriteAlpha)
				{
					pDest[3] = 1.0f;
					pDest += 4;
				}
				else pDest += 3;

				pSrc += 4;
			}
			while (--lPixelsNum);
		}
		else
		{
			if (!IsPlainFormat(m_Format) || (!IsPlainFormat(a_NewFmt) || a_NewFmt == eFormat_RGB10A2 || a_NewFmt == eFormat_RGBE8 || a_NewFmt == eFormat_RGB9E5)) return false;
			if (m_Format == a_NewFmt) return true;

			ubyte *pSrc = m_pPixels;
			ubyte *pDest = pNewPixels = new ubyte[GetMipMappedSize(0, m_lMipMapsNum, a_NewFmt) * m_lArraySize];
			
			if (m_Format == eFormat_RGB8 && a_NewFmt == eFormat_RGBA8)
			{
				do
				{
					pDest[0] = pSrc[0];
					pDest[1] = pSrc[1];
					pDest[2] = pSrc[2];
					pDest[3] = 255;
					pDest += 4;
					pSrc += 3;
				}
				while (--lPixelsNum);
			}
			else
			{
				int lSrcSize = GetBytesPerPixel(m_Format);
				int lSrcChannelsNum = GetChannelCount(m_Format);

				int lDestSize = GetBytesPerPixel(a_NewFmt);
				int lDestChannelsNum = GetChannelCount(a_NewFmt);

				do
				{
					float vRGBA[4];

					if (IsPlainFormat(m_Format))
					{
						if (m_Format <= eFormat_RGBA16F)
							for (int i = 0;i < lSrcChannelsNum;i++) vRGBA[i] = ((short *)pSrc)[i];
						else
							for (int i = 0;i < lSrcChannelsNum;i++) vRGBA[i] = ((float *)pSrc)[i];
					}
					else if (m_Format >= eFormat_I16 && m_Format <= eFormat_RGBA16)
						for (int i = 0;i < lSrcChannelsNum;i++) vRGBA[i] = ((unsigned short *)pSrc)[i] * (1.0f / 65535.0f);
					else
						for (int i = 0;i < lSrcChannelsNum;i++) vRGBA[i] = pSrc[i] * (1.0f / 255.0f);

					if (lSrcChannelsNum < 4) vRGBA[3] = 1.0f;
					if (lSrcChannelsNum == 1) vRGBA[2] = vRGBA[1] = vRGBA[0];

					if (lDestChannelsNum == 1) vRGBA[0] = 0.30f * vRGBA[0] + 0.59f * vRGBA[1] + 0.11f * vRGBA[2];

					if (IsFloatFormat(a_NewFmt))
					{
						if (a_NewFmt <= eFormat_RGBA32F)
						{
							if (a_NewFmt <= eFormat_RGBA16F)
								for (int i = 0;i < lDestChannelsNum;i++) ((short *)pDest)[i] = vRGBA[i];
							else
								for (int i = 0;i < lDestChannelsNum;i++) ((float *)pDest)[i] = vRGBA[i];
						}
						else
						{
							if (a_NewFmt == eFormat_RGBE8)
								*(uint32 *)pDest = RGBToRGBE8(cVector3f(vRGBA[0], vRGBA[1], vRGBA[2]));
							else
								*(uint32 *)pDest = RGBToRGB9E5(cVector3f(vRGBA[0], vRGBA[1], vRGBA[2]));
						}
					}
					else if (a_NewFmt >= eFormat_I16 && a_NewFmt <= eFormat_RGBA16)
						for (int i = 0;i < lDestChannelsNum;i++) ((unsigned short *)pDest)[i] = (unsigned short)(65535 * cMath::Saturate(vRGBA[i]) + 0.5f); 
					else if (a_NewFmt == eFormat_RGB10A2)
					{
						*(unsigned int *)pDest = 
							(unsigned int(1023.0f * cMath::Saturate(vRGBA[0]) + 0.5f) << 22) | 
							(unsigned int(1023.0f * cMath::Saturate(vRGBA[1]) + 0.5f) << 12) | 
							(unsigned int(1023.0f * cMath::Saturate(vRGBA[2]) + 0.5f) << 2)	 | 
							(unsigned int(3.0f * cMath::Saturate(vRGBA[3]) + 0.5f)); 
					}
					else
						for (int i = 0; i < lDestChannelsNum; i++) pDest[i] = (unsigned char)(255 * cMath::Saturate(vRGBA[i]) + 0.5f);

					pSrc += lSrcSize;
					pDest += lDestSize;
				}
				while (--lPixelsNum);
			}
		}
		delete[] m_pPixels;
		m_pPixels = pNewPixels;
		m_Format = a_NewFmt;

		

		return true;
	}

	//--------------------------------------------------------------

	unsigned char *cDX11Bitmap2D::Create(const eFormat a_Fmt, const int a_lW, const int a_lH, const int a_lD,const int a_lMipMapCount, const int a_lArraySize)
	{
		m_Format = a_Fmt;
		m_lWidth = a_lW;
		m_lHeight = a_lH;
		m_lDepth = a_lD;
		m_lMipMapsNum = a_lMipMapCount;
		m_lArraySize = a_lArraySize;
		int lSize = GetMipMappedSize(0, m_lMipMapsNum);
		return (m_pPixels = new unsigned char[GetMipMappedSize(0, m_lMipMapsNum) * m_lArraySize]);
	}

	//--------------------------------------------------------------

	void cDX11Bitmap2D::CreateFromBitmapVec(tBitmap2DVec *a_pBitmaps)
	{
		int lMaxImages = a_pBitmaps->size();
		iBitmap2D *pBmp = a_pBitmaps->at(0);
		int lMipMaps = 1;
		ubyte *pDest = Create(pBmp->GetFormat(), pBmp->GetWidth(), pBmp->GetHeight(), 0, lMipMaps, 1);

		for (int i = 0; i < lMaxImages; i++)
		{
			cDX11Bitmap2D *pSlice = static_cast<cDX11Bitmap2D*>(a_pBitmaps->at(i));
			int lSize = pSlice->GetMipMappedSize(0, 1);
			memcpy(pDest, pSlice->GetPixels(0), lSize);
			pDest += lSize;
		}
	}

	//--------------------------------------------------------------

	void cDX11Bitmap2D::Free()
	{
		delete[] m_pPixels;
		m_pPixels = NULL;
	}

	//--------------------------------------------------------------

	void cDX11Bitmap2D::Clear()
	{
		Free();

		m_lWidth = 0;
		m_lHeight = 0;
		m_lDepth = 0;
		m_lMipMapsNum = 0;
		m_Format = eFormat_None;
	}

	//--------------------------------------------------------------

	unsigned char *cDX11Bitmap2D::GetPixels(const int a_lMipMapLevel)const
	{
		return (a_lMipMapLevel < m_lMipMapsNum) ? m_pPixels + GetMipMappedSize(0, a_lMipMapLevel) : NULL;
	}

	//--------------------------------------------------------------

	unsigned char *cDX11Bitmap2D::GetPixels(const int a_lMipMapLevel, const int a_lArraySlice)const
	{
		if (a_lMipMapLevel >= m_lMipMapsNum || a_lArraySlice >= m_lArraySize) return NULL;

		return m_pPixels + GetMipMappedSize(0, m_lMipMapsNum) * a_lArraySlice + GetMipMappedSize(0, a_lMipMapLevel);
	}

	//--------------------------------------------------------------

	int cDX11Bitmap2D::GetMipMappedSize(const int a_lFirstMipLevel, int a_lMipMapLevels, eFormat a_SrcFmt) const
	{
		int lW = GetWidth(a_lFirstMipLevel);
		int lH = GetHeight(a_lFirstMipLevel);
		int lD = GetDepth(a_lFirstMipLevel);

		if (a_SrcFmt == eFormat_None)
			a_SrcFmt = m_Format;

		int lSize = 0;
		while (a_lMipMapLevels)
		{
			lSize += lW * lH * lD;
			
			lW >>= 1;
			lH >>= 1;
			lD >>= 1;
			if (lW + lH + lD == 0) break;
			if (lW == 0) break;
			if (lH == 0) break;
			if (lD == 0) break;
			a_lMipMapLevels--;
		}

		lSize *= GetBytesPerPixel(a_SrcFmt);

		return (m_lDepth==0)? 6 * lSize : lSize;
	}

	//--------------------------------------------------------------

	unsigned int cDX11Bitmap2D::GetPixelCount(const int a_lFirstMipMapLevel, int a_lMipMapLevelsNum)const
	{
		int lW = GetWidth(a_lFirstMipMapLevel);
		int lH = GetHeight(a_lFirstMipMapLevel);
		int lD = GetDepth(a_lFirstMipMapLevel);
		int lSize = 0;
		while(a_lMipMapLevelsNum)
		{
			lSize += lW * lH * lD;
			lW >> 1;
			lH >> 1;
			lD >> 1;
			if (lW + lH + lD == 0) break;
			if (lW == 0) lW = 1;
			if (lH == 0) lH = 1;
			if (lD == 0) lD = 1;

			a_lMipMapLevelsNum--;
		}

		return (m_lDepth==0)? 6 * lSize : lSize;
	}

	//--------------------------------------------------------------

	int cDX11Bitmap2D::GetWidth(const int a_lMipMapLevel) const
	{
		int lA = m_lWidth >> a_lMipMapLevel;
		return (lA==0) ? 1 : lA;
	}

	//--------------------------------------------------------------

	int cDX11Bitmap2D::GetHeight(const int a_lMipMapLevel) const
	{
		int lA = m_lHeight >> a_lMipMapLevel;
		return (lA==0) ? 1 : lA;
	}

	//--------------------------------------------------------------

	int cDX11Bitmap2D::GetDepth(const int a_lMipMapLevel) const
	{
		int lA = m_lDepth >> m_lMipMapsNum;
		return (lA==0) ? 1 : lA;
	}

	//--------------------------------------------------------------

	bool cDX11Bitmap2D::LoadJpeg(const char *a_sFilePath)
	{
		return false;
		//jpeg_decompress_struct cinfo;
	}

	//--------------------------------------------------------------

	bool cDX11Bitmap2D::LoadBmp(const char *a_sFilePath)
	{
		cBMPHeader header;

		FILE *pFile;
		unsigned char *pDest, *pSrc, *pTmp;
		int lLen;
		unsigned char vPalette[1024];

		if ((pFile=fopen(a_sFilePath, "rb"))==NULL)return false;

		fread(&header, sizeof(header), 1, pFile);
		if (header.bmpIdentifier != MCHAR2('B', 'M'))
		{
			fclose(pFile);
			return false;
		}

		m_lWidth = header.width;
		m_lHeight = header.height;
		m_lDepth = 1;
		m_lMipMapsNum = 1;
		m_lArraySize = 1;

		switch(header.bpp)
		{
		case 8:
			if (header.compression)
			{
				fclose(pFile);
				return false;
			}

			m_Format = eFormat_RGB8;
			m_pPixels = new unsigned char[m_lWidth*m_lHeight*3];

			fread(vPalette, sizeof(vPalette), 1, pFile);
			SwapChannels(vPalette, 256, 4, 0, 2);

			pTmp = new unsigned char[m_lWidth];
			for (int i = m_lHeight-1;i >= 0; i--)
			{
				pDest = m_pPixels + i * m_lWidth * 3;
				fread(pTmp, m_lWidth, 1, pFile);
				lLen = m_lWidth;
				do
				{
					pSrc = vPalette + ((*pTmp++) << 2);
					*pDest++ = *pSrc++;
					*pDest++ = *pSrc++;
					*pDest++ = *pSrc++;
				}while (--lLen);
				pTmp -= m_lWidth;
			}

			delete[] pTmp;
			break;
		case 24:
		case 32:
			int lChannelsNum;
			lChannelsNum = (header.bpp == 24)?3 : 4;
			int lPadding;
			lPadding = (m_lWidth * lChannelsNum) & 3;
			if (lPadding) lPadding = 4 - lPadding;
			m_Format = (header.bpp == 24)? eFormat_RGB8 : eFormat_RGBA8;
			m_pPixels = new unsigned char[m_lWidth * m_lHeight * lChannelsNum];
			for (int i = m_lHeight-1; i >= 0; i--)
			{
				pDest = m_pPixels + i * m_lWidth * lChannelsNum;
				fread(pDest, m_lWidth * lChannelsNum, 1, pFile);
				fseek(pFile, lPadding, SEEK_CUR);
				SwapChannels(pDest, m_lWidth, lChannelsNum, 0, 2);
			}
			break;
		default:
			fclose(pFile);
			return false;
		}

		fclose(pFile);

		return true;
	}

	//--------------------------------------------------------------
}