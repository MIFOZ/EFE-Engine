#ifndef EFE_COLLIDER_ENTITY_H
#define EFE_COLLIDER_ENTITY_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "graphics/Renderable.h"

namespace efe
{
	class iPhysicsWorld;
	class iPhysicsBody;

	class cColliderEntity : public iRenderable
	{
	public:
		cColliderEntity(const tString &a_sName, iPhysicsBody *a_pBody, iPhysicsWorld *a_pWorld);
		~cColliderEntity();

		iMaterial *GetMaterial() {return NULL;}
		void UpdateGraphics(cCamera3D *a_pCamera, float a_fFrameTime, cRenderList *a_pRenderList){}
		iVertexBuffer *GetVertexBuffer() {return NULL;}

		cBoundingVolume *GetBoundingVolume();

		cMatrixf *GetModelMatrix(cCamera3D *a_pCamera);
		int GetMatrixUpdateCount();

		eRenderableType GetRenderType() {return eRenderableType_Dummy;}

		//Entity implementation
		tString GetEntityType() {return "Collider";}
		bool IsVisible() {return false;}

		bool IsShadowCaster() {return false;}

		void UpdateLogic(float a_fTimeStep) {}

	private:
		iPhysicsWorld *m_pPhysicsWorld;
		iPhysicsBody *m_pBody;
	};
};
#endif