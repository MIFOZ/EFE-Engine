#ifndef EFE_LIGHT_2D_H
#define EFE_LIGHT_2D_H

#include <list>
#include "scene/Entity2D.h"
#include "scene/Light.h"
#include "graphics/GraphicsTypes.h"

namespace efe
{
	class iLowLevelGraphics;

	class iLight2D : public iEntity2D, iLight
	{
	public:
		iLight2D(tString a_sName);
		virtual ~iLight2D();

		tString GetEntityType(){return "iLight2D";}
	};

	typedef std::list<iLight2D*> tLightList;
	typedef tLightList::iterator tLightListIt;
};
#endif