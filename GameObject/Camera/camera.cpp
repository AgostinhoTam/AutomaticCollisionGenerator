/*===================================================================================

カメラ制御(camera.cpp)

====================================================================================*/
#include "Main/main.h"
#include "GameObject/Camera/camera.h"
#include "Renderer/renderer.h"
/*===================================================================================

マクロ定義

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
	//ビューマトリクス設定

	XMMATRIX viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&m_Position), XMLoadFloat3(&m_Target), XMLoadFloat3(&m_Up));

	Renderer::SetViewMatrix(viewMatrix);

	XMStoreFloat4x4(&m_MtxView, viewMatrix);

	//プロジェクションマトリクス設定（画角、画面の設定）
	XMMATRIX projectionMatrix;
	projectionMatrix = XMMatrixPerspectiveFovLH(1.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);
	
	Renderer::SetProjectionMatrix(projectionMatrix);
}

