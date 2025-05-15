/*===================================================================================

2D板ポリゴン_UI用(polygon2D.h)

====================================================================================*/
#pragma once
#include "System/Renderer/renderer.h"
#include "GameObject/gameobject.h"

class Polygon2D:public GameObject {
private:
	VERTEX_3D vertex[4];	//	頂点データ
public:
	Polygon2D(XMFLOAT2 location, XMFLOAT2 size);
	void Init()override;
	void Uninit()override;
	void Update(const float& DeltaTime)override;
	void Draw()override;
};
