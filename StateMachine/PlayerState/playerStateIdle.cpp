#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Camera/camera.h"
#include "Manager/inputManager.h"
#include "System\Renderer\animationModel.h"
#include "System\Enum\playerStateEnum.h"
#include "playerStateIdle.h"

void PlayerStateIdle::Init()
{
	if (!m_Player)return;
	m_Player->SetMoveForwardDirection(0);
	m_Player->SetMoveRightDirection(0);
	m_AnimationModel->SetNextAnimation("Idle");
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

void PlayerStateIdle::UpdateAnimation()
{
	if (!m_AnimationModel)return;
	const char* curAnimationName = m_AnimationModel->GetCurrentAnimationName().c_str();
	const char* nextAnimationName = m_AnimationModel->GetNextAnimationName().c_str();
	//	‘JˆÚŠ®¬‚µ‚½‚ç
	if (m_AnimationModel->GetBlendRatio() >= 1)
	{
		m_AnimationModel->SetIsTransitioning(false);
		m_AnimationModel->SetCurrentAnimation("Idle");
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
