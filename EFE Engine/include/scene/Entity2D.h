#ifndef EFE_ENTITY2D_H
#define EFE_ENTITY2D_H

#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "scene/Entity.h"

namespace efe
{
	class cGrid2DObject;

	class iEntity2D : public iEntity
	{
	public:
		iEntity2D(tString a_sName);
		virtual ~iEntity2D(){}

		virtual const cRectf &GetBoundingBox()=0;
		virtual bool UpdateBoundingBox()=0;

		cVector3f &GetLocalPosition(){return m_vPosition;}
		cVector3f &GetLocalRotation(){return m_vRotation;}
		cVector3f &GetLocalScale(){return m_vScale;}

		cVector3f GetWorldPosition();
		cVector3f GetWorldRotation();
		cVector3f GetWorldScale();

		void SetPosition(const cVector3f &a_vPos);
		void SetRotation(const cVector3f &a_vRot);
		void SetScale(const cVector3f &a_vScale);

		cGrid2DObject *GetGrid2DObject(){return m_pGridObject;}
		void SetGrid2DObject(cGrid2DObject *a_pGrid){m_pGridObject = a_pGrid;}
		bool HasGrid2DObject(){return m_pGridObject != NULL;}
	protected:
		cGrid2DObject *m_pGridObject;

		cVector3f  m_vPosition;
		cVector3f  m_vRotation;
		cVector3f m_vScale;

		cVector3f  m_vLastPosition;
		cVector3f  m_vLastRotation;
		cVector3f m_vLastScale;

		cRectf m_BoundingBox;
	};

	typedef std::list<iEntity2D*> tEntity2DList;
	typedef tEntity2DList::iterator tEntity2DListIt;
};

#endif