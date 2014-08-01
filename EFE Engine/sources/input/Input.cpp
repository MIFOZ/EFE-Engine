#include "input/Input.h"
#include "system/LowLevelSystem.h"
#include "input/Mouse.h"
#include "input/Keyboard.h"
#include "input/Joystick.h"
#include "input/LowLevelInput.h"
#include "input/Action.h"
#include "input/ActionKeyboard.h"

namespace efe
{
	cInput::cInput(iLowLevelInput *a_pLowLevelInput) : iUpdateable("EFE_Input")
	{
		m_pLowLevelInput = a_pLowLevelInput;

		m_pKeyboard = m_pLowLevelInput->CreateKeyboard();
		m_pMouse = m_pLowLevelInput->CreateMouse();
		m_pJoystick = m_pLowLevelInput->CreateJoystick();

		m_lstInputDevices.push_back(m_pMouse);
		m_lstInputDevices.push_back(m_pKeyboard);
		m_lstInputDevices.push_back(m_pJoystick);
	}

	cInput::~cInput()
	{
		Log("Exitin Input Module\n");
		Log("-----------------------------------------------------\n");

		STLMapDeleteAll(m_mapActions);

		if (m_pKeyboard)efeDelete(m_pKeyboard);
		if (m_pMouse)efeDelete(m_pMouse);
		if (m_pJoystick)efeDelete(m_pJoystick);

		Log("-----------------------------------------------------\n\n");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cInput::Update(float a_fTimeStep)
	{
		m_pLowLevelInput->BeginInputUpdate();

		for (tInputDeviceListIt it = m_lstInputDevices.begin();it!=m_lstInputDevices.end();++it)
		{
			(*it)->Update();
		}

		m_pLowLevelInput->EndInputUpdate();

		for ( tActionMapIt it = m_mapActions.begin();it!=m_mapActions.end();++it)
			it->second->Update(a_fTimeStep);
	}

	//--------------------------------------------------------------

	void cInput::AddAction(iAction *a_pAction)
	{
		tString sName = a_pAction->GetName();
		tActionMap::value_type val = tActionMap::value_type(sName,a_pAction);
		m_mapActions.insert(val);
	}

	//--------------------------------------------------------------

	bool cInput::IsTriggered(tString a_sName)
	{
		iAction *pAction = GetAction(a_sName);
		if (pAction == NULL) return false;
		return pAction->IsTriggered();
	}

	//--------------------------------------------------------------

	bool cInput::WasTriggered(tString a_sName)
	{
		iAction *pAction = GetAction(a_sName);
		if (pAction==NULL) return false;

		return pAction->WasTriggered();
	}

	//--------------------------------------------------------------

	bool cInput::BecameTriggered(tString a_sName)
	{
		iAction *pAction = GetAction(a_sName);
		if (pAction==NULL) return false;

		return pAction->BecameTriggered();
	}

	//--------------------------------------------------------------

	bool cInput::DoubleTriggered(tString a_sName, float a_fLimit)
	{
		iAction *pAction = GetAction(a_sName);
		if (pAction==NULL) return false;

		return pAction->DoubleTriggered(a_fLimit);
	}

	//--------------------------------------------------------------

	iKeyboard *cInput::GetKeyboard()
	{
		return m_pKeyboard;
	}

	//--------------------------------------------------------------

	iMouse *cInput::GetMouse()
	{
		return m_pMouse;
	}

	//--------------------------------------------------------------

	iAction *cInput::GetAction(tString a_sName)
	{
		tActionMapIt it = m_mapActions.find(a_sName);
		if (it == m_mapActions.end()) return NULL;

		return it->second;
	}

	//--------------------------------------------------------------

	iLowLevelInput *cInput::GetLowLevel()
	{
		return m_pLowLevelInput;
	}
}