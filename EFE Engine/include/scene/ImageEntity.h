#ifndef EFE_IMAGE_ENTITY_H
#define EFE_IMAGE_ENTITY_H

#include <vector>
#include "scene/Entity2D.h"
#include "graphics/GraphicsTypes.h"

class TiXmlElement;

namespace efe
{
	class cResources;
	class cGraphics;
	class cImageEntityData;
	class cImageAnimation;

	class cImageEntity : public iEntity2D
	{
	public:
		
		void Render();
	};
};
#endif