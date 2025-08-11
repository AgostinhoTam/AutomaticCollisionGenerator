#pragma once
#include "Behavior/behaviorNode.h"
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