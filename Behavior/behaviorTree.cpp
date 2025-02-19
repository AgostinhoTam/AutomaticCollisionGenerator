#include"Manager\sceneManager.h"
#include "Manager\gameObjectManager.h"
#include "Scene\scene.h"
#include "Main\main.h"
#include "GameObject\Character\Player\playerh.h"
#include "GameObject\Character\Enemy\enemy.h"
#include "behaviorTree.h"

constexpr float SENSE_DISTANCE = 10.0f;
constexpr float FOLLOW_DISTANCE = 25.0f;
constexpr float ATTACK_DISTANCE = 5.0f;
constexpr float ATTACK_COOLDOWN = 3.0f;

//	===========================����m�[�h===============================
BEHAVIOR_RESULT BehaviorSequence::Update(const float DeltaTime)
{
	while (m_Index < m_Child.size())
	{
		BEHAVIOR_RESULT result = m_Child[m_Index]->Update(DeltaTime);

		if (result == BEHAVIOR_RESULT::SUCCESS)
		{
			++m_Index;
			if (m_Index >= m_Child.size())
			{
				m_Index = 0;
				return BEHAVIOR_RESULT::SUCCESS;
			}
		}
		else if (result == BEHAVIOR_RESULT::CONTINUE)
		{
			return BEHAVIOR_RESULT::CONTINUE;
		}
		else
		{
			m_Index = 0;
			return BEHAVIOR_RESULT::FAILURE;
		}
	}

	return BEHAVIOR_RESULT::FAILURE;
}

BEHAVIOR_RESULT BehaviorSelector::Update(const float DeltaTime)
{
	while (m_Index < m_Child.size())
	{
		BEHAVIOR_RESULT result = m_Child[m_Index]->Update(DeltaTime);
		if (result == BEHAVIOR_RESULT::SUCCESS)
		{
			m_Index = 0;
			return BEHAVIOR_RESULT::SUCCESS;
			break;
		}
		else if (result == BEHAVIOR_RESULT::CONTINUE)
		{
			return BEHAVIOR_RESULT::CONTINUE;
		}
		else
		{
			++m_Index;
		}

	}
	m_Index = 0;
	return BEHAVIOR_RESULT::FAILURE;
}
//====================================================================


//=========================�X�e�[�g==========================
void BehaviorIdle::Init()
{
	if (!m_AnimationModel)return;
	//	�Đݒ�h�~
	if (m_AnimationModel->GetCurrentAnimationName() != "Idle")
	{
		m_AnimationModel->SetNextAnimation("Idle");
	}

}

BEHAVIOR_RESULT BehaviorIdle::Update(const float DeltaTime)
{
	if (!m_AnimationModel)return BEHAVIOR_RESULT::FAILURE;
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR direction = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(direction));
	if (length < SENSE_DISTANCE)
	{
		// ���̏�Ԃ�
		return BEHAVIOR_RESULT::SUCCESS;
	}
	if (m_AnimationModel->GetNextAnimationName() != "Idle")
	{
		Init();
	}
	m_AnimationModel->UpdateAnimationBlend();
	return BEHAVIOR_RESULT::CONTINUE;
}

BehaviorNode::BehaviorNode(Enemy* Enemy)
{
	m_Enemy = Enemy;
	m_AnimationModel = m_Enemy->GetAnimationModel();
	Scene* scene = SceneManager::GetCurrentScene();
	if (!scene)return;
	GameObjectManager* gameObjectManager = scene->GetGameObjectManager();
	if (!gameObjectManager)return;
	Player* player = gameObjectManager->GetGameObject<Player>(GAMEOBJECT_TYPE::PLAYER);
	if (player)m_Player = player;
}

void BehaviorNode::AddChildNode(BehaviorNode* Node)
{
	//	�c���[�ǉ��̎��Q�ƒǉ�
	Node->m_Parent = this;
	m_Child.emplace_back(Node);
}

void BehaviorMove::Init()
{
	if (!m_AnimationModel)return;
	if (m_AnimationModel->GetCurrentAnimationName() != "Run")
	{
		m_AnimationModel->SetNextAnimation("Run");
	}
}

BEHAVIOR_RESULT BehaviorMove::Update(const float DeltaTime)
{
	if (!m_AnimationModel)return BEHAVIOR_RESULT::FAILURE;
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(vector));

	//	��苗���Œ�~
	if (length < ATTACK_DISTANCE)
	{
		m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
		return BEHAVIOR_RESULT::SUCCESS;
	}
	else if (length > FOLLOW_DISTANCE)
	{
		m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
		return BEHAVIOR_RESULT::FAILURE;
	}

	if (m_AnimationModel->GetNextAnimationName() != "Run")
	{
		Init();
	}
	XMVECTOR normalizeDirection = XMVector3Normalize(vector);
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, normalizeDirection);
	float yaw = atan2f(direction.x, direction.z);
	m_Enemy->SetRotationY(yaw); // �����X�V
	m_Enemy->SetMoveDirection(direction);
	m_AnimationModel->UpdateAnimationBlend();
	return BEHAVIOR_RESULT::CONTINUE;
}

void BehaviorAttack::Init()
{
	if (!m_AnimationModel)return;
	//	���݂Ɠ����Ȃ�X�L�b�v
	if (m_AnimationModel->GetCurrentAnimationName() != m_AnimationName)
	{
		m_AnimationModel->SetNextAnimation(m_AnimationName);
	}

	m_IsAttackStart = true;
	m_BehaviorCoolDown->StartCoolDown();

}

BehaviorAttack::BehaviorAttack(Enemy* Enemy, const std::string& Type, const float AttackDistance) :BehaviorNode(Enemy), m_AttackDistance(AttackDistance)
{
	m_AnimationName = Type;
	m_BehaviorCoolDown = new BehaviorCoolDown(m_Enemy);	// CoolDown����

}

BehaviorAttack::~BehaviorAttack()
{
	delete m_BehaviorCoolDown;
}

BEHAVIOR_RESULT BehaviorAttack::Update(const float DeltaTime)
{
	if (!m_AnimationModel)return BEHAVIOR_RESULT::FAILURE;
	if (!m_BehaviorCoolDown)return BEHAVIOR_RESULT::FAILURE;

	//	�U��������
	if (!m_IsAttackStart)
	{
		Init();
	}

	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(vector));

	//	�ǂ������鋗���O��������
	if (length > SENSE_DISTANCE)
	{
		return BEHAVIOR_RESULT::FAILURE;
	}
	//	�U���͈͓�����Ȃ���
	if (length >= m_AttackDistance)
	{
		XMVECTOR normalizeDirection = XMVector3Normalize(vector);
		XMFLOAT3 direction;
		XMStoreFloat3(&direction, normalizeDirection);
		m_Enemy->SetMoveDirection(direction);
		float yaw = atan2f(direction.x, direction.z);
		m_Enemy->SetRotationY(yaw);
		return BEHAVIOR_RESULT::CONTINUE;
	}
	else
	{
		//�͈͓���������~�܂�
		m_Enemy->SetMoveDirection(XMFLOAT3(0, 0, 0));
	}


	//	�J�ڒ��A�܂��̓A�j���[�V�����̍Đ�����������
	if (m_AnimationModel->GetIsTransitioning() || m_AnimationModel->GetCurrentAnimationFrame() <= m_AnimationModel->GetAnimationDuration("Attack"))
	{
		m_AnimationModel->UpdateAnimationBlend();
		return BEHAVIOR_RESULT::CONTINUE;
	}

	//======�A�j���[�V�����Đ��I�������̏���=======

	//	�N�[���_�E�����������玟�̃A�^�b�N��
	if (m_BehaviorCoolDown->Update(DeltaTime) == BEHAVIOR_RESULT::FAILURE)
	{
		return BEHAVIOR_RESULT::FAILURE;
	}
	//	�N�[���_�E���I�������
	else
	{
		m_IsAttackStart = false;
		m_BehaviorCoolDown->ResetCoolDown();
		return BEHAVIOR_RESULT::SUCCESS;
	}


}

BEHAVIOR_RESULT BehaviorCoolDown::Update(const float DeltaTime)
{
	if (!m_IsCoolDownActive)return BEHAVIOR_RESULT::SUCCESS;

	m_ElapsedTime += DeltaTime;
	if (m_ElapsedTime >= ATTACK_COOLDOWN)
	{
		m_IsCoolDownActive = false;
		return BEHAVIOR_RESULT::SUCCESS;	// �N�[���_�E���I��
	}
	return BEHAVIOR_RESULT::FAILURE;		//�@�N�[���_�E����
}

void BehaviorCoolDown::StartCoolDown()
{
	m_ElapsedTime = 0;
	m_IsCoolDownActive = true;
}
void BehaviorCoolDown::ResetCoolDown()
{
	m_IsCoolDownActive = false;

}
//===================================================================

void BehaviorStandByAttack::Init()
{
	if (!m_AnimationModel)return;
	//	���݂Ɠ����Ȃ�X�L�b�v
	if (m_AnimationModel->GetCurrentAnimationName() != "Idle")
	{
		m_AnimationModel->SetNextAnimation("Idle");
	}
}

BEHAVIOR_RESULT BehaviorStandByAttack::Update(const float DeltaTime)
{
	if (!m_AnimationModel)return BEHAVIOR_RESULT::FAILURE;

	//	������
	if (m_AnimationModel->GetCurrentAnimationName() != "Idle")
	{
		Init();
	}

	//	�����X�V
	XMVECTOR playerPosition = XMLoadFloat3(&m_Player->GetPosition());
	XMVECTOR enemyPosition = XMLoadFloat3(&m_Enemy->GetPosition());
	XMVECTOR vector = XMVectorSubtract(playerPosition, enemyPosition);
	float length = XMVectorGetX(XMVector3Length(vector));
	XMVECTOR normalizeDirection = XMVector3Normalize(vector);
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, normalizeDirection);
	float yaw = atan2f(direction.x, direction.z);
	m_Enemy->SetRotationY(yaw); //	������1�񂾂��X�V����

	m_AnimationModel->UpdateAnimationBlend();

	//�@��Ԏ�O�̏�ԂɕԂ�
	return BEHAVIOR_RESULT::SUCCESS;
}



