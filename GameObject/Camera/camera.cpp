/*===================================================================================

�J��������(camera.cpp)

====================================================================================*/
#include "Main/main.h"
#include "GameObject/Camera/camera.h"
#include "Renderer/renderer.h"
/*===================================================================================

�}�N����`

====================================================================================*/

void Camera::Init()
{
	m_Position = XMFLOAT3(0.0f, 5.0f, -10.0f);
	m_Target = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Up = GetUp();
}

void Camera::Update(const float& DeltaTime)
{
	
}

void Camera::Uninit()
{
}

void Camera::Draw()
{
	//�r���[�}�g���N�X�ݒ�

	XMMATRIX viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&m_Position), XMLoadFloat3(&m_Target), XMLoadFloat3(&m_Up));

	Renderer::SetViewMatrix(viewMatrix);

	XMStoreFloat4x4(&m_MtxView, viewMatrix);

	//�v���W�F�N�V�����}�g���N�X�ݒ�i��p�A��ʂ̐ݒ�j
	XMMATRIX projectionMatrix;
	projectionMatrix = XMMatrixPerspectiveFovLH(1.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);
	
	Renderer::SetProjectionMatrix(projectionMatrix);
}

