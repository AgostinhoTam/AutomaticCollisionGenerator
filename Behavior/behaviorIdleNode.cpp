#include "Behavior\blackBoard.h"
#include "GameObject\Character\Enemy\enemy.h"
#include "Behavior\behaviorIdleNode.h"

BehaviorIdleNode::BehaviorIdleNode(Enemy* Owner, const std::string& AnimationName) :BehaviorNode(Owner), m_AnimationName(AnimationName)
{
	if (!m_Owner)return;

	m_BlackBoard = m_Owner->GetBlackBoard();
}

void BehaviorIdleNode::Init()
{
	if (!m_Owner)return;
	m_Owner->GetAnimationModel()->SetNextAnimation(m_AnimationName);
}

BEHAVIOR_RESULT BehaviorIdleNode::Update(const float DeltaTime)
{
	if (!m_Owner)return BEHAVIOR_RESULT::FAILURE;
	if (!m_BlackBoard)return BEHAVIOR_RESULT::FAILURE;
	
	if (m_Owner->GetAnimationModel()->GetNextAnimationName() != m_AnimationName)
	{
		Init();
	}
	
	if (m_BlackBoard->GetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_SIGHT) || m_BlackBoard->GetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_ATTACK_RANGE))
	{
		return BEHAVIOR_RESULT::SUCCESS;
	}

	return BEHAVIOR_RESULT::RUNNING;

}
