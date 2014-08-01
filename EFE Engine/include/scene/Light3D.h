#ifndef EFE_LIGHT3D_H
#define EFE_LIGHT3D_H

#include <list>
#include <set>
#include "scene/Entity3D.h"
#include "scene/Light.h"
#include "graphics/GraphicsTypes.h"
#include "graphics/Renderable.h"

class TiXmlElement;

namespace efe
{
	enum eLight3DType
	{
		eLight3DType_Point,
		eLight3DType_Spot,
		eLight3DType_LastEnum
	};

	enum eShadowVolumeType
	{
		eShadowVolumeType_None,
		eShadowVolumeType_ZPass,
		eShadowVolumeType_ZFail,
		eShadowVolumeType_LastEmum
	};

	class iLowLevelGraphics;
	class cRenderSettings;
	class cCamera3D;
	class cFrustum;
	class iGpuProgram;
	class iTexture;
	class cTextureManager;
	class cResources;
	class cFileSearcher;

	class cSectorVisibilityContainer;

	typedef std::set<iRenderable*> tCasterCacheSet;
	typedef tCasterCacheSet::iterator tCasterCacheSetIt;



	//--------------------------------------------------------------

	class iLight3D : public iLight, public iRenderable
	{
	public:
		iLight3D(tString a_sName, cResources *a_pResources);
		virtual ~iLight3D();

		void UpdateLogic(float a_fTimeStep);

		virtual void SetFarAttenuation(float a_fX);
		virtual void SetNearAttenuation(float a_fX);

		cVector3f GetLightPosition();

		virtual bool BeginDraw(cRenderSettings *a_pRenderSettings, iLowLevelGraphics *a_pLowLevelGraphics);
		virtual void EndDraw(cRenderSettings *a_pRenderSettings, iLowLevelGraphics *a_pLowLevelGraphics);

		bool CheckObjectIntersection(iRenderable *a_pObject);

		void AddShadowCaster(iRenderable *a_pObject, cFrustum *a_pFrustum, bool a_bStatic ,cRenderList *a_pRenderList);
		bool HasStaticCasters();
		void ClearCasters(bool a_bClearStatic);

		void SetAllStaticCastersAdded(bool a_bX){m_bStaticCasterAdded = a_bX;}
		bool m_bAllStaticCastersAdded(){return m_bStaticCasterAdded;}

		eLight3DType GetLightType(){return m_LightType;}
		tString	GetEntityType(){return "iLight3D";}

		bool IsVisible();
		void SetVisible(bool a_bVisible);

		//Renderable implementation
		iMaterial *GetMaterial(){return NULL;}
		iVertexBuffer *GetVertexBuffer(){return NULL;}

		bool IsShadowCaster(){return false;}
		eRenderableType GetRenderType(){return eRenderableType_Light;}

		cBoundingVolume *GetBoundingVolume();

		int GetMatrixUpdateCount(){return GetTransformUpdateCount();}

		cMatrixf *GetModelMatrix(cCamera3D *a_pCamera);

		__forceinline void RenderShadow(iRenderable *a_pObject, cRenderSettings *a_pRenderSettings, iLowLevelGraphics *a_pLowLevelGraphics);

		void LoadXMLProperties(const tString a_sFile);

		void SetOnlyAffectInSector(bool a_bX){m_bOnlyAffectInSector = a_bX;}
		bool GetOnlyAffectInSector(){return m_bOnlyAffectInSector;}
	protected:
		void OnFlickerOff();
		void OnFlickerOn();
		void OnSetDiffuse();

		//virtual cSectorVisibilityContainer *CreateSectorVisibility()=0;

		virtual void UpdateBoundingVolume()=0;
		virtual bool CreateClipRect(cRectl &a_ClipRect, cRenderSettings *a_pRenderSettings, iLowLevelGraphics *a_pLowLevelGraphics)=0;

		eLight3DType m_LightType;

		cTextureManager *m_pTextureManager;
		cFileSearcher *m_pFileSearcher;

		iTexture *m_pFalloffMap;

		iTexture *m_vTempTextures[3];

		cMatrixf mtxTemp;

		bool m_bStaticCasterAdded;

		bool m_bOnlyAffectInSector;

		int m_lSectorVisibilityCount;
		cSectorVisibilityContainer *m_pVisSectorCont;

		unsigned int *m_pIndexArray;
	};

	typedef std::list<iLight3D*> tLight3DList;
	typedef tLight3DList::iterator tLight3DListIt;
};
#endif