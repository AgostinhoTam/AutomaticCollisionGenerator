#pragma once
#include "Behavior/behaviorNode.h"

//	移動状態ノード
class BehaviorMove :public BehaviorNode
{
private:
    std::string m_AnimationName{};
    float m_ChaseDistance{};
public:
    virtual void Init()override;
    using BehaviorNode::BehaviorNode;
    BehaviorMove(Enemy* Enemy, const std::string& Type);
    virtual Behavior_Result Update(const float DeltaTime)override;
};

