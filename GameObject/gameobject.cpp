
#include "GameObject/gameobject.h"

XMFLOAT3 GameObject::GetForward()const
{
	XMMATRIX rotationMatrix;
	rotationMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

	XMFLOAT3 forward;
	XMStoreFloat3(&forward, rotationMatrix.r[2]);
	return forward;
}

XMFLOAT3 GameObject::GetRight()const
{
	XMMATRIX rotationMatrix;
	rotationMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

	XMFLOAT3 right;
	XMStoreFloat3(&right, rotationMatrix.r[0]);
	return right;
}

XMFLOAT3 GameObject::GetUp()const
{
	XMMATRIX rotationMatrix;
	rotationMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

	XMFLOAT3 up;
	XMStoreFloat3(&up, rotationMatrix.r[1]);
	return up;
}
