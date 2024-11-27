#pragma once
#include "StateMachine/PlayerState/playerState.h"




class Player;
class PlayerStateWalk :public PlayerState
{
public:
	PlayerStateWalk(Player* player) :PlayerState(player){}
	virtual void Init() override;
	virtual void Update() override;
	virtual void Uninit() override{}
	virtual void Draw() override{}
	virtual void UserInputDection() override;
};