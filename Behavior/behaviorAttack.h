#pragma once
#include "Behavior/behaviorNode.h"
class BehaviorCoolDown;

//	攻撃状態ノード
class BehaviorAttack : public BehaviorNode
{
private:
    std::string m_AnimationName{};
    BehaviorCoolDown* m_BehaviorCoolDown{};
    bool m_IsAttackStart = false;
    float m_AttackDistance = 0.0f;
    float m_SenseDistance = 0.0f;
public:
    virtual void Init()override;
    BehaviorAttack(Enemy* Enemy, const std::string& Type,const float AttackDistance);
    ~BehaviorAttack();
    using BehaviorNode::BehaviorNode;
    virtual Behavior_Result Update(const float DeltaTime)override;
};
