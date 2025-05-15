/*===================================================================================

プレイヤー移動状態処理(playerStateWalk.cpp)

====================================================================================*/
#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Camera/camera.h"
#include "Manager/inputManager.h"
#include "System/Renderer/animationModel.h"
#include "System/Enum/playerStateEnum.h"
#include "playerStateWalk.h"

//	===================プレイヤー移動状態初期化======================
void PlayerStateWalk::Init()
{
	if (!m_Player)return;
	if (!m_Camera)return;
	UserInputDection();
	m_AnimationModel->SetNextAnimation("Player_Run");
}

void PlayerStateWalk::Update()
{
	if (!m_Player)return;
	if (!m_Camera)return;
	//	ユーザー入力
	UserInputDection();
}

//	===================ユーザー入力管理======================
void PlayerStateWalk::UserInputDection()
{
	//	移動方向初期化
	m_Player->SetMoveDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));

	bool hasInput = false;
	
	//	何キー押されたのか
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
	//	キー押されてないなら
	if (!hasInput)
	{
		m_Player->ChangeState(Player_State::Idle);
		//	アニメーション遷移
		if (!m_AnimationModel->GetIsTransitioning())
		{
			m_AnimationModel->SetNextAnimationFrame(m_AnimationModel->GetNextAnimationFrame());
		}
	}

}


