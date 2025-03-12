#pragma once
#include <vector>
#include "System\Enum\behaviorTreeEnum.h"
class Enemy;

class BehaviorNode
{
protected:
	Enemy* m_Owner;
	std::vector<BehaviorNode*> m_Child;
	int m_CurrentIndex = 0;
public:
	BehaviorNode(Enemy* Owner):m_Owner(Owner){}
	virtual void Init(){}
	virtual void Uninit(){}
	virtual ~BehaviorNode() = default;
	virtual BEHAVIOR_RESULT Update(const float DeltaTime) = 0;
	void AddChildNode(BehaviorNode* Node);
};

class BehaviorSelector :public BehaviorNode
{
public:
	using BehaviorNode::BehaviorNode;

	BEHAVIOR_RESULT Update(const float DeltaTime)override;
};

class BehaviorSequence :public BehaviorNode
{
private:
public:
	using BehaviorNode::BehaviorNode;
	virtual BEHAVIOR_RESULT Update(const float DeltaTime)override;
};