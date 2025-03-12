#include "GameObject\Character\Player\playerh.h"
#include "Manager\animationRendererManager.h"
#include "Manager\shaderManager.h"
#include "Manager\gameObjectManager.h"
#include "Manager\sceneManager.h"
#include "System\Renderer\animationModel.h"
#include "System\Collision\characterBoneCollision.h"
#include "Scene\scene.h"
#include "Manager\conditionManager.h"
#include "Behavior\conditionNode.h"
#include "Behavior\BlackBoard.h"
#include "Behavior\behaviorNode.h"
#include "Behavior\behaviorAttackNode.h"
#include "Behavior\behaviorIdleNode.h"
#include "Behavior\behaviorRunNode.h"
#include "enemy.h"

namespace EnemyTypeHuman
{
	constexpr float MAX_SPEED = 10.0f;
	constexpr float MAX_ACCL_SPEED = 20.0f;
	constexpr float MAX_JUMP_SPEED = 100.0f;
	constexpr float SCALE = 0.01f;
	constexpr float ATTACK_RANGE = 5.0f;
	constexpr float SIGHT_RANGE = 10.0f;
}
namespace EnemyTypeMonster
{
	constexpr float MAX_SPEED = 10.0f;
	constexpr float MAX_ACCL_SPEED = 20.0f;
	constexpr float MAX_JUMP_SPEED = 100.0f;
	constexpr float SCALE = 0.01f;
	constexpr float ATTACK_RANGE = 5.0f;
	constexpr float SIGHT_RANGE = 10.0f;
}

Enemy::Enemy(const ENEMY_TYPE& EnemyType)
{
	m_EnemyData.Type = EnemyType;

	GameObjectManager* gameObjectManager = SceneManager::GetInstance()->GetGameObjectManager();
	if (gameObjectManager)
	{
		m_CharacterID = static_cast<unsigned int>(gameObjectManager->GetGameObjectsList(GAMEOBJECT_TYPE::ENEMY).size());
	}
}

void Enemy::Init()
{

	SetEnemyType(m_EnemyData.Type);

	//	シェーダー
	m_Shader = ShaderManager::LoadShader(SHADER_NAME::UNLIT_SKINNING_TEXTURE);

	//	とりあえず接地
	m_Position.y = 0;
	m_IsGround = true;

	//	プレイヤー取得
	GameObjectManager* gameObjectManager = SceneManager::GetInstance()->GetGameObjectManager();
	if (gameObjectManager)
	{
		m_Player = gameObjectManager->GetGameObject<Player>(GAMEOBJECT_TYPE::PLAYER);
	}

}

void Enemy::Uninit()
{
	if (!m_AnimationModel)return;
	m_AnimationModel->Uninit();
	m_BehaviorRoot->Uninit();
	if (m_ConditionManager)
	{
		m_ConditionManager->Uninit();
		delete m_ConditionManager;
	}
	if (m_BlackBoard)
	{
		m_BlackBoard->Uninit();
		delete m_BlackBoard;
	}
}

void Enemy::Update(const float& DeltaTime)
{
	
	if (!m_AnimationModel)return;
	if (!m_BehaviorRoot)return;

	BlackBoardUpdate();
	CollisionCheck();
	m_BehaviorRoot->Update(DeltaTime);
	Character::Update(DeltaTime);

	m_AnimationModel->Update();
	UpdateBoneCollision();
}

void Enemy::Draw()
{
	if (!m_AnimationModel)return;
	m_AnimationModel->Draw();

	for (auto& pair : m_Collisions)
	{
		if (!pair.second)continue;
		pair.second->Draw();
	}

}

void Enemy::CollisionCheck()
{

	//	キャッシュ
	std::unordered_map<std::string, Collision*>& playerCollisonList = m_Player->GetCollisionList();

	for (const auto& enemyPair : m_Collisions)
	{
		bool isHit = false;

		if (!enemyPair.second)
		{
			continue;
		}

		for (auto& playerPair : playerCollisonList)
		{
			if (!playerPair.second)continue;

			if (enemyPair.second->IsCollisionOverlapping(playerPair.second))
			{
				isHit = true;
				playerPair.second->SetIsHit(isHit);
				break;
			}

		}
		enemyPair.second->SetIsHit(isHit);

	}

}

void Enemy::BlackBoardUpdate()
{
	if (!m_Player)return;
	if (!m_BlackBoard)return;

	const XMFLOAT3& playerPos = m_Player->GetPosition();
	const XMFLOAT3& enemyPos = m_Position;

	XMVECTOR playerPosVec = XMLoadFloat3(&playerPos);
	XMVECTOR enemyPosVec = XMLoadFloat3(&enemyPos);
	XMVECTOR lengthVec = XMVector3Length(playerPosVec - enemyPosVec);
	float length = XMVectorGetX(lengthVec);

	if (length < m_EnemyData.SightRange)
	{
		m_BlackBoard->SetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_SIGHT, true);
	}
	else
	{
		m_BlackBoard->SetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_SIGHT, false);
	}

	if (length < m_EnemyData.AttackRange)
	{
		m_BlackBoard->SetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_ATTACK_RANGE, true);
	}
	else
	{
		m_BlackBoard->SetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_ATTACK_RANGE, false);
	}

	
}

void Enemy::SetEnemyType(const ENEMY_TYPE& Type)
{

	m_BlackBoard = new BlackBoard();
	//	BB初期化
	m_BlackBoard->SetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_SIGHT, false);
	m_BlackBoard->SetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_DEAD, false);
	m_BlackBoard->SetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_ATTACK_RANGE, false);

	if (Type == ENEMY_TYPE::ENEMY)
	{
		m_Name = "EnemyHuman";
		m_AnimationModel = AnimationRendererManager::LoadAnimationModel(MODEL_NAME::ENEMY, this);
		m_AnimationModel->SetCurrentAnimation("Idle");
		m_AnimationModel->SetNextAnimation("Idle");
		m_MaxMovementSpeed = EnemyTypeHuman::MAX_SPEED;
		m_MaxHorizontalAcclSpeed = EnemyTypeHuman::MAX_ACCL_SPEED;
		m_Scale = { EnemyTypeHuman::SCALE,EnemyTypeHuman::SCALE,EnemyTypeHuman::SCALE };
		m_EnemyData.SightRange = EnemyTypeHuman::SIGHT_RANGE;
		m_EnemyData.AttackRange = EnemyTypeHuman::ATTACK_RANGE;
		m_BehaviorRoot = new BehaviorSequence(this);
		m_BehaviorRoot->AddChildNode(new BehaviorIdleNode(this, "Idle"));
		m_BehaviorRoot->AddChildNode(new BehaviorRunNode(this, "Run"));
		BehaviorNode* attackNode = new BehaviorSelector(this);
		attackNode->AddChildNode(new BehaviorAttackNode(this, "Attack",3.0f,1.0f));
		m_BehaviorRoot->AddChildNode(attackNode);
		CreateCharacterBoneCollision(CHARACTER_BONE_TYPE::HUMANOID);

	}
	else if (Type == ENEMY_TYPE::MONSTER)
	{
		m_Name = "EnemyMonster";
		m_AnimationModel = AnimationRendererManager::LoadAnimationModel(MODEL_NAME::MONSTER, this);
		m_AnimationModel->SetCurrentAnimation("Idle");
		m_AnimationModel->SetNextAnimation("Idle");
		m_MaxMovementSpeed = EnemyTypeMonster::MAX_SPEED;
		m_MaxHorizontalAcclSpeed = EnemyTypeMonster::MAX_ACCL_SPEED;
		m_Scale = { EnemyTypeMonster::SCALE,EnemyTypeMonster::SCALE,EnemyTypeMonster::SCALE };
		m_EnemyData.SightRange = EnemyTypeMonster::SIGHT_RANGE;
		m_EnemyData.AttackRange = EnemyTypeMonster::ATTACK_RANGE;
		m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_BehaviorRoot = new BehaviorSequence(this);
		m_BehaviorRoot->AddChildNode(new BehaviorIdleNode(this, "Idle"));
		m_BehaviorRoot->AddChildNode(new BehaviorRunNode(this, "Run"));
		BehaviorNode* attackNode = new BehaviorSelector(this);
		attackNode->AddChildNode(new BehaviorAttackNode(this, "Attack",3.0f,1.0f));
		m_BehaviorRoot->AddChildNode(attackNode);
		CreateCharacterBoneCollision(CHARACTER_BONE_TYPE::MONSTER);
	}
	else
	{
		m_Name = "EnemyHuman";
		m_AnimationModel = AnimationRendererManager::LoadAnimationModel(MODEL_NAME::ENEMY, this);
		m_AnimationModel->SetCurrentAnimation("Idle");
		m_AnimationModel->SetNextAnimation("Idle");
		m_MaxMovementSpeed = EnemyTypeHuman::MAX_SPEED;
		m_MaxHorizontalAcclSpeed = EnemyTypeHuman::MAX_ACCL_SPEED;
		m_Scale = { EnemyTypeHuman::SCALE,EnemyTypeHuman::SCALE,EnemyTypeHuman::SCALE };
		m_EnemyData.SightRange = EnemyTypeHuman::SIGHT_RANGE;
		m_EnemyData.AttackRange = EnemyTypeHuman::ATTACK_RANGE;
		m_BehaviorRoot = new BehaviorSequence(this);
		m_BehaviorRoot->AddChildNode(new BehaviorIdleNode(this, "Idle"));
		m_BehaviorRoot->AddChildNode(new BehaviorRunNode(this, "Run"));
		BehaviorNode* attackNode = new BehaviorSelector(this);
		attackNode->AddChildNode(new BehaviorAttackNode(this, "Attack",3.0f,1.0f));
		m_BehaviorRoot->AddChildNode(attackNode);
		CreateCharacterBoneCollision(CHARACTER_BONE_TYPE::HUMANOID);

	}
	m_Name += std::to_string(m_CharacterID);

}

void Enemy::FaceTargetDirection()
{
	const XMFLOAT3& playerPos = m_Player->GetPosition();
	const XMFLOAT3& enemyPos = m_Position;

	XMVECTOR playerPosVec = XMLoadFloat3(&playerPos);
	XMVECTOR enemyPosVec = XMLoadFloat3(&enemyPos);

	XMVECTOR directionVec = XMVectorSubtract(playerPosVec, enemyPosVec);

	float yaw = atan2f(XMVectorGetX(directionVec), XMVectorGetZ(directionVec));

	m_Rotation.y = yaw;
}

void Enemy::MoveToTargetDirection()
{
	const XMFLOAT3& playerPos = m_Player->GetPosition();
	const XMFLOAT3& enemyPos = m_Position;

	XMVECTOR playerPosVec = XMLoadFloat3(&playerPos);
	XMVECTOR enemyPosVec = XMLoadFloat3(&enemyPos);

	XMVECTOR directionVec = XMVectorSubtract(playerPosVec, enemyPosVec);

	float yaw = atan2f(XMVectorGetX(directionVec), XMVectorGetZ(directionVec));

	m_Rotation.y = yaw;

	m_MoveDirection = XMFLOAT3(XMVectorGetX(directionVec), 0.0f, XMVectorGetZ(directionVec));
}

float Enemy::GetTargetDistance()
{
	const XMFLOAT3& playerPos = m_Player->GetPosition();
	const XMFLOAT3& enemyPos = m_Position;

	XMVECTOR playerPosVec = XMLoadFloat3(&playerPos);
	XMVECTOR enemyPosVec = XMLoadFloat3(&enemyPos);

	XMVECTOR directionVec = XMVectorSubtract(playerPosVec, enemyPosVec);

	XMVECTOR lengthVec = XMVector3Length(directionVec);

	return XMVectorGetX(lengthVec);
}

