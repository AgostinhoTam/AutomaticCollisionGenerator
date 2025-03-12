#pragma once
#include "Behavior\behaviorNode.h"
#include <string>

class BehaviorIdleNode:public BehaviorNode
{
private:
	std::string m_AnimationName = "";
	BlackBoard* m_BlackBoard = nullptr;
public:
	using BehaviorNode::BehaviorNode;
	BehaviorIdleNode(Enemy* Owner, const std::string& AnimationName);
	virtual void Init()override;
	virtual BEHAVIOR_RESULT Update(const float DeltaTime)override;
};
