#pragma once
#include <vector>
class BlackBoardCondition;
class BlackBoard;
class ConditionManager
{
private:
	std::vector<BlackBoardCondition*> m_Conditions;
public:
	~ConditionManager();
	void Uninit();
	void AddCondition(BlackBoardCondition* Condition);
	bool EvaluateAny(const BlackBoard& BlackBoard);
};