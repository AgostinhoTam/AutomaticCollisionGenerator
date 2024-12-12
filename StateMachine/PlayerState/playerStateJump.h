#pragma once
#include "StateMachine\PlayerState\playerState.h"
class PlayerStateJump :public PlayerState
{
public:
	PlayerStateJump(Player* player, Camera* camera,AnimationModel* model) :PlayerState(player, camera,model) {}
	virtual void Init() override;
	virtual void Update() override;
	virtual void Uninit() override {}
	virtual void Draw() override {}
	virtual void UserInputDection() override;

};

