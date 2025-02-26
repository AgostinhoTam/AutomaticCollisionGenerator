#include "StateMachine\PlayerState\playerStateJump.h"
#include "Manager\inputManager.h"
PlayerStateJump::PlayerStateJump(Player* player, Camera* camera, const std::string& AnimationName):PlayerState(player, camera, AnimationName)
{
}
void PlayerStateJump::Init()
{

}

void PlayerStateJump::Update()
{
}

void PlayerStateJump::UserInputDection()
{
	if (InputManager::GetKeyPress(VK_SPACE))
	{
		
	}
}
