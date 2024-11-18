#pragma once
#include "Scene/scene.h"

class GameScene :public Scene
{
public:
	virtual void Init()override;
	virtual void Update(const float& DeltaTime)override;
	virtual void Draw()override;
	virtual void Uninit()override;
};