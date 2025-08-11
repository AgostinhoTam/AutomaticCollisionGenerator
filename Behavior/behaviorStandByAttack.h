#pragma once
#include "Behavior/behaviorNode.h"

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