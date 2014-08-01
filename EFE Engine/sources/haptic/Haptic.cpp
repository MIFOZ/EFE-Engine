#include "haptic/Haptic.h"
#include "system/LowLevelSystem.h"
#include "haptic/LowLevelHaptic.h"

namespace efe
{
	bool cHaptic::m_bIsUsed = false;

	//////////////////////////////////////////////////////////////
	// CONSTRUCTERS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cHaptic::cHaptic(iLowLevelHaptic *a_pLowLevelHaptic) : iUpdateable("EFE_Haptic")
	{
		m_pLowLevelHaptic = a_pLowLevelHaptic;
	}

	//--------------------------------------------------------------

	cHaptic::~cHaptic()
	{
		Log("Exiting Haptic Module\n");
		Log("-----------------------------------------------------\n");

		Log("-----------------------------------------------------\n\n");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cHaptic::Init(cResources *a_pResources)
	{
		if (m_bIsUsed == false) return;

		Log("Initializing Haptic Module\n");
		Log("-----------------------------------------------------\n");

		if (m_pLowLevelHaptic->Init(a_pResources) == false)
			SetIsUsed(false);

		Log("-----------------------------------------------------\n\n");
	}

	//--------------------------------------------------------------

	void cHaptic::Update(float a_fTimeStep)
	{
		if (m_bIsUsed == false) return;

		m_pLowLevelHaptic->Update(a_fTimeStep);
	}
};