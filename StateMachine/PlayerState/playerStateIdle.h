#pragma once
#include "StateMachine/PlayerState/playerState.h"
class Player;
class Camera;
class PlayerStateIdle :public PlayerState
{
public:
	PlayerStateIdle(Player* player,Camera* camera) :PlayerState(player,camera) {}
	virtual void Init() override;
	virtual void Update() override;
	virtual void Uninit() override {}
	virtual void Draw() override {}
	virtual void UserInputDection() override;
};