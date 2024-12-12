#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Camera/camera.h"
#include "Manager/inputManager.h"
#include "Renderer\animationModel.h"
#include "Enum\playerStateEnum.h"
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
	const char* curAnimationName = m_AnimationModel->GetCurrentAnimationName().c_str();
	const char* nextAnimationName = m_AnimationModel->GetNextAnimationName().c_str();
	//	‘JˆÚŠ®¬‚µ‚½‚ç
	if (m_AnimationModel->GetBlendRatio() >= 1)
	{
		m_AnimationModel->SetIsTransitioning(false);
		m_AnimationModel->SetCurrentAnimation(m_AnimationModel->GetNextAnimationName());
		int nextFrame = m_AnimationModel->GetNextAnimationFrame();
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
