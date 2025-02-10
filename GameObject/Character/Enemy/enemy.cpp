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
namespace EnemyTypeA
{
	constexpr float MAX_ENEMY_SPEED = 10.0f;
	constexpr float ENEMY_MAX_ACCL_SPEED = 20.0f;
	constexpr float ENEMY_MAX_JUMP_SPEED = 100.0f;
	constexpr float ENEMY_SCALE = 0.01f;
}

Enemy::Enemy(ENEMY_TYPE EnemyType)
{

	switch (EnemyType)
	{
	case ENEMY_TYPE::ENEMY:
		m_Name = "Enemy";
		m_AnimationModel = AnimationRendererManager::LoadAnimationModel(MODEL_NAME::ENEMY);
		m_MaxMovementSpeed = EnemyTypeA::MAX_ENEMY_SPEED;
		m_MaxHorizontalAcclSpeed = EnemyTypeA::ENEMY_MAX_ACCL_SPEED;
		m_Scale = { EnemyTypeA::ENEMY_SCALE,EnemyTypeA::ENEMY_SCALE,EnemyTypeA::ENEMY_SCALE };

		m_BehaviorRoot = new BehaviorSequence(this);
		m_BehaviorRoot->AddChildNode(new BehaviorIdle(this));
		m_BehaviorRoot->AddChildNode(new BehaviorMove(this));
		BehaviorNode* attackNode = new BehaviorSelector(this);
		attackNode->AddChildNode(new BehaviorAttack(this));
		m_BehaviorRoot->AddChildNode(attackNode);

		CreateCharacterBoneCollision(CHARACTER_BONE_TYPE::HUMANOID);

		break;
	}
}
void Enemy::Init()
{

	m_Shader = ShaderManager::LoadShader(SHADER_NAME::UNLIT_TEXTURE);

	//	‚Æ‚è‚ ‚¦‚¸Ú’n
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
}

void Enemy::Update(const float& DeltaTime)
{
	if (!m_AnimationModel)return;
	CollisionCheck();
	m_BehaviorRoot->Update();
	Character::Update(DeltaTime);

	UpdateBoneCollision();
}

void Enemy::Draw()
{
	if (!m_AnimationModel)return;
	m_AnimationModel->Draw(this);

	if (m_Collision)m_Collision->Draw();
	for (auto& capsule : m_Collisions)
	{
		capsule.second->Draw();
	}

}

void Enemy::CollisionCheck()
{

	const std::unordered_map<std::string,Collision*>& playerCollisonList =  m_Player->GetCollisionList();

	for (const auto& enemyPair : m_Collisions)
	{
		bool isHit = false;


		for (const auto& playerPair : playerCollisonList)
		{
			Collision* playerCollision = playerPair.second;

			if (enemyPair.second->IsCollisionOverlapping(playerCollision))
			{
				isHit = true;
				playerCollision->SetIsHit(isHit);
				break;
			}
			
		}
		enemyPair.second->SetIsHit(isHit);
		
	}

}
