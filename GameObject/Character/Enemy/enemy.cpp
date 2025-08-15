/*===================================================================================

エネミー制御(enemy.cpp)

====================================================================================*/
#include "GameObject/Character/Player/player.h"
#include "Manager/animationRendererManager.h"
#include "Manager/shaderManager.h"
#include "Manager/gameObjectManager.h"
#include "Manager/sceneManager.h"
#include "System/Renderer/animationModel.h"
#include "System/Collision/characterBoneCollision.h"
#include "Behavior/behaviorCoolDown.h"
#include "Behavior/behaviorNode.h"
#include "Behavior/behaviorIdle.h"
#include "Behavior/behaviorAttack.h"
#include "Behavior/behaviorStandByAttack.h"
#include "Behavior/behaviorMove.h"
#include "enemy.h"

//=============敵種類=============
namespace EnemyTypeHuman
{
    constexpr float MAX_ENEMY_SPEED = 10.0f;
    constexpr float MAX_ACCL_SPEED = 20.0f;
    constexpr float MAX_JUMP_SPEED = 100.0f;
    constexpr float SCALE = 0.01f;
    constexpr float SENSE_DISTANCE = 10.0f;
    constexpr float ATTACK_DISTANCE = 2.0f;
    constexpr float ATTACK_COOLDOWN = 2.0f;
    constexpr float CHASE_DISTANCE = 25.0f;
}

namespace EnemyTypeMonster
{
    constexpr float MAX_SPEED = 10.0f;
    constexpr float MAX_ACCL_SPEED = 20.0f;
    constexpr float MAX_JUMP_SPEED = 100.0f;
    constexpr float SCALE = 0.01f;
    constexpr float SENSE_DISTANCE = 10.0f;
    constexpr float ATTACK_DISTANCE = 2.0f;
    constexpr float ATTACK_COOLDOWN = 2.0f;
    constexpr float CHASE_DISTANCE = 25.0f;
    
}

//====================================


//	=========================敵初期化=========================
void Enemy::Init()
{
    GameObjectManager* gameObjectManager = SceneManager::GetInstance()->GetGameObjectManager();
    if (!gameObjectManager)return;

    //	敵のタイプで生成データ選択
    if (m_EnemyType == Enemy_Type::Humanroid)
    {
        m_Name = "EnemyHuman_" + m_Name;
        m_AnimationModel = AnimationRendererManager::LoadAnimationModel(Model_Name::Enemy_Humanoid, this);
        m_MaxMovementSpeed = EnemyTypeHuman::MAX_ENEMY_SPEED;
        m_MaxHorizontalAcclSpeed = EnemyTypeHuman::MAX_ACCL_SPEED;
        m_SenseDistance = EnemyTypeHuman::SENSE_DISTANCE;
        m_AttackDistance = EnemyTypeHuman::ATTACK_DISTANCE;
        m_AttackCoolDownTime = EnemyTypeHuman::ATTACK_COOLDOWN;
        m_ChaseDistance = EnemyTypeHuman::CHASE_DISTANCE;
        m_Scale = {EnemyTypeHuman::SCALE, EnemyTypeHuman::SCALE, EnemyTypeHuman::SCALE};
        m_BehaviorRoot = new BehaviorSequence(this);
        m_BehaviorRoot->AddChildNode(new BehaviorIdle(this, "Enemy_Idle"));
        m_BehaviorRoot->AddChildNode(new BehaviorMove(this, "Enemy_Run"));
        BehaviorNode* attackNode = new BehaviorSelector(this);
        attackNode->AddChildNode(new BehaviorAttack(this, "Enemy_Kick", 1.5f));
        attackNode->AddChildNode(new BehaviorStandByAttack(this, "Enemy_Idle"));
        m_BehaviorRoot->AddChildNode(attackNode);
        CreateCharacterBoneCollision(Character_Bone_Type::Humanoid);
    }
    else if (m_EnemyType == Enemy_Type::Monster)
    {
        m_Name = "EnemyMonster_" + m_Name;
        m_AnimationModel = AnimationRendererManager::LoadAnimationModel(Model_Name::Enemy_Monster, this);
        m_MaxMovementSpeed = EnemyTypeMonster::MAX_SPEED;
        m_MaxHorizontalAcclSpeed = EnemyTypeMonster::MAX_ACCL_SPEED;
        m_SenseDistance = EnemyTypeMonster::SENSE_DISTANCE;
        m_AttackDistance = EnemyTypeMonster::ATTACK_DISTANCE;
        m_AttackCoolDownTime = EnemyTypeMonster::ATTACK_COOLDOWN;
        m_ChaseDistance = EnemyTypeMonster::CHASE_DISTANCE;
        m_Scale = {EnemyTypeMonster::SCALE, EnemyTypeMonster::SCALE, EnemyTypeMonster::SCALE};
        m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
        m_BehaviorRoot = new BehaviorSequence(this);
        m_BehaviorRoot->AddChildNode(new BehaviorIdle(this, "Monster_Idle"));
        m_BehaviorRoot->AddChildNode(new BehaviorMove(this, "Monster_Run"));
        BehaviorNode* attackNode = new BehaviorSelector(this);
        attackNode->AddChildNode(new BehaviorAttack(this, "Monster_Attack", 1.0f));
        attackNode->AddChildNode(new BehaviorStandByAttack(this, "Monster_Idle"));
        m_BehaviorRoot->AddChildNode(attackNode);
        CreateCharacterBoneCollision(Character_Bone_Type::Monster);
    }
    else // デフォルト人間タイプ
    {
        m_Name = "EnemyHuman_" + m_Name;
        m_AnimationModel = AnimationRendererManager::LoadAnimationModel(Model_Name::Enemy_Humanoid, this);
        m_MaxMovementSpeed = EnemyTypeHuman::MAX_ENEMY_SPEED;
        m_MaxHorizontalAcclSpeed = EnemyTypeHuman::MAX_ACCL_SPEED;
        m_SenseDistance = EnemyTypeHuman::SENSE_DISTANCE;
        m_AttackDistance = EnemyTypeHuman::ATTACK_DISTANCE;
        m_AttackCoolDownTime = EnemyTypeHuman::ATTACK_COOLDOWN;
        m_ChaseDistance = EnemyTypeHuman::CHASE_DISTANCE;
        m_Scale = {EnemyTypeHuman::SCALE, EnemyTypeHuman::SCALE, EnemyTypeHuman::SCALE};
        m_BehaviorRoot = new BehaviorSequence(this);
        m_BehaviorRoot->AddChildNode(new BehaviorIdle(this, "Enemy_Idle"));
        m_BehaviorRoot->AddChildNode(new BehaviorMove(this, "Enemy_Run"));
        BehaviorNode* attackNode = new BehaviorSelector(this);
        attackNode->AddChildNode(new BehaviorAttack(this, "Enemy_Kick", 1.0f));
        attackNode->AddChildNode(new BehaviorStandByAttack(this, "Enemy_Idle"));
        m_BehaviorRoot->AddChildNode(attackNode);
        CreateCharacterBoneCollision(Character_Bone_Type::Humanoid);
     
    }

    //	GPUスキンニングシェーダー読み込み
    m_Shader = ShaderManager::LoadShader(Shader_Type::Unlit_Skinning_Texture);

    //	とりあえず接地
    m_Position.y = 0;
    m_IsGround = true;

    //	プレイヤー設置
    if (gameObjectManager)
    {
        m_Player = gameObjectManager->GetGameObject<Player>(GameObject_Type::Player);
    }
}

//	=========================敵Uninit=========================
void Enemy::Uninit()
{
    if (!m_AnimationModel)return;
    m_AnimationModel->Uninit();
}

//	=========================敵更新=========================
//	DeltaTIme	:	float	デルタタイム
void Enemy::Update(const float& DeltaTime)
{
    if (!m_AnimationModel)return;

    //	当たり判定
    CollisionCheck();

    //	ビヘイビアー更新
    m_BehaviorRoot->Update(DeltaTime);

    //	キャラクター共通部分更新
    Character::Update(DeltaTime);

    //	移動後当たり判定更新
    UpdateBoneCollision();
}

//	=========================敵描画=========================
void Enemy::Draw()
{
    if (!m_AnimationModel)return;

    //	モデル描画
    m_AnimationModel->Draw();

    //	コリジョンの描画
    for (auto& pair : m_Collisions)
    {
        if (!pair.second)continue;
        pair.second->Draw();
    }
}

//	========================当たり判定探知======================
void Enemy::CollisionCheck()
{
    //	プレイヤーの当たり判定をキャッシュ
    std::unordered_map<std::string, Collision*>& playerCollisonList = m_Player->GetCollisionList();

    //	敵クラス自身が保持している当たり判定を走査
    for (const auto& enemyPair : m_Collisions)
    {
        bool isHit = false;

        if (!enemyPair.second)
        {
            continue;
        }

        //	プレイヤーとの当たり判定走査
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
