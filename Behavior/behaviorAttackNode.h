#pragma once
#include "Behavior\behaviorNode.h"
#include <string>

class BehaviorAttackNode:public BehaviorNode
{
private:
	std::string m_AnimationName = "";
	BlackBoard* m_BlackBoard = nullptr;
	float	m_AttackRange = 0.0f;
	float	m_AttackCoolDown = 0.0f;
	float	m_ElapsedTime = 0.0f;
	bool	m_IsAttacked = false;
public:
	using BehaviorNode::BehaviorNode;
	BehaviorAttackNode(Enemy* Owner, const std::string& AnimationName,float AttackCoolDown, float AttackRange);
	virtual void Init()override;
	virtual BEHAVIOR_RESULT Update(const float DeltaTime)override;
};
