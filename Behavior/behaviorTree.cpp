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
constexpr float ATTACK_COOLDOWN = 3.0f;

//	===========================蛻ｶ蠕｡繝弱・繝・==============================
BEHAVIOR_RESULT BehaviorSequence::Update(const float DeltaTime)
{
	while (m_Index < m_Child.size())
	{
		BEHAVIOR_RESULT result = m_Child[m_Index]->Update(DeltaTime);

		if (result == BEHAVIOR_RESULT::SUCCESS)	//	謌仙粥縺励◆繧画ｬ｡縺ｮ繝弱・繝峨∈
		{
			++m_Index;
			if (m_Index >= m_Child.size())
			{
				m_Index = 0;
				return BEHAVIOR_RESULT::SUCCESS;
			}
		}
		else if (result == BEHAVIOR_RESULT::CONTINUE)	//縲邯咏ｶ・
		{
			return BEHAVIOR_RESULT::CONTINUE;
		}
		else // 螟ｱ謨励＠縺溘ｉ繝ｪ繧ｻ繝・ヨ
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

//=========================繧ｹ繝・・繝・=========================
void BehaviorIdle::Init()
{
	if (!m_AnimationModel)return;
	//	蜀崎ｨｭ螳夐亟豁｢
	if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
	{
		m_AnimationModel->SetNextAnimation(m_AnimationName);
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
		// 谺｡縺ｮ迥ｶ諷九∈
		return BEHAVIOR_RESULT::SUCCESS;
	}
	if (m_AnimationModel->GetNextAnimationName() != m_AnimationName)
	{
		Init();
	}
	m_AnimationModel->UpdateAnimationBlend();
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
	//	繝・Μ繝ｼ霑ｽ蜉縺ｮ譎ょ盾辣ｧ霑ｽ蜉
	Node->m_Parent = this;
	m_Child.emplace_back(Node);
}

BehaviorMove::BehaviorMove(Enemy* Enemy, const std::string& Type) :BehaviorNode(Enemy)
{
	m_AnimationName = Type;
}

void BehaviorMove::Init()
{
	if (!m_AnimationModel)return;
	if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
	{
		m_AnimationModel->SetNextAnimation(m_AnimationName);
	}
}

BEHAVIOR_RESULT BehaviorMove::Update(const float DeltaTime)
{
	if (!m_AnimationModel)return BEHAVIOR_RESULT::FAILURE;
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(vector));
	
	//	荳螳夊ｷ晞屬縺ｧ蛛懈ｭ｢
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

	if (m_AnimationModel->GetNextAnimationName() != m_AnimationName)
	{
		Init();
	}
	XMVECTOR normalizeDirection = XMVector3Normalize(vector);
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, normalizeDirection);
	float yaw = atan2f(direction.x, direction.z);
	m_Enemy->SetRotationY(yaw); // 蜷代″譖ｴ譁ｰ
	m_Enemy->SetMoveDirection(direction);
	m_AnimationModel->UpdateAnimationBlend();
	return BEHAVIOR_RESULT::CONTINUE;
}

void BehaviorAttack::Init()
{
	if (!m_AnimationModel)return;
	//	迴ｾ蝨ｨ縺ｨ蜷後§縺ｪ繧峨せ繧ｭ繝・・
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

BEHAVIOR_RESULT BehaviorAttack::Update(const float DeltaTime)
{
	if (!m_AnimationModel)return BEHAVIOR_RESULT::FAILURE;
	if (!m_BehaviorCoolDown)return BEHAVIOR_RESULT::FAILURE;
	
	//	謾ｻ謦・・譛溷喧
	if (!m_IsAttackStart)
	{
		Init();
	}

	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(vector));

	//	霑ｽ縺・°縺代ｋ霍晞屬螟悶□縺｣縺溘ｉ
	if (length > SENSE_DISTANCE)
	{
		return BEHAVIOR_RESULT::FAILURE;
	}
	//	謾ｻ謦・ｯ・峇蜀・§繧・↑縺・凾
	if (length >= m_AttackDistance)
	{
		XMVECTOR normalizeDirection = XMVector3Normalize(vector);
		XMFLOAT3 direction;
		XMStoreFloat3(&direction, normalizeDirection);
		m_Enemy->SetMoveDirection(direction);
		float yaw = atan2f(direction.x, direction.z);
		m_Enemy->SetRotationY(yaw); 
		return BEHAVIOR_RESULT::CONTINUE;
	}
	else
	{	
		//遽・峇蜀・□縺｣縺溘ｉ豁｢縺ｾ繧・
		m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
	}


	//	驕ｷ遘ｻ荳ｭ縲√∪縺溘・繧｢繝九Γ繝ｼ繧ｷ繝ｧ繝ｳ縺ｮ蜀咲函荳ｭ縺縺｣縺溘ｉ
	if (m_AnimationModel->GetIsTransitioning() || m_AnimationModel->GetCurrentAnimationFrame() <= m_AnimationModel->GetAnimationDuration(m_AnimationName))
	{
		m_AnimationModel->UpdateAnimationBlend();
		return BEHAVIOR_RESULT::CONTINUE;
	}

	//======繧｢繝九Γ繝ｼ繧ｷ繝ｧ繝ｳ蜀咲函邨ゅｏ縺｣縺溷ｾ後・蜃ｦ逅・======

	//	繧ｯ繝ｼ繝ｫ繝繧ｦ繝ｳ荳ｭ縺縺｣縺溘ｉ谺｡縺ｮ繧｢繧ｿ繝・け縺ｸ
	if (m_BehaviorCoolDown->Update(DeltaTime) == BEHAVIOR_RESULT::FAILURE)
	{
		return BEHAVIOR_RESULT::FAILURE;
	}
	//	繧ｯ繝ｼ繝ｫ繝繧ｦ繝ｳ邨ゅｏ縺｣縺溘ｉ
	else
	{
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
		return BEHAVIOR_RESULT::SUCCESS;	// 繧ｯ繝ｼ繝ｫ繝繧ｦ繝ｳ邨ゆｺ・
	}
	return BEHAVIOR_RESULT::FAILURE;		//縲繧ｯ繝ｼ繝ｫ繝繧ｦ繝ｳ荳ｭ
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
	if (!m_AnimationModel)return;
	//	迴ｾ蝨ｨ縺ｨ蜷後§縺ｪ繧峨せ繧ｭ繝・・
	if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
	{
		m_AnimationModel->SetNextAnimation(m_AnimationName);
	}
}

BehaviorStandByAttack::BehaviorStandByAttack(Enemy* Enemy, const std::string& Type):BehaviorNode(Enemy)
{
	m_AnimationName = Type;
}

BEHAVIOR_RESULT BehaviorStandByAttack::Update(const float DeltaTime)
{
	if (!m_AnimationModel)return BEHAVIOR_RESULT::FAILURE;

	//	蛻晄悄蛹・
	if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
	{
		Init();
	}

	//	蜷代″譖ｴ譁ｰ
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(vector));
	XMVECTOR normalizeDirection = XMVector3Normalize(vector);
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, normalizeDirection);
	float yaw = atan2f(direction.x, direction.z);
	m_Enemy->SetRotationY(yaw); //	蜷代″縺ｯ1蝗槭□縺第峩譁ｰ縺吶ｋ

	m_AnimationModel->UpdateAnimationBlend();
	
	//縲荳逡ｪ謇句燕縺ｮ迥ｶ諷九↓霑斐☆
	return BEHAVIOR_RESULT::SUCCESS;
}



