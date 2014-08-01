#ifndef EFE_INPUT_H
#define EFE_INPUT_H

#include <map>
#include <list>

#include "system/SystemTypes.h"
#include "game/Updateable.h"
#include "input/InputTypes.h"

namespace efe
{
	class iKeyboard;
	class iMouse;
	class iJoystick;
	class iLowLevelInput;
	class iInputDevice;
	class iAction;

	typedef std::map<tString, iAction*> tActionMap;
	typedef tActionMap::iterator tActionMapIt;

	typedef std::multimap<tString, iAction*> tActionMultiMap;
	typedef tActionMultiMap::iterator tActionMultiMapIt;

	typedef std::list<iInputDevice*> tInputDeviceList;
	typedef tInputDeviceList::iterator tInputDeviceListIt;

	class cInput : public iUpdateable
	{
	public:
		cInput(iLowLevelInput *a_pLowLevelInput);
		~cInput();

		void Update(float a_fTimeStep);

		void AddAction(iAction *a_pAction);

		bool IsTriggered(tString a_sName);

		bool WasTriggered(tString a_sName);

		bool BecameTriggered(tString a_sName);

		bool DoubleTriggered(tString a_sName, float a_fLimit);

		iKeyboard *GetKeyboard();

		iMouse *GetMouse();

		iAction *GetAction(tString a_sName);
		
		iLowLevelInput *GetLowLevel();

	private:
		tActionMap m_mapActions;
		tInputDeviceList m_lstInputDevices;

		iLowLevelInput *m_pLowLevelInput;

		iMouse *m_pMouse;
		iKeyboard *m_pKeyboard;
		iJoystick *m_pJoystick;
	};
};
#endif