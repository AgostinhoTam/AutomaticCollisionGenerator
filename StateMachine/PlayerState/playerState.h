#pragma once
class Player;
class PlayerState
{
protected:
	Player* m_Player;
public:
	PlayerState(Player* player) :m_Player(player) {}
	virtual ~PlayerState() = default;
	virtual void Init() {}
	virtual void Update() {}
	virtual void Uninit() {}
	virtual void Draw() {}
	virtual void UserInputDection() {}
	const Player* GetPlayer()const { return m_Player; }
};