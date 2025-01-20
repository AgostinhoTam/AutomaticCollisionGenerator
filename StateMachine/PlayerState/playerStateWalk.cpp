#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Camera/camera.h"
#include "Manager/inputManager.h"
#include "System\Renderer\animationModel.h"
#include "System\Enum\playerStateEnum.h"
#include "playerStateWalk.h"

void PlayerStateWalk::Init()
{
	if (!m_Player)return;
	if (!m_Camera)return;
	UserInputDection();
	m_AnimationModel->SetNextAnimation("Run");
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
		if (!m_AnimationModel->GetIsTransitioning())
		{
			m_AnimationModel->SetNextAnimationFrame(m_AnimationModel->GetNextAnimationFrame());
		}
	}

}

void PlayerStateWalk::UpdateAnimation()
{
	if (!m_AnimationModel)return;

	//	‘JˆÚŠ®¬‚µ‚½‚ç
	if (m_AnimationModel->GetBlendRatio() >= 1)
	{
		m_AnimationModel->SetIsTransitioning(false);
		m_AnimationModel->SetCurrentAnimation(m_AnimationModel->GetNextAnimationName());
		m_AnimationModel->SetCurrentAnimationFrame(m_AnimationModel->GetNextAnimationFrame());
		m_AnimationModel->SetBlendRatio(0);
	}
	m_AnimationModel->Update();

	//	‘JˆÚ’†‚¾‚Á‚½‚ç
	if (m_AnimationModel->GetIsTransitioning())
	{
		m_AnimationModel->AddBlendRatio();
		m_AnimationModel->AddCurrentAnimationFrame();
		m_AnimationModel->AddNextAnimationFrame();
	}
	//	•’Ê‚ÌÄ¶
	else if (!m_AnimationModel->GetIsTransitioning())
	{
		m_AnimationModel->AddCurrentAnimationFrame();
	}
}
