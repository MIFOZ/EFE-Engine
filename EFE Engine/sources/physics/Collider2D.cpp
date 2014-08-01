#include "physics/Collider2D.h"

//#include "scene/World2D.h"

#include "math/Math.h"
#include "system/LowLevelSystem.h"
#include "physics/CollideData2D.h"


namespace efe
{
	cCollider2D::cCollider2D()
	{
		m_pWorld = NULL;
	}

	cCollider2D::~cCollider2D()
	{
	}
}