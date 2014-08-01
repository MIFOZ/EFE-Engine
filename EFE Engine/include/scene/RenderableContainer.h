#ifndef EFE_RENDERABLE_CONTAINER_H
#define EFE_RENDERABLE_CONTAINER_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"

namespace efe
{
	class iRenderable;
	class cFrustum;
	class cRenderList;

	class iRenderableContainer
	{
	public:
		virtual void GetVisible(cFrustum *a_pFrustum, cRenderList *a_pRenderList)=0;
	};
};
#endif