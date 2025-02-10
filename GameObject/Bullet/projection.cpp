
#include "projection.h"
#include "System\Collision\sphereCollision.h"

void Projection::Init()
{
	switch (m_ProjectionType)
	{
	case PROJECTION_TYPE::BEAM:
		m_Collision = new SphereCollision(m_Position, XMFLOAT3(0, 0, 0), 1);
		m_Name = "Beam";
		break;
	case PROJECTION_TYPE::MAX_TYPE:
		break;
	default:
		break;
	}
}

void Projection::Update(const float& DeltaTime)
{
}

void Projection::Draw()
{
}

void Projection::Uninit()
{
}
