#pragma once
#include "StateMachine/PlayerState/playerState.h"
class PlayerStateIdle :public PlayerState
{
public:
	PlayerStateIdle(Player* player, Camera* camera,AnimationModel* model,const std::string& AnimationName) :PlayerState(player, camera,model, AnimationName) {}
	virtual void Init() override;
	virtual void Update() override;
	virtual void Uninit() override {}
	virtual void Draw() override {}
	virtual void UserInputDection() override;
};