#include "scene/Entity2D.h"
#include "scene/Node2D.h"
#include "scene/GridMap2D.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iEntity2D::iEntity2D(tString a_sName) : iEntity(a_sName), m_vPosition(0), m_vRotation(0), m_vScale(0),
		m_vLastPosition(0), m_vLastRotation(0), m_vLastScale(0),
		m_pGridObject(NULL)
	{
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cVector3f iEntity2D::GetWorldPosition()
	{
		if (m_pParentNode)
		{
			cNode2D *pNode2D = static_cast<cNode2D*>(m_pParentNode);

			return pNode2D->GetPosition() + m_vPosition;
		}
		else
			return m_vPosition;
	}

	//--------------------------------------------------------------

	cVector3f iEntity2D::GetWorldRotation()
	{
		if (m_pParentNode)
		{
			cNode2D *pNode2D = static_cast<cNode2D*>(m_pParentNode);

			return pNode2D->GetRotation() + m_vRotation;
		}
		else
			return m_vRotation;
	}

	//--------------------------------------------------------------

	cVector3f iEntity2D::GetWorldScale()
	{
		if (m_pParentNode)
		{
			cNode2D *pNode2D = static_cast<cNode2D*>(m_pParentNode);

			return pNode2D->GetRotation() + m_vRotation;
		}
		else
			return m_vRotation;
	}

	//--------------------------------------------------------------

	void iEntity2D::SetPosition(const cVector3f &a_vPos)
	{
		m_vLastPosition = m_vPosition;
		m_vPosition = a_vPos;

		if (UpdateBoundingBox())
			if (m_pGridObject)
				m_pGridObject->Update(GetBoundingBox());
	}

	//--------------------------------------------------------------

	void iEntity2D::SetRotation(const cVector3f &a_vRot)
	{
		m_vLastRotation = m_vRotation;
		m_vRotation = a_vRot;

		if (UpdateBoundingBox())
			if (m_pGridObject)
				m_pGridObject->Update(GetBoundingBox());
	}

	//--------------------------------------------------------------

	void iEntity2D::SetScale(const cVector3f &a_vScale)
	{
		m_vLastRotation = m_vScale;
		m_vScale = a_vScale;

		if (UpdateBoundingBox())
			if (m_pGridObject)
				m_pGridObject->Update(GetBoundingBox());
	}
}