#include "GameObject\Character\Enemy\enemy.h"
#include "Behavior\blackBoard.h"
#include "behaviorAttackNode.h"

BehaviorAttackNode::BehaviorAttackNode(Enemy* Owner, const std::string& AnimationName, float AttackCoolDown,float AttackRange) :BehaviorNode(Owner), m_AnimationName(AnimationName), m_AttackCoolDown(AttackCoolDown), m_AttackRange(AttackRange)
{
	if (!m_Owner)return;
	m_BlackBoard = m_Owner->GetBlackBoard();
}

void BehaviorAttackNode::Init()
{
	m_Owner->GetAnimationModel()->SetNextAnimation(m_AnimationName);
	m_IsAttacked = true;
	m_Owner->FaceTargetDirection();
}

BEHAVIOR_RESULT BehaviorAttackNode::Update(const float DeltaTime)
{
	if (!m_Owner)return BEHAVIOR_RESULT::FAILURE;
	if (!m_BlackBoard)return BEHAVIOR_RESULT::FAILURE;
	AnimationModel* animationModel = m_Owner->GetAnimationModel();
	if (!animationModel)return BEHAVIOR_RESULT::FAILURE;

	
	// 攻撃してないなら初期化
	if (!m_IsAttacked)
	{
		Init();
	}

	//	時間経過
	m_ElapsedTime += DeltaTime;



	//	アニメーションが遷移中
	if (animationModel->GetIsTransitioning())
	{
		return BEHAVIOR_RESULT::RUNNING;
	}

	//	攻撃クールダウン完了
	if (m_ElapsedTime >= m_AttackCoolDown)
	{
		m_ElapsedTime = 0.0f;
		m_IsAttacked = false;
		return BEHAVIOR_RESULT::SUCCESS;
	}

	//	アニメーションが終了したら
	if (animationModel->GetCurrentAnimationFrame() >= animationModel->GetAnimationDuration(m_AnimationName))
	{
		return BEHAVIOR_RESULT::SUCCESS;
	}

	// プレイヤーが攻撃範囲外なら追いかける
	if (m_Owner->GetTargetDistance() > m_AttackRange)
	{
		return BEHAVIOR_RESULT::SUCCESS;
	}

	return BEHAVIOR_RESULT::RUNNING;
}
