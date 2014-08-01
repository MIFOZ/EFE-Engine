#ifndef EFE_SCRIPT_FUNC_H
#define EFE_SCRIPT_FUNC_H

#include "physics/PhysicsJoint.h"

namespace efe
{
	class cGraphics;
	class cResources;
	class cSystem;
	class cSound;
	class cScene;
	class cInput;
	class cGame;

	class cScriptJointCallback : public iPhysicsJointCallback
	{
	public:
		cScriptJointCallback(cScene *a_pScene);

		void OnMinLimit(iPhysicsJoint *a_pJoint);
		void OnMaxLimit(iPhysicsJoint *a_pJoint);

		bool IsScript(){return true;}

		tString m_sMaxFunc;
		tString m_sMinFunc;

		cScene *m_pScene;
	};

	class cScriptFunc
	{
	public:
		static void Init(cGraphics *a_pGraphics,
			cResources *a_pResources,
			cSystem *a_pSystem,
			cInput *a_pInput,
			cScene *a_pScene,
			cSound *a_pSound,
			cGame *a_pGame);
	};
};
#endif