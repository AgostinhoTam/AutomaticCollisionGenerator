
#include "Main/main.h"
#include "Manager/inputManager.h"


BYTE InputManager::m_OldKeyState[256];
BYTE InputManager::m_KeyState[256];
POINT InputManager::m_CurrentMousePos;
POINT InputManager::m_LastMousePos;
POINT InputManager::m_MouseDelta;

void InputManager::Init()
{

	memset( m_OldKeyState, 0, 256 );
	memset( m_KeyState, 0, 256 );
	GetCursorPos(&m_CurrentMousePos);
	m_LastMousePos = m_CurrentMousePos;
	ScreenToClient(GetWindow(), &m_CurrentMousePos);
	m_MouseDelta.x = 0.0f;
	m_MouseDelta.y = 0.0f;

}

void InputManager::Uninit()
{


}

void InputManager::Update()
{
	GetCursorPos(&m_CurrentMousePos);

	m_MouseDelta.x = m_CurrentMousePos.x - m_LastMousePos.x;
	m_MouseDelta.y = m_CurrentMousePos.y - m_LastMousePos.y;

	memcpy( m_OldKeyState, m_KeyState, 256 );

	(void)GetKeyboardState( m_KeyState );

}

void InputManager::UpdateLastMousePos()
{
	m_LastMousePos = m_CurrentMousePos;
}

bool InputManager::GetKeyPress(BYTE KeyCode)
{
	return (m_KeyState[KeyCode] & 0x80);
}

bool InputManager::GetKeyTrigger(BYTE KeyCode)
{
	return ((m_KeyState[KeyCode] & 0x80) && !(m_OldKeyState[KeyCode] & 0x80));
}

bool InputManager::GetAllKeyInput()
{

	return false;
}
