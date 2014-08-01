#ifndef EFE_LIGHT3D_SPOT_H
#define EFE_LIGHT3D_SPOT_H

#include "scene/Light3D.h"

namespace efe
{
	class cResources;
	class iTexture;
	class cFrustum;

	/*kSaveData_ChildClass(iLight3D, cLight3DSpot)
	{
		kSaveData_ChildInit(cLight3DSpot)
	public:
		tString m_sTexture;

		float m_fFOV;
		float m_fAspect;
		float m_fNearClipPlane;


	};*/

	class cLight3DSpot : public iLight3D
	{
	public:
		cLight3DSpot(tString a_sName, cResources *a_pResources);
		~cLight3DSpot();

		const cMatrixf &GetViewMatrix();
		const cMatrixf &GetProjectionMatrix();
		const cMatrixf &GetViewProjMatrix();

		void SetTexture(iTexture *a_pTexture);
		iTexture *GetTexture();

		cFrustum *GetFrustum();
	private:
		//cSectorVisibilityContainer *CreateSectorVisibility();
		void UpdateBoundingVolume();

		bool CreateClipRect(cRectl &a_ClipRect, cRenderSettings *a_pRenderSettings, iLowLevelGraphics *a_pLowLevelGraphics);

		cMatrixf m_mtxProjection;
		cMatrixf m_mtxViewProj;
		cMatrixf m_mtxView;

		cFrustum *m_pFrustum;

		iTexture *m_pTexture;

		float m_fFOV;
		float m_fAspect;
		float m_fNearClipPlane;

		bool m_bProjectionUpdated;
		bool m_bViewProjUpdated;
		bool m_bFrustumUpdated;

		int m_lViewProjMatrixCount;
		int m_lViewMatrixCount;
		int m_lFrustumMatrixCount;
	};
};
#endif