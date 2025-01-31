#pragma once
#include "collision.h"
class CapsuleCollision :public Collision
{
public:
	CapsuleCollision(const XMFLOAT3& StartPosition, const XMFLOAT3& Offset, float Radius);	// 引数（Ownerポインタ、Offset値、半径、高さ）
	virtual bool IsCollisionOverlapping(const Collision* Collision)const override;
	bool IsCollisionOverlapping(const CapsuleCollision* Collision)const;
	virtual void UpdateCollision(const XMFLOAT3& Position)override;
	virtual void Init()override;
	virtual void Draw()override;
	void CreateLineVertex(std::vector<XMFLOAT3>& SphereLineVertices);
private:
	float m_Height{};
	float m_Radius{};
	std::vector<XMFLOAT3> m_SphereLineVertices;
};

