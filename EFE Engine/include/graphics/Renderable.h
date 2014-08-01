#ifndef EFE_RENDERABLE_H
#define EFE_RENDERABLE_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "scene/Entity3D.h"

namespace efe
{
	class iMaterial;
	class iVertexBuffer;

	class cBoundingVolume;
	class cFrustum;

	class iLight3D;
	class cCamera3D;

	class cRenderList;

	class iRenderable;

	enum eRenderableType
	{
		eRenderableType_Normal,
		eRenderableType_ParticleSystem,
		eRenderableType_Mesh,
		eRenderableType_Light,
		eRenderableType_Dummy,
		eRenderableType_LastEnum
	};

	kSaveData_ChildClass(iEntity3D, iRenderable)
	{
		kSerializableClassInit(iRenderable)
	public:
		bool m_bStatic;
		bool m_bRendered;
		float m_fZ;
	};

	class iRenderable : public iEntity3D
	{
	public:
		iRenderable(const tString &a_sName);

		bool IsRendered(){return m_bRendered;}
		virtual void SetRendered(bool a_bX){m_bRendered = a_bX;}

		virtual iMaterial *GetMaterial()=0;
		virtual iVertexBuffer *GetVertexBuffer()=0;

		virtual bool CollidesWithBV(cBoundingVolume *a_pBV);
		virtual bool CollidesWithFrustum(cFrustum *a_pFrustum);

		virtual cMatrixf *GetModelMatrix(cCamera3D *a_pCamera)=0;

		virtual eRenderableType GetRenderType()=0;

		virtual void UpdateGraphics(cCamera3D *a_pCamera, float a_fFrameTime, cRenderList *a_pRenderList){}

		virtual int GetMatrixUpdateCount()=0;

		float GetZ(){return m_fZ;}
		void SetZ(float a_fZ){m_fZ = a_fZ;}

		cMatrixf *GetInvModelMatrix();

		const cVector3f &GetCalcScale();

		void SetStatic(bool a_bX){m_bStatic = a_bX;}
		bool IsStatic() const {return m_bStatic;}

		__forceinline int GetRenderCount() const{return m_lRenderCount;}
		__forceinline void SetRenderCount(const int a_lCount){m_lRenderCount = a_lCount;}

		//SaveObject implementation
		virtual iSaveData *CreateSaveData();
		virtual void SaveToSaveData(iSaveData *a_pSaveData);
		virtual void LoadFromSaveData(iSaveData *a_pSaveData);
		virtual void SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);

	protected:
		cMatrixf m_mtxInvModel;
		cMatrixf m_mtxPrevious;

		bool m_bIsOneSided;
		cVector3f m_vOneSidedNormal;

		int m_lLastMatrixCount;

		bool m_bStatic;

		bool m_bForceShadow;

		int m_lRenderCount;
		int m_lPrevRenderCount;

		int m_lCalcScaleMatrixCount;
		cVector3f m_vCalcScale;

		bool m_bRendered;
		float m_fZ;
	};
};
#endif