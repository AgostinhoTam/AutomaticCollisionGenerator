#pragma once
#include "Main/main.h"
#include "Renderer/renderer.h"
#include "GameObject/gameobject.h"

class Field:public GameObject {
private:
	VERTEX_3D vertex[4];
	XMFLOAT3 _scale;
	XMFLOAT3 _rotation;
	XMFLOAT3 _position;
public:
	Field(XMFLOAT3 location, XMFLOAT2 size);
	void Init()override;
	void Update()override;
	void Draw()override;
	void Uninit()override;
};