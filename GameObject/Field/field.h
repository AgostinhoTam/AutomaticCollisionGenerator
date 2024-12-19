#pragma once
#include "GameObject/gameobject.h"
#include "System\Renderer/renderer.h"
class Field:public GameObject {
private:
	VERTEX_3D vertex[4];
public:
	Field(const XMFLOAT3& location, const XMFLOAT2& size);
	void Init()override;
	void Update(const float& DeltaTime)override;
	void Draw()override;
	void Uninit()override;
};