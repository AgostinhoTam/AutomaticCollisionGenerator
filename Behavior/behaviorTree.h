/*===================================================================================

ビヘイビアーツリー(BehaviorTree.h)

====================================================================================*/
#pragma once
#include <vector>

class Enemy;
class Player;
class AnimationModel;

//	=====Enum宣言=====
enum class Current_Behavior
{
	Next,
	Idle,
	Walk,
	Attack,
	Max_Behavior
};
enum class Behavior_Result
{
	Success,
	Continue,
	Failure,
	Cannot_Excute,
	Max_Result
};
//	=========================

//	===========ノード基底クラス=========
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
	virtual Behavior_Result Update(const float DeltaTime) = 0;
	BehaviorNode() = delete;
	BehaviorNode(Enemy* Enemy);
	virtual ~BehaviorNode(){}
	void AddChildNode(BehaviorNode* Node);
};

// Sequenceノード
class BehaviorSequence :public BehaviorNode
{
private:
	int m_Index = 0;
public:
	using BehaviorNode::BehaviorNode;
	virtual Behavior_Result Update(const float DeltaTime)override;
};

//	Selectorノード
class BehaviorSelector :public BehaviorNode
{
private:
	int m_Index = 0;
public:
	using BehaviorNode::BehaviorNode;
	virtual Behavior_Result Update(const float DeltaTime)override;
};

//	クールダウンノード
class BehaviorCoolDown :public BehaviorNode
{
private:
	float m_ElapsedTime{};
	bool m_IsCoolDownActive = false;
public:
	using BehaviorNode::BehaviorNode;
	virtual Behavior_Result Update(const float DeltaTime)override;
	float GetElaspedTime() const { return m_ElapsedTime; }
	bool GetIsCoolDownActive()const { return m_IsCoolDownActive; }
	void StartCoolDown();
	void ResetCoolDown();
};

//	Idle状態ノード
class BehaviorIdle :public BehaviorNode
{
private:
	std::string m_AnimationName{};
public:
	virtual void Init()override;
	using BehaviorNode::BehaviorNode;
	BehaviorIdle(Enemy* Enemy, const std::string& Type);
	virtual Behavior_Result Update(const float DeltaTime)override;
};

//	移動状態ノード
class BehaviorMove :public BehaviorNode
{
private:
	std::string m_AnimationName{};
public:
	virtual void Init()override;
	using BehaviorNode::BehaviorNode;
	BehaviorMove(Enemy* Enemy, const std::string& Type);
	virtual Behavior_Result Update(const float DeltaTime)override;
};

//	攻撃待機ノード
class BehaviorStandByAttack : public BehaviorNode
{
private:
	std::string m_AnimationName{};
public:
	virtual void Init()override;
	using BehaviorNode::BehaviorNode;
	BehaviorStandByAttack(Enemy* Enemy, const std::string& Type);
	virtual Behavior_Result Update(const float DeltaTime)override;

};

//	攻撃状態ノード
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
	virtual Behavior_Result Update(const float DeltaTime)override;
};

