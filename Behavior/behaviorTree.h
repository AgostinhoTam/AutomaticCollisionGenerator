#pragma once

#include <vector>
class Enemy;
class Player;
class AnimationModel;
enum class CURRENT_BEHAVIOR
{
	NEXT,
	IDLE,
	WALK,
	ATTACK,
	MAX_BEHAVIOR
};
enum class BEHAVIOR_RESULT
{
	SUCCESS,
	CONTINUE,
	FAILURE,
	CANNOT_EXCUTE,
	MAX_RESULT
};
class BehaviorNode
{
protected:
	std::vector<BehaviorNode*> m_Child;
	BehaviorNode* m_Parent{};
	Enemy* m_Enemy{};
	Player* m_Player{};
	AnimationModel* m_AnimationModel{};
public:
	virtual void Init() {}
	virtual BEHAVIOR_RESULT Update(const float DeltaTime) = 0;
	BehaviorNode() = delete;
	BehaviorNode(Enemy* Enemy);
	virtual ~BehaviorNode(){}
	void AddChildNode(BehaviorNode* Node);
};

// BehaviorTreeSequence
class BehaviorSequence :public BehaviorNode
{
private:
	int m_Index = 0;
public:
	using BehaviorNode::BehaviorNode;
	virtual BEHAVIOR_RESULT Update(const float DeltaTime)override;
};

class BehaviorSelector :public BehaviorNode
{
private:
	int m_Index = 0;
public:
	using BehaviorNode::BehaviorNode;
	virtual BEHAVIOR_RESULT Update(const float DeltaTime)override;
};

class BehaviorCoolDown :public BehaviorNode
{
private:
	float m_ElapsedTime{};
	bool m_IsCoolDownActive = false;
public:
	using BehaviorNode::BehaviorNode;
	virtual BEHAVIOR_RESULT Update(const float DeltaTime)override;
	float GetElaspedTime() const { return m_ElapsedTime; }
	bool GetIsCoolDownActive()const { return m_IsCoolDownActive; }
	void StartCoolDown();
	void ResetCoolDown();
};

class BehaviorIdle :public BehaviorNode
{
private:
	std::string m_AnimationName{};
public:
	virtual void Init()override;
	using BehaviorNode::BehaviorNode;
	BehaviorIdle(Enemy* Enemy, const std::string& Type);
	virtual BEHAVIOR_RESULT Update(const float DeltaTime)override;
};

class BehaviorMove :public BehaviorNode
{
private:
	std::string m_AnimationName{};
public:
	virtual void Init()override;
	using BehaviorNode::BehaviorNode;
	BehaviorMove(Enemy* Enemy, const std::string& Type);
	virtual BEHAVIOR_RESULT Update(const float DeltaTime)override;
};

class BehaviorStandByAttack : public BehaviorNode
{
private:
	std::string m_AnimationName{};
public:
	virtual void Init()override;
	using BehaviorNode::BehaviorNode;
	BehaviorStandByAttack(Enemy* Enemy, const std::string& Type);
	virtual BEHAVIOR_RESULT Update(const float DeltaTime)override;

};

class BehaviorAttack : public BehaviorNode
{
private:
	std::string m_AnimationName{};
	BehaviorCoolDown* m_BehaviorCoolDown{};
	bool m_IsAttackStart = false;
	float m_AttackDistance = 0.0f;
public:
	virtual void Init()override;
	BehaviorAttack(Enemy* Enemy, const std::string& Type,const float AttackDistance);
	~BehaviorAttack();
	using BehaviorNode::BehaviorNode;
	virtual BEHAVIOR_RESULT Update(const float DeltaTime)override;
	float GetAttackDistance()const { return m_AttackDistance; }
};

