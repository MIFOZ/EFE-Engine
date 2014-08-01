#include "scene/Light3DSpot.h"
#include "math/Math.h"
#include "math/Frustum.h"
#include "resources/TextureManager.h"
#include "resources/Resources.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/Renderer3D.h"
#include "scene/Camera3D.h"

#include "scene/World3D.h"
#include "scene/Scene.h"
#include "game/Game.h"

#include "scene/SectorVisibility.h"
#include "scene/PortalContainer.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	cLight3DSpot::cLight3DSpot(tString a_sName, cResources *a_pResources) : iLight3D(a_sName, a_pResources)
	{
		m_bProjectionUpdated = true;
		m_bViewProjUpdated = true;
		m_bFrustumUpdated = true;

		m_LightType = eLight3DType_Spot;

		m_pFrustum = efeNew(cFrustum, ());

		m_lViewProjMatrixCount = -1;
		int m_lViewMatrixCount = -1;
		int m_lFrustumMatrixCount = -1;

		m_pTexture = NULL;

		m_fFOV = cMath::ToRad(60.0f);
		m_fAspect = 1.0f;
		m_fFarAttenuation = 100.0f;
		m_fNearClipPlane = 0.1f;

		m_mtxView = cMatrixf::Identity;
		m_mtxViewProj = cMatrixf::Identity;
		m_mtxProjection = cMatrixf::Identity;

		UpdateBoundingVolume();
	}

	cLight3DSpot::~cLight3DSpot()
	{
		if (m_pTexture) m_pTextureManager->Destroy(m_pTexture);
		efeDelete(m_pFrustum);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	const cMatrixf &cLight3DSpot::GetViewMatrix()
	{
		if (m_lViewMatrixCount != GetTransformUpdateCount())
		{
			m_lViewMatrixCount = GetTransformUpdateCount();
			m_mtxView = cMath::MatrixInverse(GetWorldMatrix());
		}

		return m_mtxView;
	}

	const cMatrixf &cLight3DSpot::GetProjectionMatrix()
	{
		if (m_bProjectionUpdated)
		{
			float fFar = m_fFarAttenuation;
			float fNear = m_fNearAttenuation;
			float fTop = tan(m_fFOV*0.5f) * fNear;
			float fBottom = -fTop;
			float fRight = m_fAspect * fTop;
			float fLeft = m_fAspect * fBottom;

			float A = (2.0f*fNear) / (fRight - fLeft);
			float B = (2.0f*fNear) / (fTop - fBottom);
			float D = -1.0f;
			float C = -(2.0f*fFar*fNear) / (fFar - fNear);
			float Z = -(fFar + fNear) / (fFar - fNear);

			float X = 0;
			float Y = 0;

			m_mtxProjection = cMatrixf(
				A,0,X,0,
				0,B,Y,0,
				0,0,Z,C,
				0,0,D,0);

			m_bProjectionUpdated = false;
			m_bViewProjUpdated = true;
			m_bFrustumUpdated = true;
		}

		return m_mtxProjection;
	}

	void cLight3DSpot::SetTexture(iTexture *a_pTexture)
	{
		if (m_pTexture) m_pTextureManager->Destroy(a_pTexture);

		m_pTexture = a_pTexture;
	}

	//--------------------------------------------------------------

	cFrustum *cLight3DSpot::GetFrustum()
	{
		if (m_lFrustumMatrixCount != GetTransformUpdateCount() || m_bFrustumUpdated || m_bProjectionUpdated)
		{
			m_pFrustum->SetViewProjMatrix(	GetProjectionMatrix(),
											GetViewMatrix(),
											m_fFarAttenuation,m_fNearClipPlane,
											m_fFOV,m_fAspect, GetWorldPosition(),false);

			m_bFrustumUpdated = false;
			m_lFrustumMatrixCount = GetTransformUpdateCount();
		}

		return m_pFrustum;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cLight3DSpot::UpdateBoundingVolume()
	{
		m_BoundingVolume = GetFrustum()->GetBoundingVolume();
	}

	bool cLight3DSpot::CreateClipRect(cRectl &a_ClipRect, cRenderSettings *a_pRenderSettings, iLowLevelGraphics *a_pLowLevelGraphics)
	{
		cVector2f vScreenSize = a_pLowLevelGraphics->GetScreenSize();
		bool bVisible = cMath::GetClipRectFromBV(a_ClipRect, *GetBoundingVolume(),
												a_pRenderSettings->m_pCamera->GetViewMatrix(),
												a_pRenderSettings->m_pCamera->GetProjectionMatrix(),
												a_pRenderSettings->m_pCamera->GetNearClipPlane(),
												cVector2l((int)vScreenSize.x,(int)vScreenSize.y));
		return bVisible;
	}
}