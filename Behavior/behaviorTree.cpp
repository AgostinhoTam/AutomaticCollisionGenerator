/*===================================================================================

	ビヘイビアーツリー(BehaviorTree.cpp)

====================================================================================*/
#include"Manager/sceneManager.h"
#include "Manager/gameObjectManager.h"
#include "Scene/scene.h"
#include "main.h"
#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Character/Enemy/enemy.h"
#include "behaviorTree.h"

constexpr float SENSE_DISTANCE = 10.0f;
constexpr float FOLLOW_DISTANCE = 25.0f;
constexpr float ATTACK_DISTANCE = 5.0f;
constexpr float ATTACK_COOLDOWN = 3.0f;

//	===========================Sequenceノード更新==============================
//	DeltaTime	:	float	デルタタイム
Behavior_Result BehaviorSequence::Update(const float DeltaTime)
{
	while (m_Index < m_Child.size())
	{
		Behavior_Result result = m_Child[m_Index]->Update(DeltaTime);

		if (result == Behavior_Result::Success)	//	成功したら次のノードへ
		{
			++m_Index;
			if (m_Index >= m_Child.size())
			{
				m_Index = 0;
				return Behavior_Result::Success;
			}
		}
		else if (result == Behavior_Result::Continue)	//継続
		{
			return Behavior_Result::Continue;
		}
		else // 失敗したらリセット
		{
			m_Index = 0;
			return Behavior_Result::Failure;
		}
	}
	
	return Behavior_Result::Failure;
}

//	===========================Selectorノード更新==============================
//	DeltaTime	:	float	デルタタイム
Behavior_Result BehaviorSelector::Update(const float DeltaTime)
{
	//	子ノード全部走査
	while (m_Index < m_Child.size())
	{
		//	ノード更新
		Behavior_Result result = m_Child[m_Index]->Update(DeltaTime);
		
		if (result == Behavior_Result::Success)	//	成功だったらリセット
		{
			m_Index = 0;
			return Behavior_Result::Success;
			break;
		}
		else if(result == Behavior_Result::Continue)	//	継続
		{
			return Behavior_Result::Continue;
		}
		else if (result == Behavior_Result::Failure)	//	失敗なら次の子ノードへ
		{
			++m_Index;
		}

	}
	//	子クラス全部失敗ならリセットとFailure返す
	m_Index = 0;
	return Behavior_Result::Failure;
}

//	===========================Idle状態==============================
//	Enemy	:	Enemy*	保持者のポインタ
//	Type	:	std::string	再生するアニメーションの名前
BehaviorIdle::BehaviorIdle(Enemy* Enemy, const std::string& Type):BehaviorNode(Enemy)
{
	m_AnimationName = Type;
}

//=========================Idle初期化=========================
void BehaviorIdle::Init()
{
	if (!m_AnimationModel)return;
	//	現在のアニメーションはこのステートのアニメーションと同じじゃなければ
	if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
	{
		m_AnimationModel->SetNextAnimation(m_AnimationName);
	}

}

//	=========================Idle状態更新=========================
//	DeltaTime	:	float	デルタタイム
Behavior_Result BehaviorIdle::Update(const float DeltaTime)
{
	if (!m_AnimationModel)return Behavior_Result::Failure;

	//	プレイヤーとの距離を計算
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR direction = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(direction));
	if (length < SENSE_DISTANCE)
	{
		// 距離内になったら次のノードへ
		return Behavior_Result::Success;
	}
	if (m_AnimationModel->GetNextAnimationName() != m_AnimationName)
	{
		Init();
	}
	m_AnimationModel->UpdateAnimationBlend();
	return Behavior_Result::Continue;
}

//	=========================ノードのベース=========================
//	Enemy	:	Enemy*	ノードの保持者
BehaviorNode::BehaviorNode(Enemy* Enemy)
{
	m_Enemy = Enemy;
	m_AnimationModel = m_Enemy->GetAnimationModel();	//	アニメーションプールから取り出す
	Scene* scene = SceneManager::GetCurrentScene();
	if (!scene)return;
	GameObjectManager* gameObjectManager = scene->GetGameObjectManager();
	if (!gameObjectManager)return;
	Player* player = gameObjectManager->GetGameObject<Player>(GameObject_Type::Player);
	if (player)m_Player = player;
}

//	=========================子ノード追加=========================
//	Node	:	BehaviorNode	親ノードポインタ
void BehaviorNode::AddChildNode(BehaviorNode* Node)
{	
	//	ノード追加
	Node->m_Parent = this;
	m_Child.emplace_back(Node);
}

//	=========================移動状態ノード=========================
//	Enemy	:	Enemy*	保持者のポインタ
//	Type	:	std::string	再生するアニメーションの名前
BehaviorMove::BehaviorMove(Enemy* Enemy, const std::string& Type) :BehaviorNode(Enemy)
{
	
	m_AnimationName = Type;
}

//=========================移動状態初期化=========================
void BehaviorMove::Init()
{
	if (!m_AnimationModel)return;
	//	現在のアニメーションはこのステートのアニメーションと同じじゃなければ
	if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
	{
		m_AnimationModel->SetNextAnimation(m_AnimationName);
	}
}
//	=========================移動状態更新=========================
//	DeltaTime	:	float	デルタタイム
Behavior_Result BehaviorMove::Update(const float DeltaTime)
{
	if (!m_AnimationModel)return Behavior_Result::Failure;
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(vector));
	
	//	攻撃範囲内チェック
	if (length < ATTACK_DISTANCE)
	{
		m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
		return Behavior_Result::Success;
	}
	else if (length > FOLLOW_DISTANCE)
	{
		m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
		return Behavior_Result::Failure;
	}

	//	次遷移の状態が移動状態じゃなければ初期化する
	if (m_AnimationModel->GetNextAnimationName() != m_AnimationName)
	{
		Init();
	}

	//	向き更新
	XMVECTOR normalizeDirection = XMVector3Normalize(vector);
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, normalizeDirection);
	float yaw = atan2f(direction.x, direction.z);
	m_Enemy->SetRotationY(yaw); 
	m_Enemy->SetMoveDirection(direction);
	m_AnimationModel->UpdateAnimationBlend();
	return Behavior_Result::Continue;
}

//	=========================攻撃状態初期化=========================
void BehaviorAttack::Init()
{
	if (!m_AnimationModel)return;
	//	アニメーション設置
	if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
	{
		m_AnimationModel->SetNextAnimation(m_AnimationName);
	}

	m_IsAttackStart = true;
	m_BehaviorCoolDown->StartCoolDown();
	
}

BehaviorAttack::BehaviorAttack(Enemy* Enemy, const std::string& Type,const float AttackDistance):BehaviorNode(Enemy),m_AttackDistance(AttackDistance)
{
	m_AnimationName = Type;
	m_BehaviorCoolDown = new BehaviorCoolDown(m_Enemy);	// CoolDown螳溯｣・

}

BehaviorAttack::~BehaviorAttack()
{
	delete m_BehaviorCoolDown;
}

Behavior_Result BehaviorAttack::Update(const float DeltaTime)
{
	if (!m_AnimationModel)return Behavior_Result::Failure;
	if (!m_BehaviorCoolDown)return Behavior_Result::Failure;
	
	//	攻撃したかどうかチェック
	if (!m_IsAttackStart)
	{
		Init();
	}

	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(vector));

	//	一定距離離れたらアタック状態から離れる
	if (length > SENSE_DISTANCE)
	{
		return Behavior_Result::Failure;
	}
	//	攻撃範囲より遠いなら近づく
	if (length >= m_AttackDistance)
	{
		XMVECTOR normalizeDirection = XMVector3Normalize(vector);
		XMFLOAT3 direction;
		XMStoreFloat3(&direction, normalizeDirection);
		m_Enemy->SetMoveDirection(direction);
		float yaw = atan2f(direction.x, direction.z);
		m_Enemy->SetRotationY(yaw); 
		return Behavior_Result::Continue;
	}
	else
	{	
		//	範囲内になったら止まる
		m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
	}


	//	アニメーションは再生終わった、もしくは遷移中じゃないなら
	if (m_AnimationModel->GetIsTransitioning() || m_AnimationModel->GetCurrentAnimationFrame() <= m_AnimationModel->GetAnimationDuration(m_AnimationName))
	{
		m_AnimationModel->UpdateAnimationBlend();
		return Behavior_Result::Continue;
	}

	//======クールダウン======

	//	攻撃可能状態なら
	if (m_BehaviorCoolDown->Update(DeltaTime) == Behavior_Result::Failure)
	{
		return Behavior_Result::Failure;
	}
	//	攻撃リセット
	else
	{
		m_IsAttackStart = false;
		m_BehaviorCoolDown->ResetCoolDown();
		return Behavior_Result::Success;
	}


}
//	=========================クールダウン状態更新=========================
//	DeltaTime	:	float	デルタタイム
Behavior_Result BehaviorCoolDown::Update(const float DeltaTime)
{
	//	クールダウン状態じゃないならSuccess返す
	if (!m_IsCoolDownActive)return Behavior_Result::Success;

	m_ElapsedTime += DeltaTime;

	//	クールダウン状態完了したかどうか
	if (m_ElapsedTime >= ATTACK_COOLDOWN)
	{
		m_IsCoolDownActive = false;
		return Behavior_Result::Success;	
	}
	//	クールダウン中
	return Behavior_Result::Failure;		
}

//	=========================クールダウン状態開始=========================
void BehaviorCoolDown::StartCoolDown()
{
	m_ElapsedTime = 0;
	m_IsCoolDownActive = true;
}
//	=========================クールダウン状態更新リセット=========================
void BehaviorCoolDown::ResetCoolDown()
{
	m_IsCoolDownActive = false;
}

//	=========================攻撃待ち状態初期化=========================
void BehaviorStandByAttack::Init()
{
	if (!m_AnimationModel)return;
	
	if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
	{
		m_AnimationModel->SetNextAnimation(m_AnimationName);
	}
}

//	=========================攻撃待ち状態初期化=========================
//	Enemy	:	Enemy*	保持者のポインタ
//	Type	:	std::string	再生するアニメーションの名前
BehaviorStandByAttack::BehaviorStandByAttack(Enemy* Enemy, const std::string& Type):BehaviorNode(Enemy)
{
	m_AnimationName = Type;
}

//	=========================攻撃待ち状態初期化=========================
Behavior_Result BehaviorStandByAttack::Update(const float DeltaTime)
{
	if (!m_AnimationModel)return Behavior_Result::Failure;

	//	次遷移の状態が移動状態じゃなければ初期化する
	if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
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
	m_Enemy->SetRotationY(yaw); 

	m_AnimationModel->UpdateAnimationBlend();
	
	return Behavior_Result::Success;
}



