#pragma once
#include <string>
class Player;
class Camera;
class AnimationModel;
class PlayerState
{
protected:
	Player* m_Player;
	Camera* m_Camera;
	AnimationModel* m_AnimationModel{};
	std::string m_AnimationName = "";
public:
	PlayerState(Player* player, Camera* camera,AnimationModel* model,const std::string& AnimationName) : m_Player(player), m_Camera(camera),m_AnimationModel(model),m_AnimationName(AnimationName) {}
	virtual ~PlayerState() = default;
	virtual void Init() {}
	virtual void Update() {}
	virtual void Uninit() {}
	virtual void Draw() {}
	virtual void UserInputDection() {}
	const Player* GetPlayer()const { return m_Player; }

};