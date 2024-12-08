#pragma once
#include "StateMachine\PlayerState\playerState.h"
class PlayerStateJump :public PlayerState
{
public:
	PlayerStateJump(Player* player, Camera* camera) :PlayerState(player, camera) {}
	virtual void Init() override;
	virtual void Update() override;
	virtual void Uninit() override {}
	virtual void Draw() override {}
	virtual void UserInputDection() override;
	void UpdatePlayerRotation();
};

