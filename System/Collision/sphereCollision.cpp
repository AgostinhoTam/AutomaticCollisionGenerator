#include <vector>
#include "GameObject\gameobject.h"
#include "System\Renderer\renderer.h"
#include "sphereCollision.h"
#ifdef _DEBUG
constexpr int DEBUG_LINE_SEGMENTS = 18;	//デバッグ用の線の分割数
#endif // _DEBUG


bool SphereCollision::IsCollisionOverlapping(const Collision* Collision) const
{
	if (!Collision)return false;
	return IsCollisionOverlapping(this);
}

bool SphereCollision::IsCollisionHit(const Collision* Collision) const
{
	if (!Collision)return false;
	return IsCollisionHit(this);
}

bool SphereCollision::IsCollisionOverlapping(const SphereCollision* Collision) const
{
	if (!Collision)return false;
	XMVECTOR ownerPosition = XMLoadFloat3(&m_Position);
	XMVECTOR otherPosition = XMLoadFloat3(&Collision->m_Position);
	float radiusSum = m_Radius + Collision->m_Radius;

	XMVECTOR positionSubtract = XMVectorSubtract(ownerPosition, otherPosition);
	float distance = XMVectorGetX(XMVector3LengthSq(positionSubtract));

	return (distance <= (radiusSum * radiusSum));

}

void SphereCollision::UpdateCollision()
{
	if (!m_Owner)return;
	XMFLOAT3 ownerPosition = m_Owner->GetPosition();
	m_Position = { ownerPosition.x + m_Offset.x,ownerPosition.y + m_Offset.y, ownerPosition.z + m_Offset.z };
}

void SphereCollision::Init()
{
	if (!m_Owner)return;
	UpdateCollision();
}

void SphereCollision::Draw()
{
	std::vector<XMFLOAT3> lineVertices;

	//	XZ平面
	for (int i = 0; i < DEBUG_LINE_SEGMENTS; ++i)
	{
		float angle = XM_2PI * i / DEBUG_LINE_SEGMENTS;
		float x = m_Radius * cosf(angle);
		float z = m_Radius * sinf(angle);
		lineVertices.emplace_back(XMFLOAT3{ m_Position.x + x,m_Position.y,m_Position.z + z });
	}

	//	YZ平面
	for (int i = 0; i < DEBUG_LINE_SEGMENTS; ++i)
	{
		float angle = XM_2PI * i / DEBUG_LINE_SEGMENTS;
		float y = m_Radius * cosf(angle);
		float z = m_Radius * sinf(angle);
		lineVertices.emplace_back(XMFLOAT3{ m_Position.x,m_Position.y+y,m_Position.z + z });
	}


	//	XY平面
	for (int i = 0; i < DEBUG_LINE_SEGMENTS; ++i)
	{
		float angle = XM_2PI * i / DEBUG_LINE_SEGMENTS;
		float x = m_Radius * cosf(angle);
		float y = m_Radius * sinf(angle);
		lineVertices.emplace_back(XMFLOAT3{ m_Position.x+x,m_Position.y + y,m_Position.z});
	}

	for (size_t i = 0; i < lineVertices.size(); ++i)
	{
		
	}
}
