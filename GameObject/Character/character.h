#pragma once
#include "GameObject/gameobject.h"
#include "Enum/moveDirection.h"
struct MODEL;
class Character :public GameObject
{
public:
	virtual void Init()override{}
	virtual void Update(const float& DeltaTime)override{}
	virtual void Draw()override{}
	virtual void Uninit()override{}
	const XMFLOAT3& GetVelocity() const { return m_Velocity; }
	void SetMoveForwardDirection(int forward) { m_MoveDirection.y = forward; }
	void SetMoveRightDirection(int right) { m_MoveDirection.x = right; }
protected:
	XMFLOAT3 m_Velocity{};
	MODEL* m_Model;
	XMINT2 m_MoveDirection;
};