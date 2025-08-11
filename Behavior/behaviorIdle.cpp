#include "System/Renderer/animationModel.h"
#include "main.h"
#include "GameObject/Character/Player/player.h"
#include "GameObject/Character/Enemy/enemy.h"
#include "behaviorIdle.h"
//	===========================Idle状態==============================
//	Enemy	:	Enemy*	保持者のポインタ
//	Type	:	std::string	再生するアニメーションの名前
BehaviorIdle::BehaviorIdle(Enemy* Enemy, const std::string& Type):BehaviorNode(Enemy)
{
    m_AnimationName = Type;
}

//=========================Idle初期化=========================
void BehaviorIdle::Init()
{
    if (!m_AnimationModel)return;
    //	現在のアニメーションはこのステートのアニメーションと同じじゃなければ
    if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
    {
        m_AnimationModel->SetNextAnimation(m_AnimationName);
    }

}

//	=========================Idle状態更新=========================
//	DeltaTime	:	float	デルタタイム
Behavior_Result BehaviorIdle::Update(const float DeltaTime)
{
    if (!m_AnimationModel)return Behavior_Result::Failure;

    //	プレイヤーとの距離を計算
    XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
    XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
    XMVECTOR direction = XMVectorSubtract(playerPosition, enemyPosition);
    float length = XMVectorGetX(XMVector3Length(direction));
    if (length < m_Enemy->GetSenseDistance())
    {
        // 距離内になったら次のノードへ
        return Behavior_Result::Success;
    }
    if (m_AnimationModel->GetNextAnimationName() != m_AnimationName)
    {
        Init();
    }
    m_AnimationModel->UpdateAnimationBlend();
    return Behavior_Result::Continue;
}