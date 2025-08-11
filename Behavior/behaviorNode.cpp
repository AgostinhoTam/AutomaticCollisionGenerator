/*===================================================================================

	ビヘイビアーツリー(BehaviorTree.cpp)

====================================================================================*/
#include"Manager/sceneManager.h"
#include "Manager/gameObjectManager.h"
#include "Scene/scene.h"
#include "main.h"
#include "GameObject/Character/Player/player.h"
#include "GameObject/Character/Enemy/enemy.h"
#include "behaviorNode.h"

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







