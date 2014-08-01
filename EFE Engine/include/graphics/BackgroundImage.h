#ifndef EFE_BACKGROUND_IMAGE_H
#define EFE_BACKGROUND_IMAGE_H

#include <map>
#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"
#include "graphics/Material.h"

namespace efe
{
	class cBackgroundImage
	{
	public:
		cBackgroundImage(iMaterial *a_pMat, const cVector3f &a_vPos, bool a_bTile,
				const cVector2f &a_vSize, const cVector2f &a_vPosPercent, const cVector2f &a_vVel);
		~cBackgroundImage();

		void Draw(const cRectf &a_CollideRect, iLowLevelGraphics *a_pLowLevelGraphics);

		void Update();

	private:
		iMaterial *m_pMaterial;

		tVertexVec m_vVtx;

		cVector3f m_vPos;
		bool m_bTile;
		cVector2f m_vSize;
		cVector2f m_vPosPercent;
		cVector2f m_vVel;
	};

	typedef std::map<float,cBackgroundImage*> tBackgroundImageMap;
	typedef tBackgroundImageMap::iterator tBackgroundImageMapIt;
};

#endif