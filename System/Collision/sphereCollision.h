#pragma once
#include "collision.h"
class SphereCollision :public Collision
{
public:
	SphereCollision(GameObject* Owner, XMFLOAT3 Offset, float Radius);
	virtual bool IsCollisionOverlapping(const Collision* Collision)const override;
	virtual bool IsCollisionHit(const Collision* Collision)const override;
	bool IsCollisionOverlapping(const SphereCollision* Collision)const;
	float GetRadius()const { return m_Radius; }
	virtual void UpdateCollision()override;
	virtual void Init()override;
	virtual void Draw()override;
	void CreateLineVertex(std::vector<XMFLOAT3>& SphereLineVertices);
private:
	float m_Radius{};
	std::vector<XMFLOAT3> m_SphereLineVertices;
};

