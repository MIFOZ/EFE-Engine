#ifndef EFE_LOW_LEVEL_HAPTIC_H
#define EFE_LOW_LEVEL_HAPTIC_H

#include "math/MathTypes.h"
#include "haptic/HapticTypes.h"

namespace efe
{
	class iHapticShape;
	class iHapticForce;
	class iHapticSurface;

	class iVertexBuffer;
	class cCamera3D;
	class cResources;
	class iPhysicsBody;

	class iLowLevelHaptic
	{
	public:
		iLowLevelHaptic();
		virtual ~iLowLevelHaptic();

		//************************************
		// Method:    Update
		// FullName:  efe::iLowLevelHaptic::Update
		// Access:    public 
		// Returns:   void
		// Qualifier:
		// Parameter: float a_fTimeStep
		//************************************
		void Update(float a_fTimeStep);
		bool Init(cResources *a_pResources);

		void DestroyAllShapes();
		void DestroyAll();

		virtual float GetMaxForce()=0;

		virtual void SetWorldScale(float a_fScale)=0;
		virtual float GetWorldScale()=0;

		virtual void SetRenderingActive(bool a_bX)=0;
		virtual bool IsRenderingActive()=0;

		virtual bool ShapeIsInContact(iHapticShape *a_pShape)=0;

		void SetCamera(cCamera3D *a_pCamera) {m_pCamera = a_pCamera;}
		cCamera3D *GetCamera() {return m_pCamera;}
		void SetCameraOffset(const cVector3f &a_vOffset) {m_vCameraOffset = a_vOffset;}
		cVector3f GetCameraOffset() {return m_vCameraOffset;}

		void SetUpdateShapes(bool a_bX) {m_bUpdateShapes = a_bX;}
		bool GetUpdateShapes() {return m_bUpdateShapes;}

		void SetForceMul(float a_fX) {m_fForceMul = a_fX;}
		float GetForceMul() {return m_fForceMul;}

		virtual bool ButtonIsPressed(int a_lNum)=0;

		virtual cVector3f GetHardwarePosition()=0;
		virtual cVector3f GetHardwareRotation()=0;

		cVector3f GetHardwarePosDelta();

		cVector2f GetProxyScreenPos(const cVector2f &a_vScreenPos);
		cVector2f GetProxyScreenDeltaPos(const cVector2f &a_vScreenPos);

		cVector2f GetRelativeVirtualMousePos();
		cVector2f GetRelativeMousePos();
		void SetVirtualMousePosBound(const cVector2f &a_vMin, const cVector2f &a_vMax,
			const cVector2f &a_vScreenSize);

		virtual cVector3f GetProxyPosition()=0;
		virtual void SetProxyRadius(float a_fRadius)=0;
		virtual float SetProxyRadius()=0;

		virtual int GetNumberOfButtons()=0;

		virtual iHapticSurface *CreateSimpleSurface(const tString &a_sName, float a_fDamping = 0.0f, float a_fStiffness = 0.9f)=0;

		virtual iHapticSurface *CreateFrictionalSurface(const tString &a_sName, float a_fDamping = 0.0f, float a_fStiffness = 0.9f,
			float a_fDynamicFriction = 0.2f, float a_fStartingFriction = 0.3f,
			float a_fTangentStiffness = 0.7f, float a_fStoppingFriction = 0.1f)=0;

		virtual iHapticSurface *CreateRoughSurface(const tString &a_sName, 
			float a_fDeviation = 0.1f, float a_fMean = 0.5f,
			float a_fDamping = 0.0f, float a_fStiffness = 0.9f,
			float a_fDynamicFriction = 0.2f, float a_fStartingFriction = 0.3f,
			float a_fTangentStiffness = 0.7f, float a_fStoppingFriction = 0.1f)=0;

		virtual iHapticSurface *CreateStickySurface(const tString &a_sName, 
			float a_fDeadHeight = 0.004f, float a_fStickyStiffness = 0.5f,
			float a_fDamping = 0.0f, float a_fStiffness = 0.9f,
			float a_fDynamicFriction = 0.2f, float a_fStartingFriction = 0.3f,
			float a_fTangentStiffness = 0.7f, float a_fStoppingFriction = 0.1f)=0;

		iHapticSurface *GetSurfaceFromName(const tString &a_sName);

		virtual iHapticShape *CreateBoxShape(const tString &a_sName, const cVector3f &a_vSize, cMatrixf *a_pOffsetMtx)=0;
		virtual iHapticShape *CreateSphereShape(const tString &a_sName, const cVector3f &a_vRadii, cMatrixf *a_pOffsetMtx)=0;
		virtual iHapticShape *CreateCylinderShape(const tString &a_sName, float a_fRadius, float a_fHeight, cMatrixf *a_pOffsetMtx)=0;
		virtual iHapticShape *CreateCapsuleShape(const tString &a_sName, float a_fRadius, float a_fHeight, cMatrixf *a_pOffsetMtx)=0;
		virtual iHapticShape *CreateMeshShape(const tString &a_sName, iVertexBuffer *a_pVtxBuffer)=0;
		virtual iHapticShape *CreateCompoundShape(const tString &a_sName, tHapticShapeVec &a_vShapes)=0;

		virtual iHapticShape *CreateShapeFromPhysicsBody(const tString &a_sName, iPhysicsBody *a_pBody)=0;

		void DestroyShape(iHapticShape *a_pShape);
		bool ShapeExists(iHapticShape *a_pShape);

		cHapticShapeIterator GetShapeIterator();

		virtual iHapticForce *CreateImpulseForce(const cVector3f &a_vForce)=0;
		virtual iHapticForce *CreateSinusWaveForce(const cVector3f &a_vDirection, float a_fAmp, float a_fFreq)=0;
		virtual iHapticForce *CreateSawWaveForce(const cVector3f &a_vDirection, float a_fAmp, float a_fFreq)=0;
		virtual iHapticForce *CreateSpringForce(const cVector3f &a_Position, float a_fStiffness, float a_fDamping)=0;
		virtual iHapticForce *CreateViscosityForce(const cVector3f &a_vVelocity, float a_fMass, float a_fStiffness, float a_fDamping)=0;

		void DestroyForce(iHapticForce *a_pForce);

		void StopAllForces();

	protected:
		virtual bool InitLowLevel()=0;
		virtual void UpdateLowLevel(float a_fTimeStep)=0;

		cResources *m_pResources;

		cCamera3D *m_pCamera;
		cVector3f m_vCameraOffset;

		tHapticSurfaceMap m_mapSurfaces;
		tHapticForceList m_lstForces;
		tHapticShapeList m_lstShapes;

		float m_fForceMul;

		cVector3f m_vPreviousHardwarePos;
		cVector3f m_vCurrentHardwarePos;

		cVector2f m_vPreviousScreenPos;
		cVector2f m_vCurrentScreenPos;
		bool m_bSreenPosFirstTime;

		cVector2f m_vMinMousePos;
		cVector2f m_vMaxMousePos;
		cVector2f m_vScreenSize;

		bool m_bUpdateShapes;
	};
};
#endif