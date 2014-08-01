#ifndef EFE_RENDERER2D_H
#define EFE_RENDERER2D_H

#include <set>
#include <list>
#include <math.h>

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"
#include "scene/Light2D.h"
#include "graphics/Material.h"
#include "graphics/Mesh2D.h"

namespace efe
{
#define MAX_SHADOW_POINTS (100)
#define MAX_ONSCREEN_SHADOWCASTERS (100)

	class cScene;
	class cResources;
	class iLowLevelGraphics;
	class iLowLevelResources;
	class cRenderObject2D;
	class cGraphicsDrawer;
	class cCamera2D;
	class cGridMap2D;
	class iTexture;

	class cRenderObject2DCompare
	{
	public:
		bool operator()(const cRenderObject2D &a_ObjectA, const cRenderObject2D &a_ObjectB);
	};

	class cRenderTransObjectCompare
	{
	public:
		bool operator()(const cRenderObject2D &a_ObjectA, const cRenderObject2D &a_ObjectB);
	};

	typedef std::multiset<cRenderObject2D, cRenderObject2DCompare> tRenderObjectSet;
	typedef tRenderObjectSet::iterator tRenderObjectSetIt;

	typedef std::multiset<cRenderObject2D, cRenderTransObjectCompare> tRenderTransObjectSet;
	typedef tRenderTransObjectSet::iterator tRenderTransObjectSetIt;

	typedef std::list<cRenderObject2D> tRenderObjectList;
	typedef tRenderObjectList::iterator tRenderObjectListIt;

	class cResources;
	class cWorld2D;

	class cRenderer2D
	{
	public:
		cRenderer2D(iLowLevelGraphics *a_pLowLevelGraphics,cResources *a_pResources,cGraphicsDrawer *a_pGraphicsDrawer);
		~cRenderer2D();

		void RenderObjects(cCamera2D *a_pCamera, cGridMap2D *a_pMapLights, cWorld2D *a_pWorld);
		void AddObject(cRenderObject2D &a_Object);

		void SetShadowZ(float a_fZ){m_fShadowZ = a_fZ;}
		float GetShadowZ(){return m_fShadowZ;}

	private:
		iLowLevelGraphics *m_pLowLevelGraphics;
		iLowLevelResources *m_pLowLevelResources;
		cResources *m_pResources;
		cGraphicsDrawer *m_pGraphicsDrawer;

		tRenderObjectSet m_mapObject;
		tRenderTransObjectSet m_mapTransObject;

		iTexture *m_pLightMap[2];

		cColor m_AmbientLight;

		cRectf m_PrevLightRect;

		int m_vShadowPoints[MAX_SHADOW_POINTS][2];
		int m_lShadowPointSize;

		float m_fShadowZ;

		tLightList m_lstLights;
		tLightList m_lstFaseLights;

		void ClearShadows();
		bool RenderShadow(cCamera2D *a_pCamera,iLight2D *pLight,cWorld2D *a_pWorld);

		__forceinline void RenderObject(const cRenderObject2D &a_Object, unsigned int &a_IdxAdd, iMaterial *pMat,
									iLight2D *pLight, eMaterialRenderType a_RenderType, cCamera2D *a_pCam);

		__forceinline cVector2f CalcLineEnd(cVector3f a_vLight, cVector3f a_vPoint, float a_fRadius,
											cVector2f &a_vSide, cVector2f a_vClipPos);

		__forceinline void FindShadowPoints(tMesh2DEdgeVec *a_pEdgeVec, cVector2f a_vLightPos, cVector2f a_vTilePos);

		__forceinline int CreateVertexes(cVector2f a_vLightPos, cRectf a_LightRect, float a_fRadius, bool m_bNonFit,
			cVector2f a_vTilePos, tVertexVec *a_pVtxVec, cColor a_ShadowColor, int a_lFirstIndex, float a_fSourceSize);

		__forceinline bool ClipPoints(cVector3f *a_vPoints, cRectf a_Rect, cVector2f a_vPos, float a_fSize);
	};
};

#endif