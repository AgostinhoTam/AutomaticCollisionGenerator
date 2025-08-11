#include "main.h"
#include "System/Renderer/animationModel.h"
#include "GameObject/Character/Enemy/enemy.h"
#include "GameObject/Character/Player/player.h"
#include "Behavior/behaviorMove.h"

//	=========================移動状態ノード=========================
//	Enemy	:	Enemy*	保持者のポインタ
//	Type	:	std::string	再生するアニメーションの名前
BehaviorMove::BehaviorMove(Enemy* Enemy, const std::string& Type) :BehaviorNode(Enemy)
{
	
	m_AnimationName = Type;
}

//=========================移動状態初期化=========================
void BehaviorMove::Init()
{
	if (!m_AnimationModel)return;
	//	現在のアニメーションはこのステートのアニメーションと同じじゃなければ
	if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
	{
		m_AnimationModel->SetNextAnimation(m_AnimationName);
	}
}
//	=========================移動状態更新=========================
//	DeltaTime	:	float	デルタタイム
Behavior_Result BehaviorMove::Update(const float DeltaTime)
{
	if (!m_AnimationModel)return Behavior_Result::Failure;
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(vector));
	
	//	攻撃範囲内チェック
	if (length < m_Enemy->GetAttackDistance())
	{
		m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
		return Behavior_Result::Success;
	}
	else if (length > m_Enemy->GetChaseDistance())
	{
		m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
		return Behavior_Result::Failure;
	}

	//	次遷移の状態が移動状態じゃなければ初期化する
	if (m_AnimationModel->GetNextAnimationName() != m_AnimationName)
	{
		Init();
	}

	//	向き更新
	XMVECTOR normalizeDirection = XMVector3Normalize(vector);
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, normalizeDirection);
	float yaw = atan2f(direction.x, direction.z);
	m_Enemy->SetRotationY(yaw); 
	m_Enemy->SetMoveDirection(direction);
	m_AnimationModel->UpdateAnimationBlend();
	return Behavior_Result::Continue;
}
