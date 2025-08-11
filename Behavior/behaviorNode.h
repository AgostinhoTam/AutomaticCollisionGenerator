/*===================================================================================

ビヘイビアーツリー(BehaviorTree.h)

====================================================================================*/
#pragma once
#include <vector>
#include <string>
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





