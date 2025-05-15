/*===================================================================================

プレイヤーIdle状態処理(playerStateIdle.cpp)

====================================================================================*/
#include "GameObject/Character/Player/playerh.h"
#include "Manager/inputManager.h"
#include "System/Renderer/animationModel.h"
#include "System/Enum/playerStateEnum.h"
#include "playerStateIdle.h"

//	===================プレイヤーIdle状態初期化======================
void PlayerStateIdle::Init()
{
	if (!m_Player)return;
	m_Player->SetMoveForwardDirection(0);
	m_Player->SetMoveRightDirection(0);
	m_AnimationModel->SetNextAnimation("Player_Idle");
}

//	===================プレイヤーIdle状態更新======================
void PlayerStateIdle::Update()
{
	//	ユーザー入力
	UserInputDection();
}

//	===================ユーザー入力管理======================
void PlayerStateIdle::UserInputDection()
{
	if (InputManager::GetKeyTrigger('A') || InputManager::GetKeyTrigger('W') ||
		InputManager::GetKeyTrigger('S') || InputManager::GetKeyTrigger('D'))
	{
		m_Player->ChangeState(Player_State::Walk);
	}

}

