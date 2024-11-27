#include "GameObject/Character/Player/playerh.h"
#include "Manager/inputManager.h"
#include "playerStateWalk.h"

void PlayerStateWalk::Init()
{
	if (!m_Player)return;

	UserInputDection();
}

void PlayerStateWalk::Update()
{
	if (!m_Player)return;

	UserInputDection();
}

void PlayerStateWalk::UserInputDection()
{
	bool hasInput = false;
	if (InputManager::GetKeyPress('A'))
	{
		m_Player->SetMoveRightDirection(-1);
		hasInput = true;
	}
	if (InputManager::GetKeyPress('D'))
	{
		m_Player->SetMoveRightDirection(1);
		hasInput = true;
	}
	if (InputManager::GetKeyPress('W'))
	{
		m_Player->SetMoveForwardDirection(1);
		hasInput = true;
	}
	if (InputManager::GetKeyPress('S'))
	{
		m_Player->SetMoveForwardDirection(-1);
		hasInput = true;
	}

	if (!hasInput)
	{
		m_Player->ChangeState(PLAYER_STATE::IDLE);
	}

}
