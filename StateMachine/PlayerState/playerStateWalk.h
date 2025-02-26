#pragma once
#include "StateMachine/PlayerState/playerState.h"

class PlayerStateWalk :public PlayerState
{
public:
	PlayerStateWalk(Player* player, Camera* camera, const std::string& AnimationName);
	virtual void Init() override;
	virtual void Update() override;
	virtual void Uninit() override{}
	virtual void Draw() override{}
	virtual void UserInputDection() override;

};