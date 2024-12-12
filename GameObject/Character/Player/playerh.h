#pragma once
#include "GameObject/Character/character.h"
#include <unordered_map>
class PlayerState;
class Camera;
enum class PLAYER_STATE;
class Player : public Character
{
private:

	std::unordered_map<PLAYER_STATE, PlayerState*> m_PlayerState;
	PlayerState* m_CurrentState;
	Camera* m_Camera;
public:
	virtual void Init()override;
	virtual void Uninit()override;
	virtual void Update(const float& DeltaTime)override;
	virtual void Draw()override;
	void ChangeState(PLAYER_STATE State);
	void UpdatePlayerRotation();
};