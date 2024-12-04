
#include "GameObject/gameobject.h"

const XMFLOAT3 GameObject::GetForward()const
{
	XMMATRIX rotationMatrix;
	rotationMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

	XMFLOAT3 forward;
	XMStoreFloat3(&forward, rotationMatrix.r[2]);
	return forward;
}

const XMFLOAT3 GameObject::GetRight()const
{
	XMMATRIX rotationMatrix;
	rotationMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

	XMFLOAT3 right;
	XMStoreFloat3(&right, rotationMatrix.r[0]);
	return right;
}

const XMFLOAT3 GameObject::GetUp()const
{
	XMMATRIX rotationMatrix;
	rotationMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

	XMFLOAT3 up;
	XMStoreFloat3(&up, rotationMatrix.r[1]);
	return up;
}

XMFLOAT3 GameObject::XMQuaternionToEulerAngle(XMVECTOR Quat)
{
	XMFLOAT4 quaternion;
	XMStoreFloat4(&quaternion, Quat);
	
	XMFLOAT3 eulerRotation;
	eulerRotation.x = asinf(2.0f * (quaternion.w * quaternion.x - quaternion.y * quaternion.z)); // Pitch
	eulerRotation.y = atan2f(2.0f * (quaternion.w * quaternion.y + quaternion.x * quaternion.z), 1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.y * quaternion.y)); // Yaw
	eulerRotation.z = atan2f(2.0f * (quaternion.w * quaternion.z + quaternion.x * quaternion.y), 1.0f - 2.0f * (quaternion.y * quaternion.y + quaternion.z * quaternion.z)); // Roll
	
	return eulerRotation;
}
