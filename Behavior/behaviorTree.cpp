#include"Manager\sceneManager.h"
#include "Manager\gameObjectManager.h"
#include "Scene\scene.h"
#include "Main\main.h"
#include "GameObject\Character\Player\playerh.h"
#include "GameObject\Character\Enemy\enemy.h"
#include "behaviorTree.h"

constexpr float SENSE_DISTANCE = 10.0f;
constexpr float FOLLOW_DISTANCE = 25.0f;
constexpr float ATTACK_DISTANCE = 5.0f;

BEHAVIOR_RESULT BehaviorSequence::Update()
{
	BEHAVIOR_RESULT result = m_Child[m_Index]->Update();
	switch (result)
	{
	case BEHAVIOR_RESULT::SUCCESS:
		++m_Index;
		if (m_Index < m_Child.size())
		{
			m_Child[m_Index]->Init();
			return BEHAVIOR_RESULT::CONTINUE;
		}
		if (m_Index >= m_Child.size())
		{
			m_Index = 0;
			return BEHAVIOR_RESULT::SUCCESS;
		}

	case BEHAVIOR_RESULT::CONTINUE:
		return BEHAVIOR_RESULT::CONTINUE;

	case BEHAVIOR_RESULT::FAILURE:
		m_Index = 0;
		return BEHAVIOR_RESULT::FAILURE;
	}
	return BEHAVIOR_RESULT::FAILURE;
}

BEHAVIOR_RESULT BehaviorSelector::Update()
{
	BEHAVIOR_RESULT result = m_Child[m_Index]->Update();
	switch (result)
	{
	case BEHAVIOR_RESULT::SUCCESS:
		m_Index = 0;
		return BEHAVIOR_RESULT::SUCCESS;
	case BEHAVIOR_RESULT::CONTINUE:

		return BEHAVIOR_RESULT::CONTINUE;
	case BEHAVIOR_RESULT::FAILURE:
		++m_Index;
		if (m_Index < m_Child.size())
		{
			m_Child[m_Index]->Init();
			return BEHAVIOR_RESULT::CONTINUE;
		}
		return BEHAVIOR_RESULT::FAILURE;
	}
	return BEHAVIOR_RESULT::SUCCESS;
}

void BehaviorIdle::Init()
{
	if (!m_AnimationModel)return;
	//	再設定防止
	if (m_AnimationModel->GetCurrentAnimationName() != "Idle")
	{
		m_AnimationModel->SetNextAnimation("Idle");
	}

}

BEHAVIOR_RESULT BehaviorIdle::Update()
{
	if (!m_AnimationModel)return BEHAVIOR_RESULT::FAILURE;
	if (m_AnimationModel->GetNextAnimationName() != "Idle")
	{
		Init();
	}
	m_AnimationModel->UpdateAnimationBlend();
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR direction = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(direction));
	if (length < SENSE_DISTANCE)
	{
		// 次の状態へ
		return BEHAVIOR_RESULT::SUCCESS;
	}
	return BEHAVIOR_RESULT::CONTINUE;
}

BehaviorNode::BehaviorNode(Enemy* Enemy)
{
	m_Enemy = Enemy;
	m_AnimationModel = m_Enemy->GetAnimationModel();
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

void BehaviorMove::Init()
{
	if (!m_AnimationModel)return;
	if (m_AnimationModel->GetCurrentAnimationName() != "Run")
	{
		m_AnimationModel->SetNextAnimation("Run");

	}
}

BEHAVIOR_RESULT BehaviorMove::Update()
{
	if (!m_AnimationModel)return BEHAVIOR_RESULT::FAILURE;
	if (m_AnimationModel->GetNextAnimationName() != "Run")
	{
		Init();
	}
	m_AnimationModel->UpdateAnimationBlend();
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(vector));
	if (length < ATTACK_DISTANCE)
	{
		m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
		return BEHAVIOR_RESULT::SUCCESS;
	}
	else if (length > FOLLOW_DISTANCE)
	{
		m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
		return BEHAVIOR_RESULT::SUCCESS;
	}
	XMVECTOR normalizeDirection = XMVector3Normalize(vector);
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, normalizeDirection);
	float yaw = atan2f(direction.x, direction.z);
	m_Enemy->SetRotation(XMFLOAT3(0, yaw, 0)); // この関数は敵のワールド行列や向きを更新するものとする
	m_Enemy->SetMoveDirection(direction);
	return BEHAVIOR_RESULT::CONTINUE;
}

void BehaviorAttack::Init()
{
	if (!m_AnimationModel)return;
	if (m_AnimationModel->GetCurrentAnimationName() != "Kick")
	{
		m_AnimationModel->SetNextAnimation("Kick");
	}
}

BEHAVIOR_RESULT BehaviorAttack::Update()
{
	if (!m_AnimationModel)return BEHAVIOR_RESULT::FAILURE;
	if (m_AnimationModel->GetNextAnimationName() != "Kick")
	{
		Init();
	}
	if (m_AnimationModel->GetIsTransitioning())
	{
		return BEHAVIOR_RESULT::CONTINUE;
	}
	else
	{
		return BEHAVIOR_RESULT::SUCCESS;
	}
	return BEHAVIOR_RESULT();
}
