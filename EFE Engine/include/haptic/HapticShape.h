#ifndef EFE_HAPTIC_SHAPE_H
#define EFE_HAPTIC_SHAPE_H

#include "math/MathTypes.h"
#include "haptic/HapticTypes.h"
#include "graphics/GraphicsTypes.h"

namespace efe
{
	class iHapticSurface;
	class iLowLevelGraphics;
	class iPhysicsBody;
	class cSubMeshEntity;

	class iHapticShape
	{
	public:
		iHapticShape(const tString &a_sName, eHapticShapeType a_Type) : 
			m_pSurface(NULL), m_pType(a_Type), m_sName(a_sName),
			m_pBody(NULL), m_pSubMeshEntity(NULL), m_lTransformCount(-1) {}
		virtual ~iHapticShape() {}

		eHapticShapeType GetType() {return m_pType;}
		const tString &GetName() {return m_sName;}

		virtual void SetEnabled(bool a_bX)=0;
		virtual bool GetEnabled()=0;

		virtual void SetTransform(const cMatrixf &a_mtxTransform)=0;
		virtual cMatrixf GetTransform()=0;

		virtual cVector3f GetAppliedForce()=0;

		virtual void SetSurface(iHapticSurface *a_pSurface)=0;

		virtual void RenderDebug(iLowLevelGraphics *a_pLowLevel, const cColor &a_Color)=0;

		iHapticSurface *GetSurface() const {return m_pSurface;}

		cVector3f GetSize() const {return m_vSize;}

		iPhysicsBody *GetBody() const {return m_pBody;}
		void SetBody(iPhysicsBody *a_pBody) {m_pBody = a_pBody;}

		cSubMeshEntity *GetSubMeshEntity() const {return m_pSubMeshEntity;}
		void SetSubMeshEntity(cSubMeshEntity *a_pSubMeshEntity) {m_pSubMeshEntity = a_pSubMeshEntity;}

		int GetTransformCount() const {return m_lTransformCount;}
		void SetTransformCount(int a_lX) {m_lTransformCount = a_lX;}

	protected:
		tString m_sName;
		iHapticSurface *m_pSurface;
		eHapticShapeType m_pType;
		cVector3f m_vSize;

		iPhysicsBody *m_pBody;
		cSubMeshEntity *m_pSubMeshEntity;
		int m_lTransformCount;
	};
};

#endif