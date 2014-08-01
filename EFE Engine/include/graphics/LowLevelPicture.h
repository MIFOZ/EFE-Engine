#ifndef EFE_LOWLEVEL_PICTURE_H
#define EFE_LOWLEVEL_PICTURE_H

#include "system/SystemTypes.h"
#include "system/String.h"

namespace efe
{
	enum eFormat
	{
		eFormat_None		= 0,

		//Unsigned formats
		eFormat_R8			= 1,
		eFormat_RG8			= 2,
		eFormat_RGB8		= 3,
		eFormat_RGBA8		= 4,

		eFormat_R16			= 5,
		eFormat_RG16		= 6,
		eFormat_RGB16		= 7,
		eFormat_RGBA16		= 8,

		//Signed formats
		eFormat_R8S			= 9,
		eFormat_RG8S		= 10,
		eFormat_RGB8S		= 11,
		eFormat_RGBA8S		= 12,

		eFormat_R16S		= 13,
		eFormat_RG16S		= 14,
		eFormat_RGB16S		= 15,
		eFormat_RGBA16S		= 16,

		//Float formats
		eFormat_R16F		= 17,
		eFormat_RG16F		= 18,
		eFormat_RGB16F		= 19,
		eFormat_RGBA16F		= 20,

		eFormat_R32F		= 21,
		eFormat_RG32F		= 22,
		eFormat_RGB32F		= 23,
		eFormat_RGBA32F		= 24,

		//Signed integer formats
		eFormat_R16I		= 25,
		eFormat_RG16I		= 26,
		eFormat_RGB16I		= 27,
		eFormat_RGBA16I		= 28,

		eFormat_R32I		= 29,
		eFormat_RG32I		= 30,
		eFormat_RGB32I		= 31,
		eFormat_RGBA32I		= 32,

		//Unsigned integer formats
		eFormat_R16UI		= 33,
		eFormat_RG16UI		= 34,
		eFormat_RGB16UI		= 35,
		eFormat_RGBA16UI	= 36,

		eFormat_R32UI		= 37,
		eFormat_RG32UI		= 38,
		eFormat_RGB32UI		= 39,
		eFormat_RGBA32UI	= 40,

		//Packed formats
		eFormat_RGBE8		= 41,
		eFormat_RGB9E5		= 42,
		eFormat_RG11B10F	= 43,
		eFormat_RGB565		= 44,
		eFormat_RGBA4		= 45,
		eFormat_RGB10A2		= 46,

		//Depth formats
		eFormat_D16			= 47,
		eFormat_D24			= 48,
		eFormat_D24S8		= 49,
		eFormat_D32F		= 50,

		//Compressed formats
		eFormat_DXT1		= 51,
		eFormat_DXT3		= 52,
		eFormat_DXT5		= 53,
		eFormat_ATI1N		= 54,
		eFormat_ATI2N		= 55,
	};

	class iLowLevelPicture
	{
	public:
		iLowLevelPicture(tString a_sType) : m_sType(a_sType){}
		virtual ~iLowLevelPicture(){}

		tString GetType(){return m_sType;}

		unsigned int GetHeight(){return m_lHeight;}
		unsigned int GetWidth(){return m_lWidth;}
		unsigned int GetDepth(){return m_lDepth;}

		unsigned int GetBpp(){return m_lBpp;}

		eFormat GetFormat(){return m_Format;}

		virtual bool HasAlpha()=0;

		void SetPath(tString a_sPath){m_sPath = a_sPath;}
		tString GetPath(){return m_sPath;}
		tString GetFileName(){return cString::GetFileName(m_sPath);}
	protected:
		unsigned int m_lHeight;
		unsigned int m_lWidth;
		unsigned int m_lDepth;
		unsigned int m_lBpp;
		eFormat m_Format;
	private:
		tString m_sType;
		tString m_sPath;
	};
};
#endif