#include"Manager\sceneManager.h"
#include "Manager\gameObjectManager.h"
#include "Scene\scene.h"
#include "Main\main.h"
#include "GameObject\Character\Player\playerh.h"
#include "GameObject\Character\Enemy\enemy.h"
#include "behaviorTree.h"
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
	m_AnimationModel->SetCurrentAnimation("Idle");
}

BEHAVIOR_RESULT BehaviorIdle::Update()
{
	if (!m_AnimationModel)return BEHAVIOR_RESULT::FAILURE;
	m_AnimationModel->UpdateAnimationBlend();
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR direction = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(direction));
	if (length < 15.0f)
	{
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
	m_Player = player;
}

void BehaviorMove::Init()
{
	if (!m_AnimationModel)return;
	m_AnimationModel->SetCurrentAnimation("Run");
}

BEHAVIOR_RESULT BehaviorMove::Update()
{
	if (!m_AnimationModel)return BEHAVIOR_RESULT::FAILURE;
	m_AnimationModel->UpdateAnimationBlend();
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR vector = XMVectorSubtract(playerPosition , enemyPosition);
	float length = XMVectorGetX(XMVector3Length(vector));
	XMVECTOR normalizeDirection = XMVector3Normalize(vector);
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, normalizeDirection);
	m_Enemy->SetMoveDirection(direction);
	if (length < 5.0f)
	{
		return BEHAVIOR_RESULT::SUCCESS;
	}
	return BEHAVIOR_RESULT::CONTINUE;
}

void BehaviorAttack::Init()
{
}

BEHAVIOR_RESULT BehaviorAttack::Update()
{
	return BEHAVIOR_RESULT();
}
