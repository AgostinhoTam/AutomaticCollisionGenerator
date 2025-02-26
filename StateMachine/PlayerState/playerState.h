#pragma once
#include <string>
class Player;
class Camera;
class Animator;
class PlayerState
{
protected:
	Player* m_Player;
	Camera* m_Camera;
	std::string m_AnimationName;
	Animator* m_Animator;
public:
	PlayerState(Player* Player, Camera* Camera,const std::string AnimationName) : m_Player(Player), m_Camera(Camera), m_AnimationName(AnimationName) {}
	virtual ~PlayerState() = default;
	virtual void Init() {}
	virtual void Update() {}
	virtual void Uninit() {}
	virtual void Draw() {}
	virtual void UserInputDection() {}
	const Player* GetPlayer()const { return m_Player; }

};