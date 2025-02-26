#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Camera/camera.h"
#include "Manager/inputManager.h"
#include "System\Enum\playerStateEnum.h"
#include "System\Renderer\animationModelInstance.h"
#include "playerStateWalk.h"

PlayerStateWalk::PlayerStateWalk(Player* player, Camera* camera, const std::string& AnimationName) :PlayerState(player, camera, AnimationName)
{
	m_Animator = m_Player->GetAnimator();
}


void PlayerStateWalk::Init()
{
	if (!m_Player)return;
	if (!m_Camera)return;
	UserInputDection();
	m_Player->GetAnimationModel()->SetNextAnimation(m_AnimationName);
}

void PlayerStateWalk::Update()
{
	if (!m_Player)return;
	if (!m_Camera)return;

	UserInputDection();


}

void PlayerStateWalk::UserInputDection()
{
	m_Player->SetMoveDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));

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
		Animator* animator = m_Player.getanim
			if (!m_Player->GetAnimationModel()->GetIsTransitioning())
			{
				m_Player->GetAnimationModel()->SetNextAnimationFrame(m_AnimationModel->GetNextAnimationFrame());
			}
	}

}


