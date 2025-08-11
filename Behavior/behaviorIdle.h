#pragma once
#include "Behavior/behaviorNode.h"

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