#include "GameObject/Character/Player/playerh.h"
#include "Manager/inputManager.h"
#include "playerStateIdle.h"

void PlayerStateIdle::Init()
{
	if (!m_Player)return;

	m_Player->SetMoveForwardDirection(0);
	m_Player->SetMoveRightDirection(0);
}

void PlayerStateIdle::Update()
{
	UserInputDection();
}

void PlayerStateIdle::UserInputDection()
{
	if (InputManager::GetKeyTrigger('A') || InputManager::GetKeyTrigger('W') ||
		InputManager::GetKeyTrigger('S') || InputManager::GetKeyTrigger('D'))
	{
		m_Player->ChangeState(PLAYER_STATE::WALK);
	}

}
