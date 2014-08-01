#ifndef EFE_COLLIDER2D_H
#define EFE_COLLIDER2D_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"

namespace efe
{
	class cMesh2D;
	class cWorld2D;
	class cBody2D;
	class cCollideData2D;
	class cCollisionMesh2D;

	class cCollider2DDebug
	{
	public:
		cVector2f m_vPushVec;
		cVector2f m_vPushPos;
	};

	class cCollider2D
	{
	public:
		cCollider2D();
		~cCollider2D();

		void SetWorld(cWorld2D *a_pWorld){m_pWorld = a_pWorld;}

		tFlag CollideBody(cBody2D *a_pBody, cCollideData2D *a_pdata);

		//tFlag CollideRect(
	private:
		cWorld2D *m_pWorld;
	};
};
#endif