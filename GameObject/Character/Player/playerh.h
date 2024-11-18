#pragma once
#include "GameObject/gameobject.h"

class Player : public GameObject
{
private:

	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader* m_PixelShader{};
	ID3D11InputLayout* m_VertexLayout{};

	XMFLOAT3 m_Velocity{};
	XMFLOAT4 m_Quaternion{};

	//std::string		m_AnimationName{};
	//std::string		m_NextAnimationName{};
	class Audio* m_SE;
	class SpriteEmitter* m_spriteEmitter;
	int		m_AnimationFrame = 0;
	float m_BlendRatio{};
public:
	void Init();
	void Uninit();
	void Update();
	void Draw();

};