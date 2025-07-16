/*===================================================================================

インプットマネージャー(inputManager.cpp)

====================================================================================*/
#include "main.h"
#include "Manager/inputManager.h"

BYTE InputManager::m_OldKeyState[256];
BYTE InputManager::m_KeyState[256];
POINT InputManager::m_CurrentMousePos;
POINT InputManager::m_LastMousePos;
POINT InputManager::m_MouseDelta;
bool	InputManager::m_IsInputEnable = false;

//	===================インプットマネージャー初期化======================
void InputManager::Init()
{
	//	キー状態初期化
	memset(m_OldKeyState, 0, 256);
	memset(m_KeyState, 0, 256);
	GetCursorPos(&m_CurrentMousePos);
	m_LastMousePos = m_CurrentMousePos;
	ScreenToClient(GetWindow(), &m_CurrentMousePos);
	m_MouseDelta.x = 0;
	m_MouseDelta.y = 0;

}

//	===================インプットマネージャーUninit======================
void InputManager::Uninit()
{
	
}

//	===================インプットマネージャー更新======================
void InputManager::Update()
{
	//	キー入力可能状態
	if (m_IsInputEnable)
	{
		//	マウス位置取る
		GetCursorPos(&m_CurrentMousePos);
		m_MouseDelta.x = m_CurrentMousePos.x - m_LastMousePos.x;
		m_MouseDelta.y = m_CurrentMousePos.y - m_LastMousePos.y;
	}
	//	キー状態コピー
	memcpy(m_OldKeyState, m_KeyState, 256);
	(void)GetKeyboardState(m_KeyState);
}

//	===================１フレーム前のマウス位置記録======================
void InputManager::UpdateLastMousePos()
{

	m_LastMousePos = m_CurrentMousePos;
}

//	===================キー長押し状態======================
//	KeyCode	:	BYTE	キーインプット
bool InputManager::GetKeyPress(BYTE KeyCode)
{

	return (m_KeyState[KeyCode] & 0x80);
}

//	===================キートリッカー状態======================
//	KeyCode	:	BYTE	キーインプット
bool InputManager::GetKeyTrigger(BYTE KeyCode)
{

	return ((m_KeyState[KeyCode] & 0x80) && !(m_OldKeyState[KeyCode] & 0x80));
}

//	===================全部のキーが押されたか======================
bool InputManager::GetAllKeyInput()
{
	for (int i = 0; i < 256; ++i)
	{
		if (m_KeyState[i] & 0x80)
		{
			return true;
		}
	}
	return false;
}

//	===================キーインプット可能状態======================
//	Flag	:	bool	アクティブフラグ
void InputManager::SetIsInputEnable(bool Flag)
{
	m_IsInputEnable = Flag;
	ResetInput();
}
//	===================入力全部リセット======================
void InputManager::ResetInput()
{
	m_MouseDelta.x = 0;
	m_MouseDelta.y = 0;
	m_CurrentMousePos.x = 0;
	m_CurrentMousePos.y = 0;
	m_LastMousePos.x = 0;
	m_LastMousePos.y = 0;
}
