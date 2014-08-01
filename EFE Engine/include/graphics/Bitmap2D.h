#ifndef EFE_BITMAP2D_H
#define EFE_BITMAP2D_H

#include "graphics/LowLevelPicture.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "math/Point.h"

#include "impl/Platform.h"

namespace efe
{
#define ALL_MIPMAPS 127

	

#define eFormat_I8		eFormat_R8
#define eFormat_IA8		eFormat_RG8
#define eFormat_I16		eFormat_R16
#define eFormat_IA16	eFormat_RG16
#define eFormat_I16F	eFormat_R16F
#define eFormat_IA16F	eFormat_RG16F
#define eFormat_I32F	eFormat_R32F
#define eFormat_IA32F	eFormat_RG32F

	class iBitmap2D : public iLowLevelPicture
	{
	public:
		iBitmap2D(tString a_sType) : iLowLevelPicture(a_sType){}
		virtual ~iBitmap2D(){}

		virtual bool SaveToFile(const tString &a_sFile)=0;

		virtual void DrawToBitmap(iBitmap2D *a_pBmp, const cVector2l &a_vPos)=0;

		virtual void FillRect(const cRectl &a_Rect, const cColor &a_Color)=0;

		virtual unsigned char *GetPixels()const=0;
		virtual unsigned char *GetPixels(const int a_lMipMapLevel)const=0;
		virtual unsigned char *GetPixels(const int a_lMipMapLevel, const int a_lArraySlice)const=0;

		virtual int GetNumChannels()=0;
	};

	typedef std::vector<iBitmap2D*> tBitmap2DVec;
	typedef tBitmap2DVec::iterator tBitmap2DVecIt;
};
#endif