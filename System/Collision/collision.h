#pragma once
#include "Main\main.h"
#include "System\Structure\shaderStructure.h"

class GameObject;

class Collision
{
public:
	Collision(GameObject* Owner, XMFLOAT3 Offset);
	virtual ~Collision() {}
	virtual bool IsCollisionOverlapping(const Collision* Collision)const = 0;
	virtual bool IsCollisionHit(const Collision* Collision)const = 0;
	virtual void UpdateCollision() {}
	virtual void Init() {}
	virtual void Draw() {}
	virtual void Uninit() {}
	const XMFLOAT3& GetOffset() { return m_Offset; }
	const XMFLOAT3& GetScale() { return m_Scale; }
	const XMFLOAT3& GetRotation() { return m_Rotation; }
	bool GetEnable() const { return m_IsEnable; }
	void SetEnable(const bool Flag) { m_IsEnable = Flag; }
protected:
	GameObject* m_Owner{};
	Shader* m_Shader{};
	XMFLOAT3 m_Position{};
	XMFLOAT3 m_Rotation{};
	XMFLOAT3 m_Offset{};
	XMFLOAT3 m_Scale = {1.0f,1.0f,1.0f};
	ID3D11Buffer* m_VertexBuffer{};
	ID3D11Buffer* m_IndexBuffer{};
	bool m_IsEnable = true;
};