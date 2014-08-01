#include "graphics/Renderable.h"

#include "math/Math.h"
#include "math/Frustum.h"
#include "system/LowLevelSystem.h"
#include "graphics/RenderList.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iRenderable::iRenderable(const tString &a_sName) : iEntity3D(a_sName)
	{
		m_bRendered = true;
		m_lLastMatrixCount = -1;

		m_bStatic = false;

		m_lRenderCount = -1;
		m_lPrevRenderCount = -1;

		m_lCalcScaleMatrixCount = -1;
		m_vCalcScale = cVector3f(1,1,1);

		m_bForceShadow = false;

		m_bIsOneSided = false;
		m_vOneSidedNormal = 0;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMatrixf *iRenderable::GetInvModelMatrix()
	{
		cMatrixf *pModelMatrix = GetModelMatrix(NULL);
		if (pModelMatrix == NULL) return NULL;

		if (m_lLastMatrixCount != GetMatrixUpdateCount())
		{
			m_lLastMatrixCount = GetMatrixUpdateCount();

			m_mtxInvModel = cMath::MatrixInverse(*pModelMatrix);
		}

		return &m_mtxInvModel;
	}

	//--------------------------------------------------------------

	const cVector3f &iRenderable::GetCalcScale()
	{
		cMatrixf *pModelMatrix = GetModelMatrix(NULL);

		if (pModelMatrix != NULL && m_lCalcScaleMatrixCount != GetMatrixUpdateCount())
		{
			m_lCalcScaleMatrixCount = GetMatrixUpdateCount();
			m_vCalcScale.x = pModelMatrix->GetRight().Length();
			m_vCalcScale.y = pModelMatrix->GetUp().Length();
			m_vCalcScale.z = pModelMatrix->GetForward().Length();
		}

		return m_vCalcScale;
	}

	//--------------------------------------------------------------

	bool iRenderable::CollidesWithBV(cBoundingVolume *a_pBV)
	{
		return cMath::CheckCollisionBV(*GetBoundingVolume(), *a_pBV);
	}

	//--------------------------------------------------------------

	bool iRenderable::CollidesWithFrustum(cFrustum *a_pFrustum)
	{
		return a_pFrustum->CollideBoundingVolume(GetBoundingVolume()) != eFrustumCollision_Outside;
	}

	//--------------------------------------------------------------

	kBeginSerializeVirtual(cSaveData_iRenderable, cSaveData_Entity3D)
		kSerializeVar(m_bStatic, eSerializeType_Bool)
		kSerializeVar(m_bRendered, eSerializeType_Bool)
		kSerializeVar(m_fZ, eSerializeType_Float32)
	kEndSerialize()

	iSaveData *iRenderable::CreateSaveData()
	{
		return NULL;
	}

	//--------------------------------------------------------------

	void iRenderable::SaveToSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_SaveToBegin(iRenderable);

		kSaveData_SaveTo(m_bStatic);
		kSaveData_SaveTo(m_bRendered);
		kSaveData_SaveTo(m_fZ);
	}

	//--------------------------------------------------------------

	void iRenderable::LoadFromSaveData(iSaveData *a_pSaveData)
	{
		kSaveData_LoadFromBegin(iRenderable);

		kSaveData_LoadFrom(m_bStatic);
		kSaveData_LoadFrom(m_bRendered);
		kSaveData_LoadFrom(m_fZ);
	}

	//--------------------------------------------------------------

	void iRenderable::SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame)
	{
		kSaveData_SetupBegin(iRenderable);
	}
}