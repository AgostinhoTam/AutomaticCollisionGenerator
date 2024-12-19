#include "Manager/animationRendererManager.h"
#include "Manager/sceneManager.h"
#include "Manager/shaderManager.h"
#include "Manager/inputManager.h"
#include "Manager/gameObjectManager.h"
#include "System\Enum\playerStateEnum.h"
#include "Scene/scene.h"
#include "System\Renderer/modelRenderer.h"
#include "System\Renderer/animationModel.h"
#include "StateMachine/PlayerState/playerStateIdle.h"
#include "StateMachine/PlayerState/playerStateWalk.h"
#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Camera/camera.h"
#include "GameObject\Character\Enemy\enemy.h"
#include "System\Collision\sphereCollision.h"
constexpr float PLAYER_MAX_SPEED = 20.0f;
constexpr float PLAYER_MAX_ACCL_SPEED = 50.0f;
constexpr float PLAYER_MAX_JUMP_SPEED = 100.0f;
constexpr float PLAYER_SCALE = 0.01f;
void Player::Init()
{
	m_AnimationModel = AnimationRendererManager::LoadAnimationModel(MODEL_NAME::PLAYER);

	m_Shader = ShaderManager::LoadShader(SHADER_NAME::UNLIT_TEXTURE);

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
	m_Collision = new SphereCollision(this, { 0.0f,0.0f,0.0f }, 5.0f);

	m_Position.y = 0.0f;
	m_IsGround = true;

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

	if (m_Collision)m_Collision->UpdateCollision();

	m_CurrentState->Update();

	UpdatePlayerRotation();

	Character::Update(DeltaTime);

}

void Player::Draw()
{
	if (!m_CurrentState)return;
	if (!m_AnimationModel)return;

	m_CurrentState->UpdateAnimation();
	m_AnimationModel->Draw(this);
#ifdef _DEBUG
	if (m_Collision)m_Collision->Draw();
#endif // _DEBUG

}

void Player::ChangeState(PLAYER_STATE State)
{
	//	“¯‚¶State‚É‘JˆÚ‚µ‚È‚¢‚æ‚¤‚É
	if (m_CurrentState == m_PlayerState[State])return;

	m_CurrentState = m_PlayerState[State];

	if (!m_CurrentState) return;

	m_CurrentState->Init();
}

void Player::UpdatePlayerRotation()
{

	// ˆÚ“®“ü—Í‚ª‚ ‚éê‡‚É‰ñ“]‚ðXV
	if (m_MoveDirection.x != 0.0f || m_MoveDirection.z != 0.0f)
	{
		const XMFLOAT3& cameraForward = m_Camera->GetForward();
		const XMFLOAT3& cameraRight = m_Camera->GetRight();

		float moveX = m_MoveDirection.x * cameraRight.x + m_MoveDirection.z * cameraForward.x;
		float moveZ = m_MoveDirection.x * cameraRight.z + m_MoveDirection.z * cameraForward.z;

		// ³‹K‰»‚µ‚Ä•ûŒüƒxƒNƒgƒ‹
		XMVECTOR moveVector = XMVectorSet(moveX, 0.0f, moveZ, 0.0f);
		moveVector = XMVector3Normalize(moveVector);

		XMFLOAT3 normalizeMove;
		XMStoreFloat3(&normalizeMove, moveVector);

		m_MoveDirection = XMFLOAT3(normalizeMove.x, 0.0f, normalizeMove.z);

		// ‰ñ“]XV
		float yaw = atan2f(normalizeMove.x, normalizeMove.z);
		//yaw += XM_PI;
		m_Rotation.y = yaw;

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
