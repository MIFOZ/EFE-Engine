#include "haptic/LowLevelHaptic.h"

#include "haptic/HapticSurface.h"
#include "haptic/HapticShape.h"
#include "haptic/HapticForce.h"

#include "system/LowLevelSystem.h"
#include "system/SystemTypes.h"

#include "scene/Camera3D.h"
#include "math/Math.h"
#include "physics/PhysicsBody.h"
#include "scene/SubMeshEntity.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTERS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iLowLevelHaptic::iLowLevelHaptic()
	{
		m_pCamera = NULL;
		m_vCameraOffset = 0;

		m_vMinMousePos = cVector2f(-50,-50);
		m_vMaxMousePos = cVector2f(50,50);
		m_vScreenSize = cVector2f(800,600);

		m_bUpdateShapes = true;
	}

		//--------------------------------------------------------------

	iLowLevelHaptic::~iLowLevelHaptic()
	{

	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void iLowLevelHaptic::Update(float a_fTimeStep)
	{
		UpdateLowLevel(a_fTimeStep);

		static bool bHardwareSet = false;

		m_vPreviousHardwarePos = m_vCurrentHardwarePos;
		m_vCurrentHardwarePos = GetHardwarePosition();
		if (bHardwareSet == false)
		{
			m_vPreviousHardwarePos = m_vCurrentHardwarePos;
			bHardwareSet = true;
		}

		if (m_pCamera)
		{
			m_vPreviousScreenPos = m_vCurrentScreenPos;

			cVector3f vProjPos = cMath::MatrixMul(m_pCamera->GetViewMatrix(), GetProxyPosition());
			vProjPos = cMath::MatrixMulDivideW(m_pCamera->GetProjectionMatrix(), vProjPos);

			cVector2f vPos2D((vProjPos.x + 1) * 0.5f, (-vProjPos.y + 1) * 0.5f);

			m_vCurrentScreenPos = vPos2D;

			if (m_bSreenPosFirstTime)
			{
				m_vPreviousScreenPos = m_vCurrentScreenPos;
				m_bSreenPosFirstTime = false;
			}
		}

		if (m_bUpdateShapes)
		{
			tHapticShapeListIt it = m_lstShapes.begin();
			for (; it != m_lstShapes.end(); ++it)
			{
				iHapticShape *pShape = *it;

				//If a body is attached, use it to update shape
				if (pShape->GetBody())
				{
					iPhysicsBody *pBody = pShape->GetBody();
					if (pBody->GetTransformUpdateCount() != pShape->GetTransformCount())
					{
						pShape->SetTransformCount(pBody->GetTransformUpdateCount());
						pShape->SetTransform(pBody->GetLocalMatrix());
					}
				}
				else if (pShape->GetSubMeshEntity())
				{
					cSubMeshEntity *pSubEntity = pShape->GetSubMeshEntity();
					if (pSubEntity->GetTransformUpdateCount() != pShape->GetTransformCount())
					{
						pShape->SetTransformCount(pSubEntity->GetTransformUpdateCount());
						pShape->SetTransform(pSubEntity->GetWorldMatrix());
					}
				}
			}

		}
	}

	//--------------------------------------------------------------

	bool iLowLevelHaptic::Init(cResources *a_pResources)
	{
		m_pResources = a_pResources;

		if (InitLowLevel() == false) return false;

		m_vCurrentHardwarePos = GetHardwarePosition();
		m_vPreviousHardwarePos = m_vCurrentHardwarePos;
		m_bSreenPosFirstTime = true;

		return true;
	}

	//--------------------------------------------------------------

	void iLowLevelHaptic::DestroyAllShapes()
	{
		STLDeleteAll(m_lstShapes);
	}

	//--------------------------------------------------------------

	void iLowLevelHaptic::DestroyAll()
	{
		STLMapDeleteAll(m_mapSurfaces);
		STLDeleteAll(m_lstForces);
		STLDeleteAll(m_lstShapes);
	}

	//--------------------------------------------------------------

	cVector3f iLowLevelHaptic::GetHardwarePosDelta()
	{
		return m_vCurrentHardwarePos - m_vPreviousHardwarePos;
	}

	//--------------------------------------------------------------

	cVector2f iLowLevelHaptic::GetProxyScreenPos(const cVector2f &a_vScreenPos)
	{
		return m_vCurrentScreenPos * m_vScreenSize;
	}

	cVector2f iLowLevelHaptic::GetProxyScreenDeltaPos(const cVector2f &a_vScreenPos)
	{
		return (m_vCurrentScreenPos - m_vPreviousScreenPos) * m_vScreenSize;
	}

	//--------------------------------------------------------------

	cVector2f iLowLevelHaptic::GetRelativeVirtualMousePos()
	{
		cVector3f vRel = GetHardwarePosDelta();
		cVector2f vPos = cVector2f(vRel.x / vRel.y) / (m_vMaxMousePos - m_vMinMousePos);
		vPos.y = -vPos.y;
		vPos = vPos * m_vScreenSize;
		return vPos;
	}

	cVector2f iLowLevelHaptic::GetRelativeMousePos()
	{
		cVector2f vPos = (cVector2f(m_vCurrentHardwarePos.x, m_vCurrentHardwarePos.y) - m_vMinMousePos) / 
			(m_vMaxMousePos - m_vMinMousePos);
		vPos.y = 1 - vPos.y;
		vPos = vPos * m_vScreenSize;
		return vPos;
	}

	void iLowLevelHaptic::SetVirtualMousePosBound(const cVector2f &a_vMin, const cVector2f &a_vMax,
		const cVector2f &a_vScreenSize)
	{
		m_vMinMousePos = a_vMin;
		m_vMaxMousePos = a_vMax;
		m_vScreenSize = a_vScreenSize;
	}

	//--------------------------------------------------------------

	iHapticSurface *iLowLevelHaptic::GetSurfaceFromName(const tString &a_sName)
	{
		tHapticSurfaceMapIt it = m_mapSurfaces.find(a_sName);
		if (it == m_mapSurfaces.end())
			return NULL;

		iHapticSurface *pSurface = it->second;

		return pSurface;
	}

	//--------------------------------------------------------------

	void iLowLevelHaptic::DestroyShape(iHapticShape *a_pShape)
	{
		STLFindAndDelete(m_lstShapes, a_pShape);
	}

	bool iLowLevelHaptic::ShapeExists(iHapticShape *a_pShape)
	{
		tHapticShapeListIt it = m_lstShapes.begin();
		for (; it != m_lstShapes.end(); ++it)
		{
			if (a_pShape == *it) return true;
		}

		return false;
	}

	//--------------------------------------------------------------

	cHapticShapeIterator iLowLevelHaptic::GetShapeIterator()
	{
		return cHapticShapeIterator(&m_lstShapes);
	}

	//--------------------------------------------------------------
	
	void iLowLevelHaptic::DestroyForce(iHapticForce *a_pForce)
	{
		STLFindAndDelete(m_lstForces, a_pForce);
	}

	void iLowLevelHaptic::StopAllForces()
	{
		tHapticForceListIt it = m_lstForces.begin();
		for (; it != m_lstForces.end(); ++it)
		{
			iHapticForce *pForce = *it;
			pForce->SetActive(false);
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------


}