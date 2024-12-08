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

	//UpdatePlayerRotation();

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
	}

}

void PlayerStateWalk::UpdatePlayerRotation()
{
	const XMFLOAT3& moveDirection = m_Player->GetMoveDirection();

	// �ړ����͂�����ꍇ�ɉ�]���X�V
	if (moveDirection.x != 0.0f || moveDirection.y != 0.0f)
	{
		const XMFLOAT3& cameraForward = m_Camera->GetForward();
		const XMFLOAT3& cameraRight = m_Camera->GetRight();
		
		float moveX = moveDirection.x * cameraRight.x + moveDirection.z * cameraForward.x;
		float moveZ = moveDirection.x * cameraRight.z + moveDirection.z * cameraForward.z;

		// ���K�����ĕ����x�N�g��
		XMVECTOR moveVector = XMVectorSet(moveX, 0.0f, moveZ, 0.0f);
		moveVector = XMVector3Normalize(moveVector);

		XMFLOAT3 normalizeMove;
		XMStoreFloat3(&normalizeMove, moveVector);

		m_Player->SetMoveDirection(XMFLOAT3(normalizeMove.x, 0.0f,normalizeMove.z));

		// ��]�X�V
		float yaw = atan2f(normalizeMove.x, normalizeMove.z);
		XMFLOAT3 playerRotation = m_Player->GetRotation();
		playerRotation.y = yaw;
		m_Player->SetRotation(playerRotation);
	}
}
