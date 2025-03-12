#include "Behavior\blackBoard.h"
#include "Behavior\blackBoardCondition.h"
#include "conditionManager.h"

ConditionManager::~ConditionManager()
{
	for (auto& condition : m_Conditions)
	{
		delete condition;
	}
	m_Conditions.clear();
}

void ConditionManager::Uninit()
{
	for (auto& condition : m_Conditions)
	{
		delete condition;
	}
	m_Conditions.clear();
}

void ConditionManager::AddCondition(BlackBoardCondition* Condition)
{
	m_Conditions.emplace_back(Condition);
}

bool ConditionManager::EvaluateAny(const BlackBoard& BlackBoard)
{
	for (const auto& condition : m_Conditions)
	{
		if (condition->Evaluate(BlackBoard))
		{
			return true;
		}
	}
	return false;
}
