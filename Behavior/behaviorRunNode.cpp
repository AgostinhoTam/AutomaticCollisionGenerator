#include "GameObject\Character\Enemy\enemy.h"
#include "Behavior\blackBoard.h"
#include "behaviorRunNode.h"

BehaviorRunNode::BehaviorRunNode(Enemy* Owner, const std::string& AnimationName): BehaviorNode(Owner), m_AnimationName(AnimationName)
{
	if (!m_Owner)return;

	m_BlackBoard = m_Owner->GetBlackBoard();
}

void BehaviorRunNode::Init()
{
	if (!m_Owner)return;
	m_Owner->GetAnimationModel()->SetNextAnimation(m_AnimationName);
}

BEHAVIOR_RESULT BehaviorRunNode::Update(const float DeltaTime)
{
	if (!m_Owner)return BEHAVIOR_RESULT::FAILURE;
	if (!m_BlackBoard)return BEHAVIOR_RESULT::FAILURE;

	if (m_Owner->GetAnimationModel()->GetNextAnimationName() != m_AnimationName)
	{
		Init();
	}

	if (m_BlackBoard->GetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_ATTACK_RANGE))
	{
		return BEHAVIOR_RESULT::SUCCESS;
	}
	m_Owner->MoveToTargetDirection();

    return BEHAVIOR_RESULT::RUNNING;
}
