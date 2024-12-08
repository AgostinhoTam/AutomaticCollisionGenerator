#pragma once
#include "GameObject/gameobject.h"
#include "Enum/moveDirection.h"
struct MODEL;
class AnimationModel;
class Character :public GameObject
{
public:
	virtual void Init()override{}
	virtual void Update(const float& DeltaTime)override;
	virtual void Draw()override{}
	virtual void Uninit()override{}
	const XMFLOAT3& GetVelocity() const { return m_Velocity; }
	const XMFLOAT3& GetAccl()const { return m_Accl; }
	const XMFLOAT3& GetMoveDirection()const { return m_MoveDirection; }
	void SetMoveForwardDirection(float forward) { m_MoveDirection.z = forward; }
	void SetMoveRightDirection(float right) { m_MoveDirection.x = right; }
	void SetMoveDirection(XMFLOAT3 direction) { m_MoveDirection = direction; }
	void UpdateVerticalVelocity(XMVECTOR& Velocity, const float& DeltaTime);
	void UpdateHorizontalVelocity(XMVECTOR& Velocity, const float& DeltaTime);
protected:
	XMFLOAT3 m_Velocity{};
	XMFLOAT3 m_Accl{};
	MODEL* m_Model;
	AnimationModel* m_AnimationModel;
	XMFLOAT3 m_MoveDirection;
	float	m_MaxMovementSpeed = 0;
	float	m_MaxHorizontalAcclSpeed = 0;
	float	m_MaxJumpSpeed = 0;
	bool	m_IsGround = false;
};