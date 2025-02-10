#pragma once
#include <Windows.h>

class InputManager
{
private:
	static BYTE m_OldKeyState[256];
	static BYTE m_KeyState[256];
	static POINT m_CurrentMousePos;
	static POINT m_LastMousePos;
	static POINT m_MouseDelta;
	static bool	m_IsInputEnable;

public:
	static void Init();
	static void Uninit();
	static void Update();
	static void UpdateLastMousePos();
	static bool GetKeyPress( BYTE KeyCode );
	static bool GetKeyTrigger( BYTE KeyCode );
	static bool GetAllKeyInput();
	static bool GetIsInputEnable(){ return m_IsInputEnable; }
	static POINT& GetCurrentMousePos() { return m_CurrentMousePos; }
	static POINT& GetMouseDelta() { return m_MouseDelta; }
	static void SetIsInputEnable(bool Flag);
	static void ResetInput();
};
