#pragma once
#include "collision.h"
class SphereCollision :public Collision
{
private:
	float m_Radius{};
	std::vector<XMFLOAT3> m_SphereLineVertices;
public:
	SphereCollision(const XMFLOAT3& Position, const XMFLOAT3& Offset, float Radius);
	virtual bool IsCollisionOverlapping(const Collision* Collision) override;
	bool CheckSphereToSphere(const SphereCollision* Collision);
	float GetRadius()const { return m_Radius; }
	virtual void UpdateCollision(const XMFLOAT3& Position)override;
	virtual void Init()override;
	virtual void Draw()override;
	void CreateLineVertex(std::vector<XMFLOAT3>& SphereLineVertices);
};

