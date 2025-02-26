#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Camera/camera.h"
#include "Manager/inputManager.h"
#include "System\Renderer\animator.h"
#include "System\Enum\playerStateEnum.h"
#include "playerStateIdle.h"

PlayerStateIdle::PlayerStateIdle(Player* player, Camera* camera, const std::string& AnimationName):PlayerState(player, camera, AnimationName)
{
	m_Animator = m_Player->GetAnimator(); 
}

void PlayerStateIdle::Init()
{
	if (!m_Player)return;
	if (!m_Animator)return;

	m_Player->SetMoveForwardDirection(0);
	m_Player->SetMoveRightDirection(0);
	m_Animator->SetNextAnimation(m_AnimationName);
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

