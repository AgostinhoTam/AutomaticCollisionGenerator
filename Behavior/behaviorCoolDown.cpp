#include "behaviorCoolDown.h"

#include "GameObject/Character/Enemy/enemy.h"

//	=========================クールダウン状態更新=========================
//	DeltaTime	:	float	デルタタイム
Behavior_Result BehaviorCoolDown::Update(const float DeltaTime)
{
    //	クールダウン状態じゃないならSuccess返す
    if (!m_IsCoolDownActive)return Behavior_Result::Success;

    m_ElapsedTime += DeltaTime;

    //	クールダウン状態完了したかどうか
    if (m_ElapsedTime >= m_Enemy->GetAttackCoolDownTime())
    {
        m_IsCoolDownActive = false;
        return Behavior_Result::Success;	
    }
    //	クールダウン中
    return Behavior_Result::Failure;		
}

//	=========================クールダウン状態開始=========================
void BehaviorCoolDown::StartCoolDown()
{
    m_ElapsedTime = 0;
    m_IsCoolDownActive = true;
}
//	=========================クールダウン状態更新リセット=========================
void BehaviorCoolDown::ResetCoolDown()
{
    m_IsCoolDownActive = false;
}