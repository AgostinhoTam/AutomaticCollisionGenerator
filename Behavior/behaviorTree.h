#pragma once

#include <vector>
class Enemy;
class Player;
class AnimationModel;
enum class BEHAVIOR_RESULT
{
	SUCCESS,
	CONTINUE,
	FAILURE,
	MAX_RESULT
};
class BehaviorNode
{
public:
	virtual void Init(){}
	virtual BEHAVIOR_RESULT Update() = 0;
	BehaviorNode() = delete;
	BehaviorNode(Enemy* Enemy);
	void AddChildNode(BehaviorNode* Node) { m_Child.emplace_back(Node); }
protected:
	std::vector<BehaviorNode*> m_Child;
	Enemy* m_Enemy;
	Player* m_Player;
	AnimationModel* m_AnimationModel;
};

// BehaviorTreeSequence
class BehaviorSequence :public BehaviorNode
{
public:
	using BehaviorNode::BehaviorNode;
	virtual BEHAVIOR_RESULT Update()override;

private:
	int m_Index = 0;
};

class BehaviorSelector :public BehaviorNode
{
public:
	using BehaviorNode::BehaviorNode;
	virtual BEHAVIOR_RESULT Update()override;
private:
	int m_Index = 0;
};

class BehaviorIdle :public BehaviorNode
{
public:
	virtual void Init()override;
	using BehaviorNode::BehaviorNode;
	virtual BEHAVIOR_RESULT Update()override;
};

class BehaviorMove :public BehaviorNode
{
public:
	virtual void Init()override;
	using BehaviorNode::BehaviorNode;
	virtual BEHAVIOR_RESULT Update()override;
};

class BehaviorAttack : public BehaviorNode
{
public:
	virtual void Init()override;
	using BehaviorNode::BehaviorNode;
	virtual BEHAVIOR_RESULT Update()override;
};
