#include "scene/Node2D.h"
#include "scene/Entity2D.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cNode2D::cNode2D()
	{
	}

	cNode2D::~cNode2D()
	{
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iNode *cNode2D::CreateChild()
	{
		cNode2D *pNode = efeNew(cNode2D, ());

		m_lstNode.push_back(pNode);

		return pNode;
	}

	//--------------------------------------------------------------

	cNode2D *cNode2D::CreateChiled2D()
	{
		return static_cast<cNode2D*>(CreateChild());
	}

	//--------------------------------------------------------------

	void cNode2D::SetPosition(const cVector3f &a_vPos)
	{
		cVector3f vPosAdd = a_vPos - m_vPosition;
		m_vPosition = a_vPos;

		for (tEntityListIt it = m_lstEntity.begin();it != m_lstEntity.end();++it)
		{
			iEntity2D *pEntity = static_cast<iEntity2D*>(*it);

			pEntity->SetPosition(pEntity->GetLocalPosition());
		}

		for (tNodeListIt NIt = m_lstNode.begin(); NIt != m_lstNode.end(); ++NIt)
		{
			cNode2D *pNode = static_cast<cNode2D*>(*NIt);

			pNode->SetPosition(pNode->m_vPosition + vPosAdd);
		}
	}

	//--------------------------------------------------------------

	void cNode2D::SetRotation(const cVector3f &a_vRot)
	{
	}

	//--------------------------------------------------------------

	void cNode2D::SetScale(const cVector3f &a_vScale)
	{
	}
}