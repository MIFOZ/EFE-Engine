#include "ai/AI.h"

#include "ai/AINodeGenerator.h"
#include "system/LowLevelSystem.h"
//#include "system/MemoryManager.h"

namespace efe
{
	cAI::cAI() : iUpdateable("EFE_AI")
	{
		m_pAINodeGenerator = efeNew(cAINodeGenerator, ());
	}

	cAI::~cAI()
	{
		efeDelete(m_pAINodeGenerator);
	}

	void cAI::Reset()
	{
		
	}

	void cAI::Update(float a_fTimeStep)
	{
		
	}

	void cAI::Init()
	{
		
	}
}