#include "Behavior\blackBoard.h"
#include "conditionNode.h"
bool IsPlayerVisibleNode::Evaluate(const BlackBoard& BlackBoard)const
{
    return BlackBoard.GetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_SIGHT);
}

bool IsPlayerAttackNode::Evaluate(const BlackBoard& BlackBoard)const
{
    return BlackBoard.GetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_ATTACK_RANGE);
}

bool IsIdleCondition::Evaluate(const BlackBoard& BlackBoard) const
{
    return (!BlackBoard.GetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_ATTACK_RANGE) && !BlackBoard.GetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_SIGHT));
}
