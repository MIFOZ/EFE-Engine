#ifndef EFE_TILEMAP_IT_H
#define EFE_TILEMAP_IT_H

#include "scene/Tile.h"

namespace efe
{
	class iTileMapIt
	{
	public:
		virtual ~iTileMapIt(){}

		virtual bool HasNext()=0;
		virtual cTile *Next()=0;
		virtual cTile *PeekNext()=0;
		virtual int GetNum()=0;
		virtual int GetCurrentLayer()=0;
	};
};
#endif