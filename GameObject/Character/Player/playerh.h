#pragma once
#include "GameObject/Character/character.h"
#include <unordered_map>
#include <vector>
class PlayerState;
class Camera;
class Enemy;
class Collision;
enum class PLAYER_STATE;
class Player : public Character
{
private:
	Collision* m_Collision;
	std::unordered_map<PLAYER_STATE, PlayerState*> m_PlayerState;
	PlayerState* m_CurrentState;
	Camera* m_Camera;
	std::vector<Enemy*> m_EnemyList;
	bool m_IsHardLock = false;
public:
	virtual void Init()override;
	virtual void Uninit()override;
	virtual void Update(const float& DeltaTime)override;
	virtual void Draw()override;
	void ChangeState(PLAYER_STATE State);
	void UpdatePlayerRotation();
	Enemy* LockTarget();
};