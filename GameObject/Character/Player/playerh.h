#pragma once
#include "GameObject/Character/character.h"
#include "Enum/playerStateEnum.h"
#include <unordered_map>
class PlayerState;
class Camera;
class Player : public Character
{
private:

	//ID3D11VertexShader* m_VertexShader{};
	//ID3D11PixelShader* m_PixelShader{};
	//ID3D11InputLayout* m_VertexLayout{};
	std::unordered_map<PLAYER_STATE, PlayerState*> m_PlayerState;
	PlayerState* m_CurrentState;
	//std::string		m_AnimationName{};
	//std::string		m_NextAnimationName{};
	class Audio* m_SE;
	class SpriteEmitter* m_spriteEmitter;
	int		m_AnimationFrame = 0;
	float m_BlendRatio{};
	Camera* m_Camera;
public:
	virtual void Init()override;
	virtual void Uninit()override;
	virtual void Update(const float& DeltaTime)override;
	virtual void Draw()override;
	void ChangeState(PLAYER_STATE State);
};