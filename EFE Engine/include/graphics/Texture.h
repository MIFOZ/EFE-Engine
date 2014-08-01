#ifndef EFE_TEXTURE_H
#define EFE_TEXTURE_H

#include <vector>
#include "graphics/LowLevelPicture.h"
#include "graphics/Bitmap2D.h"

#include "Graphics/GraphicsTypes.h"
#include "resources/ResourceBase.h"

namespace efe
{
	enum eTextureType
	{
		eTextureType_Normal,
		eTextureType_RenderTarget,
		eTextureType_RenderDepth,
		eTextureType_LastEnum
	};

	enum eTextureTarget
	{
		eTextureTarget_1D,
		eTextureTarget_2D,
		eTextureTarget_Rect,
		eTextureTarget_CubeMap,
		eTextureTarget_3D,
		eTextureTarget_LastEnum
	};

//#define GetString(var) return tString(#var)

	enum eTextureAddressMode
	{
		eTextureAddressMode_Wrap,
		eTextureAddressMode_Clamp,
		eTextureAddressMode_Mirror,
		eTextureAddressMode_Border,
		eTextureAddressMode_LastEnum
	};

	enum eTextureFilter
	{
		eTextureFilter_Nearest,
		eTextureFilter_Linear,
		eTextureFilter_Bilinear,
		eTextureFilter_Trilinear,
		eTextureFilter_BilinearAniso,
		eTextureFilter_TrilinearAniso,
		eTextureFilter_LastEnum
	};

	enum eTextureAnimMode
	{
		eTextureAnimMode_None,
		eTextureAnimMode_Loop,
		eTextureAnimMode_Oscillate,
		eTextureAnimMode_LastEnum
	};

	class iLowLevelGraphics;

	class iTexture : public iLowLevelPicture, public iResourceBase
	{
	public:
		iTexture(tString a_sName, tString a_sType, iLowLevelGraphics *a_pLowLevelGraphics,
			eTextureType a_Type, bool a_bUseMipMaps, eTextureTarget a_Target, eFormat a_Format = eFormat_None,
			bool a_bSRGB=false)
			: iLowLevelPicture(a_sType), iResourceBase(a_sName, 0),
			m_Type(a_Type), m_bUseMipMaps(a_bUseMipMaps),
			m_pLowLevelGraphics(a_pLowLevelGraphics)/*, m_bCompress(a_bCompress)*/,m_bSRGB(a_bSRGB),
			m_Target(a_Target),
			m_WrapS(eTextureAddressMode_Wrap), m_WrapT(eTextureAddressMode_Wrap), m_WrapR(eTextureAddressMode_Wrap),
			m_fFrameTime(1), m_AnimMode(eTextureAnimMode_Loop), m_lSizeLevel(0), m_vMinLevelSize(16,16),
			m_fAnisotropyDegree(1.0f), m_Filter(eTextureFilter_Bilinear){}

		virtual ~iTexture(){}

		void SetSizeLevel(unsigned int a_lLevel){m_lSizeLevel = a_lLevel;}
		void SetMinLevelSize(const cVector2l &a_vSize){m_vMinLevelSize = a_vSize;}

		bool Reload(){return false;}
		void Unload(){}
		void Destroy(){}

		virtual bool CreateFromBitmap(iBitmap2D *a_pBmp)=0;
		/*virtual bool CreateRenderTarget(const int a_lWidth, const int a_lHeight, const int a_lDepth, const int a_lMipMapCount,
											const int a_lArraySize, bool a_bSRGB, const int a_lMultisampling);*/

		virtual bool CreateCubeMapFromBitmapVec(tBitmap2DVec *a_vBitmaps)=0;

		virtual void SetPixels2D(int a_lLevel, const cVector2l &a_vOffset, const cVector2l &a_vSize,
			eFormat a_Format, void *a_pPixelData)=0;

		virtual bool HasAlpha(){return false;};

		//virtual void SetWrapS(eTextureWrap a_Mode)=0;
		//virtual void SetWrapT(eTextureWrap a_Mode)=0;
		//virtual void SetWrapR(eTextureWrap a_Mode)=0;

		eTextureAddressMode GetWrapS(){return m_WrapS;}
		eTextureAddressMode GetWrapT(){return m_WrapT;}
		eTextureAddressMode GetWrapR(){return m_WrapR;}

		void SetFrameTime(float a_fX){m_fFrameTime = a_fX;}

		void SetAnimMode(eTextureAnimMode a_Mode){m_AnimMode = a_Mode;}

		eTextureTarget GetTarget(){return m_Target;}
	protected:
		eTextureType m_Type;
		eTextureTarget m_Target;
		eTextureAddressMode m_WrapS;
		eTextureAddressMode m_WrapT;
		eTextureAddressMode m_WrapR;
		eTextureFilter m_Filter;
		float m_fAnisotropyDegree;

		bool m_bUseMipMaps;
		bool m_bCompress;
		bool m_bSRGB;
		iLowLevelGraphics *m_pLowLevelGraphics;
		float m_fFrameTime;
		eTextureAnimMode m_AnimMode;
		unsigned int m_lSizeLevel;
		cVector2l m_vMinLevelSize;
	};

	typedef std::vector<iTexture*> tTextureVec;
	typedef tTextureVec::iterator tTextureVecIt;
};
#endif