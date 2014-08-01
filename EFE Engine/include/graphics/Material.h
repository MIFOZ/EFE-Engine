#ifndef EFE_MATERIAL_H
#define EFE_MATERIAL_H

#include <vector>
#include "graphics/Texture.h"
#include "system/SystemTypes.h"
#include "resources/ResourceImage.h"
#include "resources/ResourceBase.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/GPUProgram.h"
#include "graphics/RenderTask.h"

class TiXmlElement;

namespace efe
{
	#define kMaxProgramNum (5)

	enum eMaterialQuality
	{
		eMaterialQuality_VeryLow = 0,
		eMaterialQuality_Low = 1,
		eMaterialQuality_Medium = 2,
		eMaterialQuality_High = 3,
		eMaterialQuality_VeryHigh = 4,
		eMaterialQuality_LastEnum
	};

	enum eMaterialTexture
	{
		eMaterialTexture_Diffuse,
		eMaterialTexture_NMap,
		eMaterialTexture_Specular,
		eMaterialTexture_Alpha,
		eMaterialTexture_Illumination,
		eMaterialTexture_CubeMap,
		eMaterialTexture_Refraction,
		eMaterialTexture_LastEnum
	};

	enum eMaterialType
	{
		eMaterialType_Null,

		eMaterialType_Diffuse,
		eMaterialType_DiffuseAlpha,
		eMaterialType_DiffuseAddictive,
		eMaterialType_DiffuseMap,
		eMaterialType_DiffuseSpecular,
		eMaterialType_BumpSpec,
		eMaterialType_Smoke,

		eMaterialType_FontNormal,

		eMaterialType_LastEnum
	};

	enum eMaterialPicture
	{
		eMaterialPicture_Image,
		eMaterialPicture_Texture,
		eMaterialPicture_LastEnum
	};

// 	enum eMaterialCullMode
// 	{
// 		eMaterialCullMode_Solid,
// 		eMaterialCullMode_Wireframe
// 	};

	enum eMaterialFillMode
	{
		eMaterialFillMode_Solid,
		eMaterialFillMode_Wireframe
	};

	enum eMaterialBlendMode
	{
		eMaterialBlendMode_None,
		eMaterialBlendMode_Add,
		eMaterialBlendMode_Mul,
		eMaterialBlendMode_MulX2,
		eMaterialBlendMode_Replace,
		eMaterialBlendMode_Alpha,
		eMaterialBlendMode_DestAlphaAdd,
		eMaterialBlendMode_LastEnum
	};

	enum eMaterialRenderType
	{
		eMaterialRenderType_Perspective,
		eMaterialRenderType_LinearDepthNormal,
		eMaterialRenderType_DualParaboloidEnvMap,
		eMaterialRenderType_GeometricBuffer,
		eMaterialRenderType_Lights,
		eMaterialRenderType_FinalPass,
		eMaterialRenderType_LastEnum
	};

// 	enum eMaterialBlendMode
// 	{
// 		eMaterialBlendMode_None,
// 		eMaterialBlendMode_Add,
// 		eMaterialBlendMode_Mul,
// 		eMaterialBlendMode_MulX2,
// 		eMaterialBlendMode_Replace,
// 		eMaterialBlendMode_Alpha,
// 		eMaterialBlendMode_DestAlphaAdd,
// 		eMaterialBlendMode_LastEnum
// 	};

	enum eMaterialAlphaMode
	{
		eMaterialAlphaMode_Solid,
		eMaterialAlphaMode_Trans,
		eMaterialAlphaMode_LastEnum
	};

	enum eMaterialChannelMode
	{
		eMaterialChannelMode_RGBA,
		eMaterialChannelMode_RGB,
		eMaterialChannelMode_A,
		eMaterialChannelMode_Z,
		eMaterialChannelMode_LastEnum
	};

	class cRenderer2D;
	class cRenderer3D;
	class cRenderSettings;
	class cTextureManager;
	class cImageManager;
	class cGpuProgramManager;
	class iLight;
	class iCamera;
	class iLight3D;
	class cMaterialRenderEffect;

	struct cMaterialParameter
	{
		bool m_bRender;
		cMaterialRenderEffect *m_pEffect;
		std::vector<cRenderTask*> m_vTasks;;
	};

	struct cMaterialTextureParam
	{
		cMaterialTextureParam() : m_pTexture(NULL),
			m_WrapS(eTextureAddressMode_Wrap), m_WrapT(eTextureAddressMode_Wrap),m_WrapR(eTextureAddressMode_Wrap),
			m_Filter(eTextureFilter_Bilinear), m_fAnisotropyDegree(0.0f){}
		iTexture *m_pTexture;
		eTextureAddressMode m_WrapS;
		eTextureAddressMode m_WrapT;
		eTextureAddressMode m_WrapR;
		eTextureFilter m_Filter;
		float m_fAnisotropyDegree;
	};

	typedef std::vector<cMaterialTextureParam> tMaterialTextureParamVec;
	typedef tMaterialTextureParamVec::iterator tMaterialTextureParamVecIt;

	//--------------------------------------------------------------

	class iDXStateProgram : public iGpuProgram
	{
	public:
		iDXStateProgram(tString a_sName)
			: iGpuProgram(a_sName, eGpuProgramType_Pixel)
		{
			m_bSetUpDone = false;
		}
		virtual ~iDXStateProgram(){}

		void SetUp(iLowLevelGraphics *a_pLowLevelGraphics)
		{
			if (m_bSetUpDone == false)
			{
				m_pLowGfx = a_pLowLevelGraphics;
				m_bSetUpDone = true;
				InitData();
			}
		}
		virtual void Bind()=0;
		virtual void UnBind()=0;

		bool CreateFromFile(const tString &a_sFile, const tString &a_sEntry){return false;}
		bool SetFloat(const tString &a_sName, float a_fX){return false;}

		bool Reload(){return false;}
		void Unload(){}
		void Destroy(){}
	protected:
		iLowLevelGraphics *m_pLowGfx;

		bool m_bSetUpDone;

		virtual void InitData()=0;
	};

	//--------------------------------------------------------------

	class iMaterialProgramSetup
	{
	public:
		virtual void SetUp(iGpuProgram *a_pProgram, cRenderSettings *a_pRenderSettings)=0;
		virtual void SetupMatrix(cMatrixf *a_pModelMatrix, cRenderSettings *a_pRenderSettings)=0;
	};

	//--------------------------------------------------------------

	class cTextureType
	{
	public:
		cTextureType(tString a_sSuffix, eMaterialTexture a_Type) : m_sSuffix(a_sSuffix), m_Type(a_Type){}

		tString m_sSuffix;
		eMaterialTexture m_Type;
	};

	typedef std::list<cTextureType> tTextureTypeList;
	typedef tTextureTypeList::iterator tTextureTypeListIt;

	//--------------------------------------------------------------

	class iMaterial : public iResourceBase
	{
	public:
		iMaterial(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics,
			cImageManager *a_pImageManager, cTextureManager *a_pTextureManager,
			cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
			eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D);
		virtual ~iMaterial();

		bool Reload(){return false;}
		void Unload(){}
		void Destroy(){}

		virtual void Compile()=0;

		virtual void Update(float a_fTimeStep){};

		//The new render system stuff
		/*virtual */iGpuProgram *GetVertexProgram(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight);/*{return NULL;}*/
		virtual bool VertexProgramUsesLight(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight){return false;}
		virtual bool VertexProgramUsesEye(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight){return false;}
		virtual iMaterialProgramSetup *GetVertexProgramSetup(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight){return NULL;}

		/*virtual */iGpuProgram *GetPixelProgram(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight);/*{return NULL;}*/
		virtual iMaterialProgramSetup *GetPixelProgramSetup(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight){return NULL;}

		/*virtual */eMaterialAlphaMode GetAlphaMode(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight);/*{return eMaterialAlphaMode_Solid;}*/
		/*virtual */eMaterialBlendMode GetBlendMode(eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight);/*{return eMaterialBlendMode_Replace;}*/

		virtual iTexture *GetTexture(int a_lUnit, eMaterialRenderType a_Type, int a_lPass, iLight3D *a_pLight);
		cRectf GetTextureOffset(eMaterialTexture a_Type);

		virtual int GetNumOfPasses(eMaterialRenderType a_Type, iLight3D *a_pLight){return 0;}

		virtual bool UsesType(eMaterialRenderType a_Type){return false;}

		bool HasAlpha(){return m_bHasAlpha;}
		void SetHasAlpha(bool a_bX){m_bHasAlpha = a_bX;}

		bool GetDepthTest(){return m_bDepthTest;}
		void SetDepthTest(bool a_bX){m_bDepthTest = a_bX;}

		float GetValue(){return m_fValue;}
		void SetValue(float a_fX){m_fValue = a_fX;}

		virtual bool LoadData(TiXmlElement *a_pRootElem){return true;}

		virtual bool StartRendering(eMaterialRenderType a_Type, iCamera *a_pCam, iLight *a_pLight)=0;

		virtual eMaterialType GetType(eMaterialRenderType a_Type)=0;

		iTexture *GetTexture(eMaterialTexture a_Type);
		cMaterialTextureParam *GetTextureParamsPtr(eMaterialTexture a_Type);

		void SetTexture(iTexture *a_pTexture, eMaterialTexture a_Type){m_vTextureParams[a_Type].m_pTexture = a_pTexture;}
		void SetImage(cResourceImage *a_pImg, eMaterialTexture a_Type){m_vImage[a_Type] = a_pImg;}
		cResourceImage *GetImage(eMaterialTexture a_Type){return m_vImage[a_Type];}

		virtual bool IsTransparent(){return m_bIsTransparent;}
		virtual bool IsGlowing(){return m_bIsGlowing;}

		virtual tTextureTypeList GetTextureTypes()
		{
			tTextureTypeList vTypes;
			vTypes.push_back(cTextureType("",eMaterialTexture_Diffuse));
			return vTypes;
		}

		void SetId(int a_lId){m_lId = a_lId;}

		const tString &GetPhysicsMaterial(){return m_sPhysicsMaterial;}
		void SetPhysicsMaterial(const tString &a_sName){m_sPhysicsMaterial = a_sName;}

		static void SetQuality(eMaterialQuality a_Quality){m_Quality = a_Quality;}
		static eMaterialQuality GetQuality(){return m_Quality;}

		// New stuff
		tGpuProgramVec GetAllVertexProgramVec();
		cMaterialRenderEffect *GetRenderEffect(eMaterialRenderType a_Type) {return m_vParams[a_Type].m_pEffect;}
	protected:
		iLowLevelGraphics *m_pLowLevelGraphics;
		cImageManager *m_pImageManager;
		cTextureManager *m_pTextureManager;
		cRenderer2D *m_pRenderer;
		cRenderer3D *m_pRenderer3D;
		cRenderSettings *m_pRenderSettings;
		cGpuProgramManager *m_pProgramManager;

		static eMaterialQuality m_Quality;

		bool m_bIsTransparent;
		bool m_bIsGlowing;
		bool m_bUsesLights;
		bool m_bHasAlpha;
		bool m_bDepthTest;
		float m_fValue;

		int m_lId;

		tString m_sPhysicsMaterial;

		tMaterialTextureParamVec m_vTextureParams;
		//tTextureVec m_vTexture;
		tResourceImageVec m_vImage;

		eMaterialType m_Type;
		eMaterialPicture m_Picture;

		iGpuProgram *m_pProgram[2][kMaxProgramNum];

		int m_lPassCount;

		// New stuff
		cMaterialParameter m_vParams[eMaterialRenderType_LastEnum];
	};

	class iMaterialType
	{
	public:
		virtual bool IsCorrect(tString a_sName)=0;
		virtual iMaterial *Create(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics,
			cImageManager *a_pImageManager, cTextureManager *a_pTextureManager,
			cRenderer2D *a_pRenderer, cGpuProgramManager *a_pProgramManager,
			eMaterialPicture a_Picture, cRenderer3D *a_pRenderer3D)=0;
	};

	typedef std::list<iMaterialType*> tMaterialTypeList;
	typedef tMaterialTypeList::iterator tMaterialTypeListIt;
};
#endif