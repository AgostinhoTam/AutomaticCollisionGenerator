#include "Manager/animationRendererManager.h"
#include "Manager/sceneManager.h"
#include "Manager/shaderManager.h"
#include "Manager/inputManager.h"
#include "Manager/gameObjectManager.h"
#include "System\Enum\playerStateEnum.h"
#include "Scene/scene.h"
#include "System\Renderer/animationModel.h"
#include "StateMachine/PlayerState/playerStateIdle.h"
#include "StateMachine/PlayerState/playerStateWalk.h"
#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Camera/camera.h"
#include "GameObject\Character\Enemy\enemy.h"
#include "System\Collision\characterBoneCollision.h"
#include "System\Renderer\animator.h"
#include "System\Collision\sphereCollision.h"
constexpr float PLAYER_MAX_SPEED = 20.0f;
constexpr float PLAYER_MAX_ACCL_SPEED = 50.0f;
constexpr float PLAYER_MAX_JUMP_SPEED = 100.0f;
constexpr float PLAYER_SCALE = 0.01f;
void Player::Init()
{
	m_AnimationModel = AnimationRendererManager::LoadAnimationModel(MODEL_NAME::PLAYER,this);
	m_Animator = new Animator;
	m_Animator->LoadAnimation()
	m_Shader = ShaderManager::LoadShader(SHADER_NAME::UNLIT_SKINNING_TEXTURE);

	m_MaxMovementSpeed = PLAYER_MAX_SPEED;
	m_MaxHorizontalAcclSpeed = PLAYER_MAX_ACCL_SPEED;
	m_Name = "Player";

	Scene* scene = SceneManager::GetInstance()->GetCurrentScene();
	if (scene)
	{
		GameObjectManager* objectManager = scene->GetGameObjectManager();
		if (objectManager)
		{
			m_Camera = objectManager->GetGameObject<Camera>(GAMEOBJECT_TYPE::CAMERA);
			objectManager->GetGameObjectsByLayer<Enemy>(m_EnemyList,GAMEOBJECT_TYPE::ENEMY);
		}
	}

	m_PlayerState.reserve(static_cast<int>(PLAYER_STATE::MAX_STATE));
	m_PlayerState.try_emplace(PLAYER_STATE::IDLE, new PlayerStateIdle(this, m_Camera, m_AnimationModel));
	m_PlayerState.try_emplace(PLAYER_STATE::WALK, new PlayerStateWalk(this, m_Camera, m_AnimationModel));
	m_CurrentState = m_PlayerState[PLAYER_STATE::IDLE];
	m_CurrentState->Init();
	m_Scale = { PLAYER_SCALE,PLAYER_SCALE,PLAYER_SCALE };
	m_Position.y = 0.0f;
	//m_Collision = new SphereCollision(m_Position, { 0.0f,1.0f,0.0f }, 1.0f);
	m_IsGround = true;
	CreateCharacterBoneCollision(CHARACTER_BONE_TYPE::HUMANOID);

}

void Player::Uninit()
{
	m_AnimationModel->Uninit();
	if (m_Collision)
	{
		delete m_Collision;
		m_Collision = nullptr;
	}
}

void Player::Update(const float& DeltaTime)
{
	if (!m_CurrentState)return;
	if (!m_AnimationModel)return;
	if (!m_Camera)return;


	//	�A�j���[�V�����X�V
	m_AnimationModel->UpdateAnimationBlend();

	//if (m_Collision)m_Collision->UpdateCollision(m_Position);


	m_CurrentState->Update();

	UpdatePlayerRotation();

	// �ړ��X�V
	Character::Update(DeltaTime);
	UpdateBoneCollision();

}

void Player::Draw()
{
	if (!m_CurrentState)return;
	if (!m_AnimationModel)return;


	if (m_Collision)m_Collision->Draw();
	for (auto& capsule : m_Collisions)
	{
		if (!capsule.second)continue;
		capsule.second->Draw();
	}

	m_AnimationModel->Draw();

}

void Player::ChangeState(PLAYER_STATE State)
{
	//	����State�ɑJ�ڂ��Ȃ��悤��
	if (m_CurrentState == m_PlayerState[State])return;

	m_CurrentState = m_PlayerState[State];

	if (!m_CurrentState) return;

	m_CurrentState->Init();
}

void Player::UpdatePlayerRotation()
{
	CollisionCheck();
	// �ړ����͂�����ꍇ�ɉ�]���X�V
	if (m_MoveDirection.x != 0.0f || m_MoveDirection.z != 0.0f)
	{
		const XMFLOAT3& cameraForward = m_Camera->GetForward();
		const XMFLOAT3& cameraRight = m_Camera->GetRight();

		float moveX = m_MoveDirection.x * cameraRight.x + m_MoveDirection.z * cameraForward.x;
		float moveZ = m_MoveDirection.x * cameraRight.z + m_MoveDirection.z * cameraForward.z;

		// ���K�����ĕ����x�N�g��
		XMVECTOR moveVector = XMVectorSet(moveX, 0.0f, moveZ, 0.0f);
		moveVector = XMVector3Normalize(moveVector);

		XMFLOAT3 normalizeMove;
		XMStoreFloat3(&normalizeMove, moveVector);

		m_MoveDirection = XMFLOAT3(normalizeMove.x, 0.0f, normalizeMove.z);

		// ��]�X�V
		float yaw = atan2f(normalizeMove.x, normalizeMove.z);
		//yaw += XM_PI;
		m_Rotation.y = yaw;

	}
}

void Player::CollisionCheck()
{
	for (const auto& playerPair : m_Collisions)
	{
		if (!playerPair.second)continue;
		playerPair.second->SetIsHit(false);
	}
}

Enemy* Player::LockTarget()
{
	for (Enemy* enemy : m_EnemyList)
	{
		m_Camera->CheckView(enemy->GetPosition());		
	}
	return nullptr;
}
