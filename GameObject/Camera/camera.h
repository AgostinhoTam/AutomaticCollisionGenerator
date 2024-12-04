#pragma once
/*===================================================================================

カメラ制御(camera.h)

====================================================================================*/
#include "GameObject/gameobject.h"
class Player;
class Camera:public GameObject 
{
private:

	XMFLOAT3			m_Target = XMFLOAT3{0.0f,0.0f,0.0f};				// カメラの注視点
	XMFLOAT3			m_Up = XMFLOAT3{ 0.0f,0.0f,0.0f };;				// カメラの上方向ベクトル
	float				m_Len = 0.0f;			// カメラの視点と注視点の距離

	XMFLOAT4X4			m_MtxView{};		// ビューマトリックス
	XMMATRIX			m_MtxInvView{};		// ビューマトリックス
	XMMATRIX			m_MtxProjection{};	// プロジェクションマトリックス
	Player*				m_Player;
	float				m_Sensitivity = 0.0f;
public:
	void Init();
	void Update(const float& DeltaTime);
	void Uninit();
	void Draw();
	const XMMATRIX& GetViewMatrix() const { return XMLoadFloat4x4(&m_MtxView); }
};
