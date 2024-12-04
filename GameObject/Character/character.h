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
	const XMFLOAT2& GetMoveDirection()const { return m_MoveDirection; }
	void SetMoveForwardDirection(float forward) { m_MoveDirection.y = forward; }
	void SetMoveRightDirection(float right) { m_MoveDirection.x = right; }
	void SetMoveDirection(XMFLOAT2 direction) { m_MoveDirection = direction; }
protected:
	XMFLOAT3 m_Velocity{};
	XMFLOAT3 m_Accl{};
	MODEL* m_Model;
	AnimationModel* m_AnimationModel;
	XMFLOAT2 m_MoveDirection;
	float	m_MaxMovementSpeed = 0;
	float	m_AcclSpeed = 0;
};