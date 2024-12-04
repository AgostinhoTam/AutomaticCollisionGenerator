#pragma once
class Player;
class Camera;
class PlayerState
{
protected:
	Player* m_Player;
	Camera* m_Camera;
public:
	PlayerState(Player* player, Camera* camera) : m_Player(player), m_Camera(camera) {}
	virtual ~PlayerState() = default;
	virtual void Init() {}
	virtual void Update() {}
	virtual void Uninit() {}
	virtual void Draw() {}
	virtual void UserInputDection() {}
	const Player* GetPlayer()const { return m_Player; }
};