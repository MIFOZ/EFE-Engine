#ifndef EFE_SURFACE_DATA_H
#define EFE_SURFACE_DATA_H

#include "math/MathTypes.h"
#include "physics/PhysicsMaterial.h"

namespace efe
{
	#define eRollFlagAxis_X		1
	#define eRollFlagAxis_Y		2
	#define eRollFlagAxis_Z		4

	class cPhysics;
	class iPhysicsWorld;
	class iPhysicsBody;
	class cResources;
	class iHapticSurface;

	class cSurfaceImpactData
	{
		friend class cSurfaceData;
	public:
		float GetMinSpeed(){return m_fMinSpeed;}

		const tString &GetSoundName(){return m_sSoundName;}
		void SetSoundName(const tString &a_sName){m_sSoundName = a_sName;}

		const tString &GetPSName(){return m_sPSName;}
		void SetPSName(const tString &a_sName){m_sPSName = a_sName;}

		int GetPSPrio(){return m_lPSPrio;}
		void SetPSPrio(int a_lPrio){m_lPSPrio = a_lPrio;}
	private:
		float m_fMinSpeed;
		tString m_sSoundName;
		tString m_sPSName;
		int m_lPSPrio;
	};

	typedef std::vector<cSurfaceImpactData*> tSurfaceImpactDataVec;
	typedef tSurfaceImpactDataVec::iterator tSurfaceImpactDataVecIt;

	class cSurfaceData
	{
	public: 
		cSurfaceData(const tString &a_sName, cPhysics *a_pPhysics, cResources *a_pResources);
		~cSurfaceData();

		const tString &GetName() const {return m_sName;}

		void OnImpact(float a_fSpeed, const cVector3f &a_vPos, int a_lContacts, iPhysicsBody *a_pBody);
		void OnSlide(float a_fSpeed, const cVector3f &a_vPos, int a_lContacts, iPhysicsBody *a_pBody,
			iPhysicsBody *a_pSlideAgainstBody);
		void CreateImpactEffect(float a_fSpeed, const cVector3f &a_vPos, int a_lContacts,
			cSurfaceData *a_pSecondSurface);

		void UpdateRollEffect(iPhysicsBody *a_pBody);

		void SetElasticity(float a_fElasticity);
		float GetElasticity();
		void SetStaticFriction(float a_fFriction);
		float GetStaticFriction() const;
		void SetKineticFriction(float a_fFriction);
		float GetKineticFriction() const;

		void SetPriority(int a_lPriority);
		int GetPriority() const;

		void SetFrictionCombMode(ePhysicsMaterialCombMode a_Mode);
		ePhysicsMaterialCombMode GetFrictionCombMode() const;
		void SetElasticityCombMode(ePhysicsMaterialCombMode a_Mode);
		ePhysicsMaterialCombMode GetElasticityCombMode() const;

		const tString &GetStepType() {return m_sStepType;}
		void SetStepType(const tString &a_sX) {m_sStepType = a_sX;}

		void SetMinScrapeSpeed(float a_fX) {m_fMinScrapeSpeed = a_fX;}
		void SetMinScrapeFreq(float a_fX) {m_fMinScrapeFreq = a_fX;}
		void SetMinScrapeFreqSpeed(float a_fX) {m_fMinScrapeFreqSpeed = a_fX;}
		void SetMaxScrapeFreq(float a_fX) {m_fMaxScrapeFreq = a_fX;}
		void SetMaxScrapeFreqSpeed(float a_fX) {m_fMaxScrapeFreqSpeed = a_fX;}
		void SetMiddleScrapeSpeed(float a_fX) {m_fMiddleScrapeSpeed = a_fX;}
		void SetMinScrapeContacts(int a_lX) {m_lMinScrapeContacts = a_lX;}
		void SetScrapeSoundName(const tString &a_sName) {m_sScrapeSoundName = a_sName;}

		void SetMinRollSpeed(float a_fX) {m_fMinRollSpeed = a_fX;}
		void SetMinRollFreq(float a_fX) {m_fMinRollFreq = a_fX;}
		void SetMinRollVolume(float a_fX) {m_fMinRollVolume = a_fX;}
		void SetMinRollFreqSpeed(float a_fX) {m_fMinRollFreqSpeed = a_fX;}
		void SetMaxRollFreq(float a_fX) {m_fMaxRollFreq = a_fX;}
		void SetMaxRollVolume(float a_fX) {m_fMaxRollVolume = a_fX;}
		void SetMaxRollFreqSpeed(float a_fX) {m_fMaxRollFreqSpeed = a_fX;}
		void SetMiddleRollSpeed(float a_fX) {m_fMiddleRollSpeed = a_fX;}
		void SetRollSoundName(const tString &a_sName) {m_sRollSoundName = a_sName;}
		void SetRollAxisFlags(tFlag a_AxisFlags) {m_RollAxisFlags = a_AxisFlags;}

		void PreloadData();

		iPhysicsMaterial *ToMaterial(iPhysicsWorld *a_pWorld);

		cSurfaceImpactData *CreateImpactData(float a_fMinSpeed);
		cSurfaceImpactData *GetImpactData(int a_lIdx);
		int GetImpactDataNum();

		cSurfaceImpactData *GetImpactDataFromSpeed(float a_fSpeed);

		cSurfaceImpactData *CreateHitData(float a_fMinSpeed);
		cSurfaceImpactData *GetHitData(int a_lIdx);
		int GetHitDataNum();

		cSurfaceImpactData *GetHitDataFromSpeed(float a_fSpeed);

	protected:
		cResources *m_pResources;
		cPhysics *m_pPhysics;
		tString m_sName;

		ePhysicsMaterialCombMode m_FrictionMode;
		ePhysicsMaterialCombMode m_ElasticityMode;

		float m_fElasticity;
		float m_fStaticFriction;
		float m_fKineticFriction;

		int m_lPriority;

		float m_fMinScrapeSpeed;
		float m_fMinScrapeFreq;
		float m_fMinScrapeFreqSpeed;
		float m_fMaxScrapeFreq;
		float m_fMaxScrapeFreqSpeed;
		float m_fMiddleScrapeSpeed;
		int m_lMinScrapeContacts;
		tString m_sScrapeSoundName;

		float m_fMinRollSpeed;
		float m_fMinRollFreq;
		float m_fMinRollVolume;
		float m_fMinRollFreqSpeed;
		float m_fMaxRollFreq;
		float m_fMaxRollVolume;
		float m_fMaxRollFreqSpeed;
		float m_fMiddleRollSpeed;
		int m_lMinRollContacts;
		tString m_sRollSoundName;
		tFlag m_RollAxisFlags;

		tString m_sStepType;

		tSurfaceImpactDataVec m_vImpactData;

		tSurfaceImpactDataVec m_vHitData;


	};
};
#endif