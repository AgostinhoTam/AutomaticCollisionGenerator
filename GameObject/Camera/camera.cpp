/*===================================================================================

カメラ制御(camera.cpp)

====================================================================================*/
#include "Main/main.h"
#include "Manager/sceneManager.h"
#include "Manager/gameObjectManager.h"
#include "Manager/inputManager.h"
#include "Scene/scene.h"
#include "Renderer/renderer.h"
#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Camera/camera.h"
/*===================================================================================

マクロ定義

====================================================================================*/

void Camera::Init()
{
	m_Position = XMFLOAT3(0.0f, 5.0f, -10.0f);
	m_Target = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Up = GetUp();
	Scene* scene = SceneManager::GetCurrentScene();
	if (scene)
	{
		GameObjectManager* gameObjectManager = scene->GetGameObjectManager();
		if (gameObjectManager)
		{
			m_Player = gameObjectManager->GetGameObject<Player>(GAMEOBJECT_TYPE::PLAYER);
		}
	}
	m_Sensitivity = 0.01f;
	m_Len = 10.0f;
}

void Camera::Update(const float& DeltaTime)
{
	if (!m_Player)return;
	XMFLOAT3 targetPos = m_Player->GetPosition();
	POINT mouseDeltaPos = InputManager::GetMouseDelta();
	
	m_Target = targetPos;

	m_Rotation.y += mouseDeltaPos.y*m_Sensitivity;
	m_Rotation.x += mouseDeltaPos.x*m_Sensitivity;

	const float maxYRotation = XM_PIDIV2 - 0.1f;
	const float minYRotation = -XM_PIDIV2 + 0.1f;
	m_Rotation.y = std::max(minYRotation, std::min(maxYRotation, m_Rotation.y));

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.y, m_Rotation.x, 0.0f);

	XMVECTOR target = XMLoadFloat3(&m_Target);
	XMVECTOR defaultPosition = XMVectorSet(0.0f, 0.0f, -m_Len, 1.0f);
	XMVECTOR cameraPosition = XMVector3Transform(defaultPosition, rotationMatrix);
	cameraPosition += target;

	XMStoreFloat3(&m_Position, cameraPosition);
	XMVECTOR up = XMLoadFloat3(&m_Up);

	XMMATRIX viewMatrix = XMMatrixLookAtLH(cameraPosition, target, up);

	XMStoreFloat4x4(&m_MtxView, viewMatrix);

}

void Camera::Uninit()
{
}

void Camera::Draw()
{
	//ビューマトリクス設定

	//XMMATRIX viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&m_Position), XMLoadFloat3(&m_Target), XMLoadFloat3(&m_Up));
	XMMATRIX viewMatrix = XMLoadFloat4x4(&m_MtxView);
	Renderer::SetViewMatrix(viewMatrix);

	//XMStoreFloat4x4(&m_MtxView, viewMatrix);

	//プロジェクションマトリクス設定（画角、画面の設定）
	XMMATRIX projectionMatrix;
	projectionMatrix = XMMatrixPerspectiveFovLH(1.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);
	
	Renderer::SetProjectionMatrix(projectionMatrix);
}

