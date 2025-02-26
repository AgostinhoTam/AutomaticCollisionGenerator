#include"Manager\sceneManager.h"
#include "Manager\gameObjectManager.h"
#include "Scene\scene.h"
#include "Main\main.h"
#include "GameObject\Character\Player\playerh.h"
#include "GameObject\Character\Enemy\enemy.h"
#include "System\Renderer\animationModelInstance.h"
#include "System\Renderer\animator.h"
#include "behaviorTree.h"

constexpr float SENSE_DISTANCE = 10.0f;
constexpr float FOLLOW_DISTANCE = 25.0f;
constexpr float ATTACK_DISTANCE = 5.0f;
constexpr float ATTACK_COOLDOWN = 3.0f;

//	===========================制御ノード===============================
BEHAVIOR_RESULT BehaviorSequence::Update(const float DeltaTime)
{
	while (m_Index < m_Child.size())
	{
		BEHAVIOR_RESULT result = m_Child[m_Index]->Update(DeltaTime);

		if (result == BEHAVIOR_RESULT::SUCCESS)
		{
			++m_Index;
			if (m_Index >= m_Child.size())
			{
				m_Index = 0;
				return BEHAVIOR_RESULT::SUCCESS;
			}
		}
		else if (result == BEHAVIOR_RESULT::CONTINUE)
		{
			return BEHAVIOR_RESULT::CONTINUE;
		}
		else
		{
			m_Index = 0;
			return BEHAVIOR_RESULT::FAILURE;
		}
	}
	
	return BEHAVIOR_RESULT::FAILURE;
}

BEHAVIOR_RESULT BehaviorSelector::Update(const float DeltaTime)
{
	while (m_Index < m_Child.size())
	{
		BEHAVIOR_RESULT result = m_Child[m_Index]->Update(DeltaTime);
		if (result == BEHAVIOR_RESULT::SUCCESS)
		{
			m_Index = 0;
			return BEHAVIOR_RESULT::SUCCESS;
			break;
		}
		else if(result == BEHAVIOR_RESULT::CONTINUE)
		{
			return BEHAVIOR_RESULT::CONTINUE;
		}
		else
		{
			++m_Index;
		}

	}
	m_Index = 0;
	return BEHAVIOR_RESULT::FAILURE;
}
//====================================================================


BehaviorIdle::BehaviorIdle(Enemy* Enemy, const std::string& Type):BehaviorNode(Enemy)
{
	m_AnimationName = Type;
}

//=========================ステート==========================
void BehaviorIdle::Init()
{
	if (!m_Animator)return;

	//	再設定防止
	if (m_Animator->GetCurrentAnimationName() != m_AnimationName)
	{
		m_Animator->SetNextAnimation(m_AnimationName);
	}

}

BEHAVIOR_RESULT BehaviorIdle::Update(const float DeltaTime)
{
	if (!m_AnimationModel)return BEHAVIOR_RESULT::FAILURE;
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR direction = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(direction));
	if (length < SENSE_DISTANCE)
	{
		// 次の状態へ
		return BEHAVIOR_RESULT::SUCCESS;
	}
	if (m_Animator->GetNextAnimationName() != m_AnimationName)
	{
		Init();
	}
	m_AnimationModel->Update();
	return BEHAVIOR_RESULT::CONTINUE;
}

BehaviorNode::BehaviorNode(Enemy* Enemy)
{
	m_Enemy = Enemy;
	m_AnimationModel = m_Enemy->GetAnimationModel();
	m_Animator = m_Enemy->GetAnimator();
	Scene* scene = SceneManager::GetCurrentScene();
	if (!scene)return;
	GameObjectManager* gameObjectManager = scene->GetGameObjectManager();
	if (!gameObjectManager)return;
	Player* player = gameObjectManager->GetGameObject<Player>(GAMEOBJECT_TYPE::PLAYER);
	if (player)m_Player = player;
}

void BehaviorNode::AddChildNode(BehaviorNode* Node)
{	
	//	ツリー追加の時参照追加
	Node->m_Parent = this;
	m_Child.emplace_back(Node);
}

BehaviorMove::BehaviorMove(Enemy* Enemy, const std::string& Type) :BehaviorNode(Enemy)
{
	m_AnimationName = Type;
}

void BehaviorMove::Init()
{
	if (!m_Animator)return;
	if (m_Animator->GetCurrentAnimationName() != m_AnimationName)
	{
		m_Animator->SetNextAnimation(m_AnimationName);
	}
}

BEHAVIOR_RESULT BehaviorMove::Update(const float DeltaTime)
{
	if (!m_Animator)return BEHAVIOR_RESULT::FAILURE;
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(vector));
	//	一定距離で停止
	if (length < ATTACK_DISTANCE)
	{
		m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
		return BEHAVIOR_RESULT::SUCCESS;
	}
	else if (length > FOLLOW_DISTANCE)
	{
		m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
		return BEHAVIOR_RESULT::FAILURE;
	}

	if (m_Animator->GetNextAnimationName() != m_AnimationName)
	{
		Init();
	}
	XMVECTOR normalizeDirection = XMVector3Normalize(vector);
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, normalizeDirection);
	float yaw = atan2f(direction.x, direction.z);
	m_Enemy->SetRotationY(yaw); // 向き更新
	m_Enemy->SetMoveDirection(direction);
	m_AnimationModel->Update();
	return BEHAVIOR_RESULT::CONTINUE;
}

void BehaviorAttack::Init()
{
	if (!m_Animator)return;
	//	現在と同じならスキップ
	if (m_Animator->GetCurrentAnimationName() != m_AnimationName)
	{
		m_Animator->SetNextAnimation(m_AnimationName);
	}

	m_IsAttackStart = true;
	m_BehaviorCoolDown->StartCoolDown();
	
}

BehaviorAttack::BehaviorAttack(Enemy* Enemy, const std::string& Type,const float AttackDistance):BehaviorNode(Enemy),m_AttackDistance(AttackDistance)
{
	m_AnimationName = Type;
	m_BehaviorCoolDown = new BehaviorCoolDown(m_Enemy);	// CoolDown実装

}

BehaviorAttack::~BehaviorAttack()
{
	delete m_BehaviorCoolDown;
}

BEHAVIOR_RESULT BehaviorAttack::Update(const float DeltaTime)
{
	if (!m_Animator)return BEHAVIOR_RESULT::FAILURE;
	if(!m_AnimationModel)return BEHAVIOR_RESULT::FAILURE;
	if (!m_BehaviorCoolDown)return BEHAVIOR_RESULT::FAILURE;
	
	//	攻撃初期化
	if (!m_IsAttackStart)
	{
		Init();
	}

	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(vector));

	//攻撃範囲内じゃない時
	if (length >= m_AttackDistance)
	{
		XMVECTOR normalizeDirection = XMVector3Normalize(vector);
		XMFLOAT3 direction;
		XMStoreFloat3(&direction, normalizeDirection);
		m_Enemy->SetMoveDirection(direction);
		float yaw = atan2f(direction.x, direction.z);
		m_Enemy->SetRotationY(yaw); 
	}
	else
	{	
		//範囲内だったら止まる
		m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
	}

	//	遷移中、またはアニメーションの再生中だったら
	if (m_Animator->GetIsTransitioning() || m_Animator->GetCurrentAnimationFrame() <= m_Animator->GetAnimationDuration(m_AnimationName))
	{
		m_AnimationModel->Update();
		return BEHAVIOR_RESULT::CONTINUE;
	}



	//======アニメーション再生終わった後の処理=======

	//	クールダウン中だったら次のアタックへ
	if (m_BehaviorCoolDown->Update(DeltaTime) == BEHAVIOR_RESULT::FAILURE)
	{
		return BEHAVIOR_RESULT::FAILURE;
	}
	//	クールダウン終わったら
	else
	{
		//	追いかける距離外だったら
		if (length > SENSE_DISTANCE)
		{
			return BEHAVIOR_RESULT::FAILURE;
		}
		m_IsAttackStart = false;
		m_BehaviorCoolDown->ResetCoolDown();
		return BEHAVIOR_RESULT::SUCCESS;
	}


}

BEHAVIOR_RESULT BehaviorCoolDown::Update(const float DeltaTime)
{
	if (!m_IsCoolDownActive)return BEHAVIOR_RESULT::SUCCESS;

	m_ElapsedTime += DeltaTime;
	if (m_ElapsedTime >= ATTACK_COOLDOWN)
	{
		m_IsCoolDownActive = false;
		return BEHAVIOR_RESULT::SUCCESS;	// クールダウン終了
	}
	return BEHAVIOR_RESULT::FAILURE;		//　クールダウン中
}

void BehaviorCoolDown::StartCoolDown()
{
	m_ElapsedTime = 0;
	m_IsCoolDownActive = true;
}
void BehaviorCoolDown::ResetCoolDown()
{
	m_IsCoolDownActive = false;
	
}
//===================================================================

void BehaviorStandByAttack::Init()
{
	if (!m_Animator)return;
	//	現在と同じならスキップ
	if (m_Animator->GetCurrentAnimationName() != m_AnimationName)
	{
		m_Animator->SetNextAnimation(m_AnimationName);
	}
}

BehaviorStandByAttack::BehaviorStandByAttack(Enemy* Enemy, const std::string& Type):BehaviorNode(Enemy)
{
	m_AnimationName = Type;
}

BEHAVIOR_RESULT BehaviorStandByAttack::Update(const float DeltaTime)
{
	if (!m_Animator)return BEHAVIOR_RESULT::FAILURE;

	//	初期化
	if (m_Animator->GetCurrentAnimationName() != m_AnimationName)
	{
		Init();
	}

	//	向き更新
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(vector));
	XMVECTOR normalizeDirection = XMVector3Normalize(vector);
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, normalizeDirection);
	float yaw = atan2f(direction.x, direction.z);
	m_Enemy->SetRotationY(yaw); //	向きは1回だけ更新する

	m_AnimationModel->Update();
	
	//　一番手前の状態に返す
	return BEHAVIOR_RESULT::SUCCESS;
}



