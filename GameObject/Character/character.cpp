#include "character.h"
constexpr float FRICTION = 0.98f;

void Character::Update(const float& DeltaTime)
{
	XMVECTOR position = XMLoadFloat3(&m_Position);
	XMVECTOR velocity = XMLoadFloat3(&m_Velocity);
	XMVECTOR accl = XMLoadFloat3(&m_Accl);
	XMVECTOR dir = XMVectorSet(m_MoveDirection.x, 0.0f, m_MoveDirection.y, 0.0f); //xz�ړ�
	//	�������K��
	XMVECTOR dirNormalize = XMVector3Normalize(dir);
	
	//	���x
	XMVECTOR targetVelocity = dirNormalize * m_MaxMovementSpeed;

	//	�����x
	accl = dirNormalize * m_AcclSpeed * DeltaTime;

	//	�ŏI���x
	XMVECTOR finalVelocity = XMVectorAdd(accl, velocity);
	
	//	����
	finalVelocity *= FRICTION;

	XMVECTOR velocityXZ = XMVectorSet(XMVectorGetX(finalVelocity), XMVectorGetZ(finalVelocity),0.0f,0.0f);
	float velocityMagnitude = XMVectorGetX(XMVector2Length(velocityXZ));

	if (velocityMagnitude > m_MaxMovementSpeed)
	{
		finalVelocity = targetVelocity;
	}

	position = XMVectorMultiplyAdd(finalVelocity, XMVectorReplicate(DeltaTime), position);
	
	XMStoreFloat3(&m_Position,position);
	XMStoreFloat3(&m_Velocity, finalVelocity);

	m_MoveDirection = { 0,0 };
}
