#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Camera/camera.h"
#include "Manager/inputManager.h"
#include "Renderer\animationModel.h"
#include "Enum\playerStateEnum.h"
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
		const unsigned int nextFrame = m_AnimationModel->GetNextAnimationFrame();
		m_AnimationModel->SetCurrentAnimationFrame(nextFrame);
		m_AnimationModel->SetBlendRatio(0);
	}
	m_AnimationModel->Update(curAnimationName, m_AnimationModel->GetCurrentAnimationFrame(), nextAnimationName, m_AnimationModel->GetNextAnimationFrame(), m_AnimationModel->GetBlendRatio());
	//	‘JˆÚ’†‚¾‚Á‚½‚ç
	if (m_AnimationModel->GetIsTransitioning())
	{

		m_AnimationModel->AddBlendRatio(0.01f);
		m_AnimationModel->AddCurrentAnimationFrame(1);
		m_AnimationModel->AddNextAnimationFrame(1);
	}
	//	•’Ê‚ÌÄ¶
	else if (!m_AnimationModel->GetIsTransitioning())
	{
		m_AnimationModel->AddCurrentAnimationFrame(1);
	}


}
