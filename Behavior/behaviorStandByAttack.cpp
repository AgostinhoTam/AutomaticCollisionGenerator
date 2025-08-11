#include "main.h"
#include "GameObject/Character/Player/player.h"
#include "GameObject/Character/Enemy/enemy.h"
#include "System/Renderer/animationModel.h"
#include "behaviorStandByAttack.h"

//	=========================攻撃待ち状態初期化=========================
void BehaviorStandByAttack::Init()
{
    if (!m_AnimationModel)return;
	
    if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
    {
        m_AnimationModel->SetNextAnimation(m_AnimationName);
    }
}

//	=========================攻撃待ち状態初期化=========================
//	Enemy	:	Enemy*	保持者のポインタ
//	Type	:	std::string	再生するアニメーションの名前
BehaviorStandByAttack::BehaviorStandByAttack(Enemy* Enemy, const std::string& Type):BehaviorNode(Enemy)
{
    m_AnimationName = Type;
}

//	=========================攻撃待ち状態初期化=========================
Behavior_Result BehaviorStandByAttack::Update(const float DeltaTime)
{
    if (!m_AnimationModel)return Behavior_Result::Failure;

    //	次遷移の状態が移動状態じゃなければ初期化する
    if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
    {
        Init();
    }

    //	向き更新
    XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
    XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
    XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
    float length = XMVectorGetX(XMVector3Length(vector));
    XMVECTOR normalizeDirection = XMVector3Normalize(vector);
    XMFLOAT3 direction;
    XMStoreFloat3(&direction, normalizeDirection);
    float yaw = atan2f(direction.x, direction.z);
    m_Enemy->SetRotationY(yaw); 

    m_AnimationModel->UpdateAnimationBlend();
	
    return Behavior_Result::Success;
}