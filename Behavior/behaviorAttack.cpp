#include "main.h"
#include "GameObject/Character/Player/player.h"
#include "GameObject/Character/Enemy/enemy.h"
#include "System/Renderer/animationModel.h"
#include "Behavior/behaviorCoolDown.h"
#include "behaviorAttack.h"
//	=========================攻撃状態初期化=========================
void BehaviorAttack::Init()
{
    if (!m_AnimationModel)return;
    //	アニメーション設置
    if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
    {
        m_AnimationModel->SetNextAnimation(m_AnimationName);
    }

    m_IsAttackStart = true;
    m_SenseDistance = m_Enemy->GetSenseDistance();
    m_BehaviorCoolDown->StartCoolDown();
}

BehaviorAttack::BehaviorAttack(Enemy* Enemy, const std::string& Type, const float AttackDistance): BehaviorNode(Enemy),
    m_AttackDistance(AttackDistance)
{
    m_AnimationName = Type;
    m_BehaviorCoolDown = new BehaviorCoolDown(m_Enemy); // CoolDown螳溯｣・
}

BehaviorAttack::~BehaviorAttack()
{
    delete m_BehaviorCoolDown;
}

Behavior_Result BehaviorAttack::Update(const float DeltaTime)
{
    if (!m_AnimationModel)return Behavior_Result::Failure;
    if (!m_BehaviorCoolDown)return Behavior_Result::Failure;

    //	攻撃したかどうかチェック
    if (!m_IsAttackStart)
    {
        Init();
    }

    XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
    XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
    XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
    float length = XMVectorGetX(XMVector3Length(vector));

    //	一定距離離れたらアタック状態から離れる
    if (length > m_SenseDistance)
    {
        return Behavior_Result::Failure;
    }
    //	攻撃範囲より遠いなら近づく
    if (length >= m_AttackDistance)
    {
        XMVECTOR normalizeDirection = XMVector3Normalize(vector);
        XMFLOAT3 direction;
        XMStoreFloat3(&direction, normalizeDirection);
        m_Enemy->SetMoveDirection(direction);
        float yaw = atan2f(direction.x, direction.z);
        m_Enemy->SetRotationY(yaw);
        return Behavior_Result::Continue;
    }
    else
    {
        //	範囲内になったら止まる
        m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
    }


    //	アニメーションは再生終わった、もしくは遷移中じゃないなら
    if (m_AnimationModel->GetIsTransitioning() || m_AnimationModel->GetCurrentAnimationFrame() <= m_AnimationModel->
        GetAnimationDuration(m_AnimationName))
    {
        m_AnimationModel->UpdateAnimationBlend();
        return Behavior_Result::Continue;
    }

    //======クールダウン======

    //	攻撃可能状態なら
    if (m_BehaviorCoolDown->Update(DeltaTime) == Behavior_Result::Failure)
    {
        return Behavior_Result::Failure;
    }
    //	攻撃リセット
    else
    {
        m_IsAttackStart = false;
        m_BehaviorCoolDown->ResetCoolDown();
        return Behavior_Result::Success;
    }
}
