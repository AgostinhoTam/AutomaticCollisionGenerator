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
	_Position = XMFLOAT3(0.0f, 5.0f, -10.0f);
	_Target = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Camera::Update()
{
	
}

void Camera::Uninit()
{
}

void Camera::Draw()
{
	//�r���[�}�g���N�X�ݒ�
	_Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&_Position), XMLoadFloat3(&_Target), XMLoadFloat3(&_Up));

	Renderer::SetViewMatrix(viewMatrix);

	XMStoreFloat4x4(&_mtxView, viewMatrix);

	//�v���W�F�N�V�����}�g���N�X�ݒ�i��p�A��ʂ̐ݒ�j
	XMMATRIX projectionMatrix;
	projectionMatrix = XMMatrixPerspectiveFovLH(1.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);
	
	Renderer::SetProjectionMatrix(projectionMatrix);
}

