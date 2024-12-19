#pragma once
#include "Main\main.h"
class GameObject;
class Collision
{
public:
	Collision(GameObject* Owner, XMFLOAT3 Offset) :m_Owner(Owner), m_Offset(Offset) {}
	virtual ~Collision() {}
	virtual bool IsCollisionOverlapping(const Collision* Collision)const = 0;
	virtual bool IsCollisionHit(const Collision* Collision)const = 0;
	virtual void UpdateCollision() {}
	virtual void Init() {}
	virtual void Draw() {}
	const XMFLOAT3& GetOffset() { return m_Offset; }
	bool GetEnable() const { return m_IsEnable; }
	void SetEnable(const bool Flag) { m_IsEnable = Flag; }
protected:
	GameObject* m_Owner{};
	XMFLOAT3 m_Position{};
	XMFLOAT3 m_Offset{};
	bool m_IsEnable = true;
};