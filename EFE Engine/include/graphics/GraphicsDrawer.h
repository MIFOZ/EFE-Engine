#ifndef EFE_GRAPHICSDRAWER_H
#define EFE_GRAPHICSDRAWER_H

#include <vector>
#include "graphics/GraphicsTypes.h"
#include "graphics/BackgroundImage.h"

namespace efe
{
	class iLowLevelGraphics;
	class cResourceImage;
	class cGfxObject;
	class cMaterialHandler;
	class iMaterial;

	class cGfxBufferObject
	{
	public:
		cGfxObject *m_pObject;
		cVector3f m_vTransform;

		bool m_bIsColorAndSize;
		cColor m_Color;
		cVector2f m_vSize;
		bool m_bFlipH;
		bool m_bFlipV;
		float m_fAngle;

		iMaterial *GetMaterial() const;
		float GetZ() const{return m_vTransform.z;}
	};

	typedef std::vector<cGfxObject> tGfxObjectVec;
	typedef tGfxObjectVec::iterator tGfxObjectVecIt;

	class cGfxBufferCompare
	{
	public:
		bool operator()(const cGfxBufferObject &a_ObjectA, const cGfxBufferObject &a_ObjectB);
	};

	typedef std::multiset<cGfxBufferObject, cGfxBufferCompare> tGfxBufferSet;
	typedef tGfxBufferSet::iterator tGfxBufferSetIt;

	class cResources;

	typedef std::list<cGfxObject*> tGfxObjectList;
	typedef tGfxObjectList::iterator tGfxObjectListIt;

	class cGraphicsDrawer
	{
	public:
		cGraphicsDrawer(iLowLevelGraphics *a_pLowLevelGraphics, cMaterialHandler *a_pMaterialHandler,
							cResources *a_pResources);
		~cGraphicsDrawer();

		void DrawGfxObject(cGfxObject *a_pObject, const cVector3f &a_vPos);

		void DrawGfxObject(cGfxObject *a_pObject, const cVector3f &a_vPos,
											const cVector2f &a_vSize, const cColor &a_Color,
											bool a_bFlipH=false, bool a_bFlipV=false, float a_fAngle = 0);

		void DrawAll();

		cGfxObject *CreateGfxObject(const tString &a_sFileName, const tString &a_sMaterialName,
									bool a_bAddToList=true);

		cGfxObject *CreateGfxObject(iBitmap2D *a_pBmp, const tString &a_sMaterialName,
									bool a_bAddToList=true);

		cGfxObject *CreateGfxObjectFromTexture(const tString &a_sFileName, const tString &a_sMaterialName,
									bool a_bAddToList=true);

		cBackgroundImage *AddBackgroundImage(const tString &a_sFileName, const tString &a_sMaterialName,
							const cVector3f &a_vPos, bool a_bTile,
							const cVector2f &a_vSize, const cVector2f &a_vPosPercent, const cVector2f &a_vVel);

		void UpdateBackgrounds();
		void DrawBackgrounds(const cRectf &a_CollideRect);
		void ClearBackgrounds();
	private:
		iLowLevelGraphics *m_pLowLevelGraphics;
		cMaterialHandler *m_pMaterialHandler;
		cResources *m_pResources;

		tGfxBufferSet m_setGfxBuffer;

		tGfxObjectList m_lstGfxObjects;

		tBackgroundImageMap m_mapBackgroundImages;
	};
};
#endif