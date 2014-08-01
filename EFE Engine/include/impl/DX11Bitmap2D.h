#ifndef EFE_DX_BITAMP2D_H
#define EFE_DX_BITAMP2D_H

#include "graphics/Bitmap2D.h"

#include "math/Math.h"
#include "math/Vector3.h"

namespace efe
{
	//unsigned int RGBToRGBE8(const cVector3f &a_vRGB);

	//unsigned int RGBToRGB9E5(const cVector3f &a_vRGB)
	
	inline bool IsPlainFormat(const eFormat a_Fmt)
	{
		return (a_Fmt <= eFormat_RGBA32UI);
	}

	inline bool IsCompressedFormat(const eFormat a_Fmt)
	{
		return (a_Fmt >= eFormat_DXT1) && (a_Fmt <= eFormat_ATI2N);
	}

	inline bool IsFloatFormat(const eFormat a_Fmt)
	{
		return (a_Fmt >= eFormat_R16F && a_Fmt <= eFormat_RG11B10F) || (a_Fmt == eFormat_D32F);
	}

	inline int GetChannelCount(const eFormat a_Format)
	{
		static const int chCount[] = 
		{
			0,
			1, 2, 3, 4,
			1, 2, 3, 4,
			1, 2, 3, 4,
			1, 2, 3, 4,
			1, 2, 3, 4,
			1, 2, 3, 4,
			1, 2, 3, 4,
			1, 2, 3, 4,
			1, 2, 3, 4,
			1, 2, 3, 4,
			3, 3, 3, 3, 4, 4,
			1, 1, 2, 1,
			3, 4, 4, 1, 2
		};
		return chCount[a_Format];
	}

	inline int GetBytesPerPixel(const eFormat a_Format)
	{
		static const int bytesPP[] = 
		{
			0,
			1, 2, 3, 4,
			2, 4, 6, 8,
			1, 2, 3, 4,
			2, 4, 6, 8,
			2, 4, 6, 8,
			4, 8, 12, 16,
			2, 4, 6, 8,
			4, 8, 12, 16,
			2, 4, 6, 8,
			4, 8, 12, 16,
			4, 4, 4, 2, 2, 4,
			2, 4, 4, 4
		};
		return bytesPP[a_Format];
	}

	inline int GetBytesPerBlock(const eFormat a_Format)
	{
		return (a_Format == eFormat_DXT1 || a_Format == eFormat_ATI1N) ? 8 : 16;
	}


	/*extern cSurface *LoadBmp(const char *a_sFile);
	extern bool BlitSurface(cSurface *a_pSrc, cSurface *a_pDest, POINT &a_DestPt);
	extern void _FillRect(cSurface *a_pDest,const cRectl *a_DestRect,const cColor &a_Color);*/

	class cDX11Bitmap2D : public iBitmap2D
	{
	public:
		cDX11Bitmap2D();
		cDX11Bitmap2D(tString a_sType);
		~cDX11Bitmap2D();

		bool SaveToFile(const tString &a_sFile);

		void DrawToBitmap(iBitmap2D *a_pBmp, const cVector2l &a_vDestPos);
		bool HasAlpha();

		bool Create(cVector2l a_vSize, eFormat a_Format);

		void CreateFromBitmapVec(tBitmap2DVec *a_pBitmaps);

		void FillRect(const cRectl &a_Rect, const cColor &a_Color);

		int GetNumChannels();

		//My Impl

		unsigned char *Create(const eFormat a_Fmt, const int a_lW, const int a_lH, const int a_lD, 
								const int a_lMipMapCount, const int a_lArraySize = 1);
		void Free();
		void Clear();

		unsigned char *GetPixels()const{return m_pPixels;}
		unsigned char *GetPixels(const int a_lMipMapLevel)const;
		unsigned char *GetPixels(const int a_lMipMapLevel, const int a_lArraySlice)const;
		
		int GetMipMapsCount()const{return m_lMipMapsNum;}
		int GetMipMappedSize(const int a_lFirstMipLevel=0, int a_lMipMapLevels = ALL_MIPMAPS, eFormat a_SrcFmt = eFormat_None) const;
		unsigned int GetPixelCount(const int a_lFirstMipMapLevel = 0, int a_lMipMapLevelsNum = ALL_MIPMAPS)const;

		int GetWidth()const{return m_lWidth;}
		int GetHeight()const{return m_lHeight;}
		int GetDepth()const{return m_lDepth;}
		int GetWidth(const int a_lMipMapLevel) const;
		int GetHeight(const int a_lMipMapLevel) const;
		int GetDepth(const int a_lMipMapLevel) const;
		int GetArraySize()const{return m_lArraySize;}

		bool Is1D() const {return m_lDepth == 1 && m_lHeight == 1;}
		bool Is2D() const {return m_lDepth == 1 && m_lHeight > 1;}
		bool Is3D() const {return m_lDepth > 1;}
		bool IsCube()const {return m_lDepth == 0;}

		eFormat GetFormat()const{return m_Format;}

		bool LoadJpeg(const char *a_sFilePath);
		bool LoadBmp(const char *a_sFilePath);

		bool SaveBmp(const tString &a_sFileName);

		bool Convert(const eFormat a_NewFmt);

		tString m_sType;
	private:
		unsigned char *m_pPixels;
		int m_lMipMapsNum;

		int m_lArraySize;
	};
};
#endif