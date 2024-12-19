/*===================================================================================

カメラ制御(camera.cpp)

====================================================================================*/
#include "Main/main.h"
#include "Manager/sceneManager.h"
#include "Manager/gameObjectManager.h"
#include "Manager/inputManager.h"
#include "Scene/scene.h"
#include "System\Renderer/renderer.h"
#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Camera/camera.h"
/*===================================================================================
定数
====================================================================================*/
constexpr float CAMERA_LEN = 10.0f;
constexpr float CAMERA_SENSITIVE = 0.01f;
void Camera::Init()
{
	m_Position = XMFLOAT3(0.0f, 5.0f, -10.0f);
	m_Target = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Up = GetUp();
	m_Name = "Camera";
	Scene* scene = SceneManager::GetCurrentScene();
	if (scene)
	{
		GameObjectManager* gameObjectManager = scene->GetGameObjectManager();
		if (gameObjectManager)
		{
			m_Player = gameObjectManager->GetGameObject<Player>(GAMEOBJECT_TYPE::PLAYER);
		}
	}
	m_Sensitivity = CAMERA_SENSITIVE;
	m_Len = CAMERA_LEN;
}

void Camera::Update(const float& DeltaTime)
{
	if (!m_Player)return;

	XMFLOAT3 targetPos = m_Player->GetPosition();
	POINT mouseDeltaPos = InputManager::GetMouseDelta();
	
	m_Target = targetPos;

	m_Rotation.y += mouseDeltaPos.x * m_Sensitivity;
	m_Rotation.x += mouseDeltaPos.y * m_Sensitivity;

	const float maxXRotation = XM_PIDIV2 - 0.1f;
	const float minXRotation = -XM_PIDIV2 + 0.1f;
	m_Rotation.x = std::max(minXRotation, std::min(maxXRotation, m_Rotation.x));

	XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

	XMVECTOR target = XMLoadFloat3(&m_Target);
	XMVECTOR defaultPosition = XMVectorSet(0.0f, 0.0f, -m_Len, 1.0f);
	XMVECTOR cameraPosition = XMVector3Rotate(defaultPosition, rotationQuat);
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

	XMMATRIX viewMatrix = XMLoadFloat4x4(&m_MtxView);
	Renderer::SetViewMatrix(viewMatrix);

	//プロジェクションマトリクス設定（画角、画面の設定）
	XMMATRIX projectionMatrix;
	projectionMatrix = XMMatrixPerspectiveFovLH(1.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);
	
	Renderer::SetProjectionMatrix(projectionMatrix);
}

bool Camera::CheckView(const XMFLOAT3& Pos)
{
	XMFLOAT3 up{ 0.0f,1.0f,0.0f };
	XMMATRIX viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&m_Position), XMLoadFloat3(&m_Target), XMLoadFloat3(&up));
	//プロジェクションマトリクス設定		カメラの視界を設定する関数
	XMMATRIX projectionMatrix;
	projectionMatrix = XMMatrixPerspectiveFovLH(1.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);

	XMMATRIX vp, invvp;
	XMVECTOR det;
	vp = viewMatrix * projectionMatrix;
	invvp = XMMatrixInverse(&det, vp);

	//	座標変換4つ分
	XMFLOAT3 vpos[4];
	XMFLOAT3 wpos[4];

	vpos[0] = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	vpos[1] = XMFLOAT3(1.0f, 1.0f, 1.0f);
	vpos[2] = XMFLOAT3(-1.0f, -1.0f, 1.0f);
	vpos[3] = XMFLOAT3(1.0f, -1.0f, 1.0f);

	XMVECTOR vposv[4];
	XMVECTOR wposv[4];

	vposv[0] = XMLoadFloat3(&vpos[0]);
	vposv[1] = XMLoadFloat3(&vpos[1]);
	vposv[2] = XMLoadFloat3(&vpos[2]);
	vposv[3] = XMLoadFloat3(&vpos[3]);

	wposv[0] = XMVector3TransformCoord(vposv[0], invvp);
	wposv[1] = XMVector3TransformCoord(vposv[1], invvp);
	wposv[2] = XMVector3TransformCoord(vposv[2], invvp);
	wposv[3] = XMVector3TransformCoord(vposv[3], invvp);

	//	視錐台の座標
	XMStoreFloat3(&wpos[0], wposv[0]);
	XMStoreFloat3(&wpos[1], wposv[1]);
	XMStoreFloat3(&wpos[2], wposv[2]);
	XMStoreFloat3(&wpos[3], wposv[3]);

	XMFLOAT3 v, v1, v2, n;

	v.x = Pos.x - m_Position.x;
	v.y = Pos.y - m_Position.y;
	v.z = Pos.z - m_Position.z;

	// 左面
	{	//　左上
		v1.x = wpos[0].x - m_Position.x;
		v1.y = wpos[0].y - m_Position.y;
		v1.z = wpos[0].z - m_Position.z;
		//　左下
		v2.x = wpos[2].x - m_Position.x;
		v2.y = wpos[2].y - m_Position.y;
		v2.z = wpos[2].z - m_Position.z;

		//	外積	順番大事y z z yとか(外積の公式)
		n.x = v1.y * v2.z - v1.z * v2.y;
		n.y = v1.z * v2.x - v1.x * v2.z;
		n.z = v1.x * v2.y - v1.y * v2.x;

		//	正規化

		//	内積
		float dot = n.x * v.x + n.y * v.y + n.z * v.z;
		if (dot < 0.0f)
		{
			return false;
		}
	}
	//	右面
	{
		//	右上
		v1.x = wpos[3].x - m_Position.x;
		v1.y = wpos[3].y - m_Position.y;
		v1.z = wpos[3].z - m_Position.z;
		//　右下
		v2.x = wpos[1].x - m_Position.x;
		v2.y = wpos[1].y - m_Position.y;
		v2.z = wpos[1].z - m_Position.z;

		//	外積	順番大事y z z yとか
		n.x = v1.y * v2.z - v1.z * v2.y;
		n.y = v1.z * v2.x - v1.x * v2.z;
		n.z = v1.x * v2.y - v1.y * v2.x;

		//	内積
		float dot = n.x * v.x + n.y * v.y + n.z * v.z;
		if (dot < 0.0f)
		{
			return false;
		}
	}
	//	上面
	{
		//　左上
		v1.x = wpos[1].x - m_Position.x;
		v1.y = wpos[1].y - m_Position.y;
		v1.z = wpos[1].z - m_Position.z;
		//　右上
		v2.x = wpos[0].x - m_Position.x;
		v2.y = wpos[0].y - m_Position.y;
		v2.z = wpos[0].z - m_Position.z;

		//	外積	順番大事y z z yとか
		n.x = v1.y * v2.z - v1.z * v2.y;
		n.y = v1.z * v2.x - v1.x * v2.z;
		n.z = v1.x * v2.y - v1.y * v2.x;

		//	内積
		float dot = n.x * v.x + n.y * v.y + n.z * v.z;
		if (dot < 0.0f)
		{
			return false;
		}
	}
	//	下面
	{
		//　左下
		v1.x = wpos[2].x - m_Position.x;
		v1.y = wpos[2].y - m_Position.y;
		v1.z = wpos[2].z - m_Position.z;
		//　右下
		v2.x = wpos[3].x - m_Position.x;
		v2.y = wpos[3].y - m_Position.y;
		v2.z = wpos[3].z - m_Position.z;

		//	外積	順番大事y z z yとか
		n.x = v1.y * v2.z - v1.z * v2.y;
		n.y = v1.z * v2.x - v1.x * v2.z;
		n.z = v1.x * v2.y - v1.y * v2.x;

		//	内積
		float dot = n.x * v.x + n.y * v.y + n.z * v.z;
		if (dot < 0.0f)
		{
			return false;
		}
	}
	//	奥面
	{
		v1.x = wpos[1].x - wpos[0].x;
		v1.y = wpos[1].y - wpos[0].y;
		v1.z = wpos[1].z - wpos[0].z;

		v2.x = wpos[2].x - wpos[0].x;
		v2.y = wpos[2].y - wpos[0].y;
		v2.z = wpos[2].z - wpos[0].z;

		//	外積	順番大事y z z yとか
		n.x = v1.y * v2.z - v1.z * v2.y;
		n.y = v1.z * v2.x - v1.x * v2.z;
		n.z = v1.x * v2.y - v1.y * v2.x;

		//	内積
		float dot = n.x * v.x + n.y * v.y + n.z * v.z;
		if (dot > 0.0f)
		{
			return false;
		}
	}
	return true;
}

XMFLOAT2 Camera::WordToScreen(const XMFLOAT3 Pos)
{
	return XMFLOAT2();
}
