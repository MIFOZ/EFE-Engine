#ifndef EFE_AI_H
#define EFE_AI_H

#include "system/SystemTypes.h"
#include "game/GameTypes.h"

#include "game/Updateable.h"

namespace efe
{
	class cAINodeGenerator;

	class cAI : public iUpdateable
	{
	public:
		cAI();
		~cAI();

		void Reset();
		void Update(float a_fTimeStep);

		void Init();

		cAINodeGenerator *GetNodeGenerator(){return m_pAINodeGenerator;}

	private:
		cAINodeGenerator *m_pAINodeGenerator;
	};
};
#endif