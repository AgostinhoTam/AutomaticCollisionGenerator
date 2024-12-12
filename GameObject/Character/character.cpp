#include "character.h"
constexpr float FRICTION = 0.9f;
constexpr float MAX_DROP_SPEED = -50.0f;
constexpr float GRAVITY = -9.8f;
void Character::Update(const float& DeltaTime)
{
	XMVECTOR position = XMLoadFloat3(&m_Position);
	XMVECTOR velocity = XMLoadFloat3(&m_Velocity);

	UpdateHorizontalVelocity(velocity, DeltaTime);

	UpdateVerticalVelocity(velocity,DeltaTime);

	position = XMVectorMultiplyAdd(velocity, XMVectorReplicate(DeltaTime), position);
	
	XMStoreFloat3(&m_Position,position);
	XMStoreFloat3(&m_Velocity, velocity);

	m_MoveDirection = { 0,0,0 };
}

void Character::UpdateVerticalVelocity(XMVECTOR& Velocity, const float& DeltaTime)
{
	float groundHeight = 0.0f;

	float velocityY = XMVectorGetY(Velocity);

	if (m_IsGround)
	{
		velocityY = 0.0f;
	}
	else
	{
		velocityY += GRAVITY * DeltaTime;
		if (velocityY < MAX_DROP_SPEED)
		{
			velocityY = MAX_DROP_SPEED;
		}
	}
	Velocity = XMVectorSetY(Velocity,velocityY);
}

void Character::UpdateHorizontalVelocity(XMVECTOR& Velocity,const float& DeltaTime)
{
	//	平面方向正規化
	XMVECTOR dirNormalize = XMVectorSet(m_MoveDirection.x, 0.0f, m_MoveDirection.z, 0.0f); //xz移動
	dirNormalize = XMVector3Normalize(dirNormalize);

	//	加速度
	XMVECTOR accl = dirNormalize * m_MaxHorizontalAcclSpeed * DeltaTime;

	//	加速度適用
	Velocity = XMVectorAdd(Velocity, accl);

	XMVECTOR velocityXZ = XMVectorSet(XMVectorGetX(Velocity), 0.0f, XMVectorGetZ(Velocity), 0.0f);
	if (XMVectorGetX(dirNormalize) == 0.0f && XMVectorGetZ(dirNormalize) == 0.0f)
	{
		velocityXZ *= FRICTION;
		Velocity = XMVectorSet(XMVectorGetX(velocityXZ), XMVectorGetY(Velocity), XMVectorGetZ(velocityXZ), 0.0f);
	}

	//	速度上限
	float velocityMagnitude = XMVectorGetX(XMVector3Length(velocityXZ));
	if (velocityMagnitude > m_MaxMovementSpeed)
	{
		//	速度調整
		velocityXZ = XMVector3Normalize(velocityXZ) * m_MaxMovementSpeed;

		Velocity = XMVectorSet(XMVectorGetX(velocityXZ), XMVectorGetY(Velocity), XMVectorGetZ(velocityXZ), 0.0f);
	}
}
