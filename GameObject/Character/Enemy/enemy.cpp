#include "GameObject\Character\Player\playerh.h"
#include "Manager\animationRendererManager.h"
#include "Manager\shaderManager.h"
#include "Manager\gameObjectManager.h"
#include "Manager\sceneManager.h"
#include "System\Renderer\animationModel.h"
#include "System\Collision\characterBoneCollision.h"
#include "Scene\scene.h"
#include "Behavior\behaviorTree.h"
#include "enemy.h"
namespace EnemyTypeHuman
{
	constexpr float MAX_ENEMY_SPEED = 10.0f;
	constexpr float ENEMY_MAX_ACCL_SPEED = 20.0f;
	constexpr float ENEMY_MAX_JUMP_SPEED = 100.0f;
	constexpr float ENEMY_SCALE = 0.01f;
}
namespace EnemyTypeMonster
{
	constexpr float MAX_SPEED = 10.0f;
	constexpr float MAX_ACCL_SPEED = 20.0f;
	constexpr float MAX_JUMP_SPEED = 100.0f;
	constexpr float SCALE = 0.01f;
}

void Enemy::Init()
{
	if (m_EnemyType == ENEMY_TYPE::ENEMY)
	{
		m_Name = "EnemyHuman";
		m_AnimationModel = AnimationRendererManager::LoadAnimationModel(MODEL_NAME::ENEMY, this);
		m_MaxMovementSpeed = EnemyTypeHuman::MAX_ENEMY_SPEED;
		m_MaxHorizontalAcclSpeed = EnemyTypeHuman::ENEMY_MAX_ACCL_SPEED;
		m_Scale = { EnemyTypeHuman::ENEMY_SCALE,EnemyTypeHuman::ENEMY_SCALE,EnemyTypeHuman::ENEMY_SCALE };
		m_BehaviorRoot = new BehaviorSequence(this);
		m_BehaviorRoot->AddChildNode(new BehaviorIdle(this, "Enemy_Idle"));
		m_BehaviorRoot->AddChildNode(new BehaviorMove(this, "Enemy_Run"));
		BehaviorNode* attackNode = new BehaviorSelector(this);
		attackNode->AddChildNode(new BehaviorAttack(this, "Enemy_Kick", 1.5f));
		attackNode->AddChildNode(new BehaviorStandByAttack(this, "Enemy_Idle"));
		m_BehaviorRoot->AddChildNode(attackNode);
		CreateCharacterBoneCollision(CHARACTER_BONE_TYPE::HUMANOID);
	}
	else if (m_EnemyType == ENEMY_TYPE::MONSTER)
	{
		m_Name = "EnemyMonster";
		m_AnimationModel = AnimationRendererManager::LoadAnimationModel(MODEL_NAME::MONSTER, this);
		m_MaxMovementSpeed = EnemyTypeMonster::MAX_SPEED;
		m_MaxHorizontalAcclSpeed = EnemyTypeMonster::MAX_ACCL_SPEED;
		m_Scale = { EnemyTypeMonster::SCALE,EnemyTypeMonster::SCALE,EnemyTypeMonster::SCALE };
		m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_BehaviorRoot = new BehaviorSequence(this);
		m_BehaviorRoot->AddChildNode(new BehaviorIdle(this, "Monster_Idle"));
		m_BehaviorRoot->AddChildNode(new BehaviorMove(this, "Monster_Run"));
		BehaviorNode* attackNode = new BehaviorSelector(this);
		attackNode->AddChildNode(new BehaviorAttack(this, "Monster_Attack", 1.0f));
		attackNode->AddChildNode(new BehaviorStandByAttack(this, "Monster_Idle"));
		m_BehaviorRoot->AddChildNode(attackNode);
		CreateCharacterBoneCollision(CHARACTER_BONE_TYPE::MONSTER);
	}
	else
	{
		m_Name = "EnemyHuman";
		m_AnimationModel = AnimationRendererManager::LoadAnimationModel(MODEL_NAME::ENEMY, this);
		m_MaxMovementSpeed = EnemyTypeHuman::MAX_ENEMY_SPEED;
		m_MaxHorizontalAcclSpeed = EnemyTypeHuman::ENEMY_MAX_ACCL_SPEED;
		m_Scale = { EnemyTypeHuman::ENEMY_SCALE,EnemyTypeHuman::ENEMY_SCALE,EnemyTypeHuman::ENEMY_SCALE };
		m_BehaviorRoot = new BehaviorSequence(this);
		m_BehaviorRoot->AddChildNode(new BehaviorIdle(this, "Enemy_Idle"));
		m_BehaviorRoot->AddChildNode(new BehaviorMove(this, "Enemy_Run"));
		BehaviorNode* attackNode = new BehaviorSelector(this);
		attackNode->AddChildNode(new BehaviorAttack(this, "Enemy_Kick", 1.0f));
		attackNode->AddChildNode(new BehaviorStandByAttack(this, "Enemy_Idle"));
		m_BehaviorRoot->AddChildNode(attackNode);
		CreateCharacterBoneCollision(CHARACTER_BONE_TYPE::HUMANOID);
	}

	m_Shader = ShaderManager::LoadShader(SHADER_NAME::UNLIT_SKINNING_TEXTURE);

	//	とりあえず接地
	m_Position.y = 0;
	m_IsGround = true;

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
}

void Enemy::Update(const float& DeltaTime)
{
	if (!m_AnimationModel)return;
	CollisionCheck();
	m_BehaviorRoot->Update(DeltaTime);
	Character::Update(DeltaTime);

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
