#pragma once
#include "Behavior\blackBoardCondition.h"

class IsPlayerVisibleNode :public BlackBoardCondition
{
public:
	virtual bool Evaluate(const BlackBoard& BlackBoard)const override;
};

class IsPlayerAttackNode :public BlackBoardCondition
{
public:
	virtual bool Evaluate(const BlackBoard& BlackBoard)const override;
};

class IsIdleCondition :public BlackBoardCondition
{
public:
	virtual bool Evaluate(const BlackBoard& BlackBoard)const override;
};

