#include "blackBoard.h"



bool BlackBoard::IsPlayerInAttackRange()
{
    return GetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_ATTACK_RANGE);
}

bool BlackBoard::IsPlayerInSight()
{
	return GetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_SIGHT);
}

bool BlackBoard::IsIdle()
{
	return (!GetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_ATTACK_RANGE) && !GetValue<bool>(BEHAVIOR_CONDITION::IS_PLAYER_IN_SIGHT));
}

bool BlackBoard::IsAttackCoolTime()
{
	return false;
}

