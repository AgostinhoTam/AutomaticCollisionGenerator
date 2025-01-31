#pragma once
#include "StateMachine/PlayerState/playerState.h"
class PlayerStateIdle :public PlayerState
{
public:
	PlayerStateIdle(Player* player, Camera* camera,AnimationModel* model) :PlayerState(player, camera,model) {}
	virtual void Init() override;
	virtual void Update() override;
	virtual void Uninit() override {}
	virtual void Draw() override {}
	virtual void UserInputDection() override;
};