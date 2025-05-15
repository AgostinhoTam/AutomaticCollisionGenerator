/*===================================================================================

プレイヤー制御(player.cpp)

====================================================================================*/
#include "Manager/animationRendererManager.h"
#include "Manager/sceneManager.h"
#include "Manager/shaderManager.h"
#include "Manager/inputManager.h"
#include "Manager/gameObjectManager.h"
#include "System/Enum/playerStateEnum.h"
#include "Scene/scene.h"
#include "System/Renderer/animationModel.h"
#include "StateMachine/PlayerState/playerStateIdle.h"
#include "StateMachine/PlayerState/playerStateWalk.h"
#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Camera/camera.h"
#include "GameObject/Character/Enemy/enemy.h"
#include "System/Collision/characterBoneCollision.h"

//	================定数=====================
constexpr float PLAYER_MAX_SPEED = 20.0f;			//	速度上限
constexpr float PLAYER_MAX_ACCL_SPEED = 50.0f;		//	加速度
constexpr float PLAYER_MAX_JUMP_SPEED = 100.0f;		//	ジャンプ速度
constexpr float PLAYER_SCALE = 0.01f;				//	スケール
//	=========================================

//	=========================プレイヤー初期化=========================
void Player::Init()
{
	//	モデルプールから読み込み
	m_AnimationModel = AnimationRendererManager::LoadAnimationModel(Model_Name::Player,this);

	//	GPUスキンニングシェーダー読み込み
	m_Shader = ShaderManager::LoadShader(Shader_Type::Unlit_Skinning_Texture);

	//	パラメーター
	m_MaxMovementSpeed = PLAYER_MAX_SPEED;
	m_MaxHorizontalAcclSpeed = PLAYER_MAX_ACCL_SPEED;
	m_Scale = { PLAYER_SCALE,PLAYER_SCALE,PLAYER_SCALE };

	//	オブジェクト名
	m_Name = "Player_" + m_Name;

	//	カメラと敵のリスト記録する
	Scene* scene = SceneManager::GetInstance()->GetCurrentScene();
	if (scene)
	{
		GameObjectManager* objectManager = scene->GetGameObjectManager();
		if (objectManager)
		{
			m_Camera = objectManager->GetGameObject<Camera>(GameObject_Type::Camera);
			objectManager->GetGameObjectsByLayer<Enemy>(m_EnemyList,GameObject_Type::Enemy);
		}
	}

	//	ステート登録
	m_PlayerState.reserve(static_cast<int>(Player_State::Max_State));
	m_PlayerState.try_emplace(Player_State::Idle, new PlayerStateIdle(this, m_Camera, m_AnimationModel));
	m_PlayerState.try_emplace(Player_State::Walk, new PlayerStateWalk(this, m_Camera, m_AnimationModel));
	m_CurrentState = m_PlayerState[Player_State::Idle];
	m_CurrentState->Init();

	//	とりあえず接地
	m_Position.y = 0.0f;
	m_IsGround = true;

	//	人形のボーンコリジョン設置
	CreateCharacterBoneCollision(Character_Bone_Type::Humanoid);

}

//	=========================プレイヤーUninit=========================
void Player::Uninit()
{
	m_AnimationModel->Uninit();
	Character::Uninit();
}
//	=========================プレイヤー更新=========================
//	DeltaTime	:	float	デルタタイム
void Player::Update(const float& DeltaTime)
{
	if (!m_CurrentState)return;
	if (!m_AnimationModel)return;
	if (!m_Camera)return;
	
	//	アニメーション更新
	m_AnimationModel->UpdateAnimationBlend();

	//	ステート更新
	m_CurrentState->Update();

	//	プレイヤー回転更新
	UpdatePlayerRotation();

	// キャラクター共通部分更新
	Character::Update(DeltaTime);

	//	移動後の当たり判定位置更新
	UpdateBoneCollision();

}

//	=========================プレイヤー描画=========================
void Player::Draw()
{
	if (!m_CurrentState)return;
	if (!m_AnimationModel)return;

	//	当たり判定描画
	for (auto& capsule : m_Collisions)
	{
		if (!capsule.second)continue;
		capsule.second->Draw();
	}

	//	モデル描画
	m_AnimationModel->Draw();

}
//	=========================プレイヤーステート遷移=========================
//	State	:	Player_State	遷移ステート指定
void Player::ChangeState(Player_State State)
{
	//	同じStateに遷移しないように
	if (m_CurrentState == m_PlayerState[State])return;
	
	m_CurrentState = m_PlayerState[State];

	if (!m_CurrentState) return;

	//	ステート初期化
	m_CurrentState->Init();
}
//	=========================プレイヤー回転更新=========================
void Player::UpdatePlayerRotation()
{
	//	当たり判定
	CollisionCheck();
	
	// 移動入力がある場合に回転を更新
	if (m_MoveDirection.x != 0.0f || m_MoveDirection.z != 0.0f)
	{
		const XMFLOAT3& cameraForward = m_Camera->GetForward();
		const XMFLOAT3& cameraRight = m_Camera->GetRight();

		float moveX = m_MoveDirection.x * cameraRight.x + m_MoveDirection.z * cameraForward.x;
		float moveZ = m_MoveDirection.x * cameraRight.z + m_MoveDirection.z * cameraForward.z;

		// 正規化して方向ベクトル
		XMVECTOR moveVector = XMVectorSet(moveX, 0.0f, moveZ, 0.0f);
		moveVector = XMVector3Normalize(moveVector);

		XMFLOAT3 normalizeMove;
		XMStoreFloat3(&normalizeMove, moveVector);

		m_MoveDirection = XMFLOAT3(normalizeMove.x, 0.0f, normalizeMove.z);

		// 回転更新
		float yaw = atan2f(normalizeMove.x, normalizeMove.z);
		//yaw += XM_PI;
		m_Rotation.y = yaw;

	}
}

//	=========================当たり判定=========================
void Player::CollisionCheck()
{
	//	一旦全部リセット
	for (const auto& playerPair : m_Collisions)
	{
		if (!playerPair.second)continue;
		playerPair.second->SetIsHit(false);
	}
}

//	=========================ターゲットロックオン=========================
Enemy* Player::LockTarget()
{
	for (Enemy* enemy : m_EnemyList)
	{
		m_Camera->CheckView(enemy->GetPosition());		
	}
	return nullptr;
}
