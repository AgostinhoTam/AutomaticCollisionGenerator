#pragma once
/*===================================================================================

カメラ制御(camera.h)

====================================================================================*/
#include "GameObject/gameobject.h"

class Camera:public GameObject 
{
private:
	XMFLOAT3			_Position;			// カメラの視点(位置)
	XMFLOAT3			_Target;				// カメラの注視点
	XMFLOAT3			_Up;				// カメラの上方向ベクトル
	XMFLOAT3			_rot;			// カメラの回転
	float				_len;			// カメラの視点と注視点の距離

	XMFLOAT4X4			_mtxView;		// ビューマトリックス
	XMMATRIX			_mtxInvView;		// ビューマトリックス
	XMMATRIX			_mtxProjection;	// プロジェクションマトリックス
public:
	void Init();
	void Update();
	void Uninit();
	void Draw();
};
