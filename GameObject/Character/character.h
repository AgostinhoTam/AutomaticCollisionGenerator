#pragma once
#include "GameObject/gameobject.h"
class Chatacter :public GameObject
{
public:
	virtual void Init()override{}
	virtual void Update(const float& DeltaTime)override{}
	virtual void Draw()override{}
	virtual void Uninit()override{}
protected:
	XMFLOAT3 m_Velocity{};
	XMFLOAT4 m_Quaternion{};
};