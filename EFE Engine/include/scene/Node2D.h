#ifndef EFE_NODE2D_H
#define EFE_NODE2D_H

#include <list>
#include "math/MathTypes.h"
#include "scene/Node.h"

namespace efe
{
	class cNode2D : public iNode
	{
	public:
		cNode2D();
		virtual ~cNode2D();

		iNode *CreateChild();
		cNode2D *CreateChiled2D();

		cVector3f &GetPosition(){return m_vPosition;}
		cVector3f &GetRotation(){return m_vRotation;}
		cVector3f &GetScale(){return m_vScale;}

		void SetPosition(const cVector3f &a_vPos);
		void SetRotation(const cVector3f &a_vRot);
		void SetScale(const cVector3f &a_vScale);

	private:
		cVector3f  m_vPosition;
		cVector3f  m_vRotation;
		cVector3f m_vScale;
	};
};

#endif