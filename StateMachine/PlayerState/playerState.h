#pragma once
class Player;
class Camera;
class AnimationModel;
class PlayerState
{
protected:
	Player* m_Player;
	Camera* m_Camera;
	AnimationModel* m_AnimationModel{};
public:
	PlayerState(Player* player, Camera* camera,AnimationModel* model) : m_Player(player), m_Camera(camera),m_AnimationModel(model) {}
	virtual ~PlayerState() = default;
	virtual void Init() {}
	virtual void Update() {}
	virtual void Uninit() {}
	virtual void Draw() {}
	virtual void UserInputDection() {}
	virtual void UpdateAnimation(){}
	const Player* GetPlayer()const { return m_Player; }

};