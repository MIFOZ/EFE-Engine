#ifndef EFE_HAPTIC_H
#define EFE_HAPTIC_H

#include "game/Updateable.h"
#include "haptic/HapticTypes.h"

namespace efe
{
	class iLowLevelHaptic;
	class cResources;

	class cHaptic : public iUpdateable
	{
	public:
		cHaptic(iLowLevelHaptic *a_pLowLevelHaptic);
		~cHaptic();

		static void SetIsUsed(bool a_bX) {m_bIsUsed = a_bX;}
		static bool GetIsUsed() {return m_bIsUsed;}

		void Init(cResources *a_pResources);

		void Update(float a_fTimeStep);

		iLowLevelHaptic *GetLowLevel() {return m_pLowLevelHaptic;}

	private:
		iLowLevelHaptic *m_pLowLevelHaptic;

		static bool m_bIsUsed;
	};
};
#endif