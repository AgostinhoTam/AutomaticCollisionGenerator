#pragma once
#include "collision.h"
class SphereCollision :public Collision
{
public:
	SphereCollision(GameObject* Owner, XMFLOAT3 Offset, float Radius) :Collision(Owner, Offset), m_Radius(Radius) {}
	virtual bool IsCollisionOverlapping(const Collision* Collision)const override;
	virtual bool IsCollisionHit(const Collision* Collision)const override;
	bool IsCollisionOverlapping(const SphereCollision* Collision)const;
	float GetRadius()const { return m_Radius; }
	virtual void UpdateCollision()override;
	void Init()override;
	virtual void Draw()override;
private:
	float m_Radius{};
};

