#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Camera/camera.h"
#include "Manager/inputManager.h"
#include "playerStateWalk.h"

void PlayerStateWalk::Init()
{
	if (!m_Player)return;
	if (!m_Camera)return;

	UserInputDection();
}

void PlayerStateWalk::Update()
{
	if (!m_Player)return;
	if (!m_Camera)return;

	UserInputDection();

	UpdatePlayerRotation();

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

void PlayerStateWalk::UpdatePlayerRotation()
{
	const XMFLOAT2& moveDirection = m_Player->GetMoveDirection();

	// 移動入力がある場合に回転を更新
	if (moveDirection.x != 0.0f || moveDirection.y != 0.0f)
	{
		const XMFLOAT3& cameraForward = m_Camera->GetForward();
		const XMFLOAT3& cameraRight = m_Camera->GetRight();
		
		float moveX = moveDirection.x * cameraRight.x + moveDirection.y * cameraForward.x;
		float moveZ = moveDirection.x * cameraRight.z + moveDirection.y * cameraForward.z;

		// 正規化して方向ベクトル
		XMVECTOR moveVector = XMVectorSet(moveX, 0.0f, moveZ, 0.0f);
		moveVector = XMVector3Normalize(moveVector);

		XMFLOAT3 normalizeMove;
		XMStoreFloat3(&normalizeMove, moveVector);

		m_Player->SetMoveDirection(XMFLOAT2(normalizeMove.x, normalizeMove.z));

		// 回転更新
		float yaw = atan2f(normalizeMove.x, normalizeMove.z);
		XMFLOAT3 playerRotation = m_Player->GetRotation();
		playerRotation.y = yaw;
		m_Player->SetRotation(playerRotation);
	}
}
