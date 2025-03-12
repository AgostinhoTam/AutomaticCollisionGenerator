#pragma once
class BlackBoard;

class BlackBoardCondition
{
public:
	virtual ~BlackBoardCondition() = default;
	virtual bool Evaluate(const BlackBoard& BlackBoard)const = 0;
};