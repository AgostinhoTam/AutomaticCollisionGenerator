#pragma once
#include "Main\main.h"
#include "System\Structure\shaderStructure.h"
struct LINE_VERTEX
{
	XMFLOAT3 Position;
	XMFLOAT4 Color;
};
class GameObject;

class Collision
{
protected:
	Shader* m_Shader{};
	XMFLOAT3 m_Position{};
	XMFLOAT3 m_Rotation{};
	XMFLOAT3 m_Offset{};
	XMFLOAT3 m_Scale = {1.0f,1.0f,1.0f};
	ID3D11Buffer* m_VertexBuffer{};
	ID3D11Buffer* m_IndexBuffer{};
	bool m_IsEnable = true;
	bool m_IsHit = false;
	bool m_IsSelected = false;
public:
	Collision(const XMFLOAT3& Position, const XMFLOAT3& Offset);	// 引数（Ownerポインタ、Offset値）
	virtual ~Collision() {}
	virtual bool IsCollisionOverlapping(const Collision* Collision) = 0;
	virtual void UpdateCollision(const XMFLOAT3& Position) {}
	virtual void Init() {}
	virtual void Draw() {}
	virtual void Uninit() {}
	const XMFLOAT3& GetOffset() { return m_Offset; }
	const XMFLOAT3& GetScale() { return m_Scale; }
	const XMFLOAT3& GetRotation() { return m_Rotation; }
	const XMFLOAT3& GetPosition() { return m_Position; }
	bool GetEnable() const { return m_IsEnable; }
	bool GetIsHit()const { return m_IsHit; }
	bool GetIsSelected()const { return m_IsSelected; }
	void SetOffset(const XMFLOAT3& Offset) { m_Offset = Offset; }
	void SetEnable(const bool Flag) { m_IsEnable = Flag; }
	void SetIsHit(const bool Flag) { m_IsHit = Flag; }
	void SetIsSelected(const bool Flag) { m_IsSelected = Flag; }
	void ResetAllCollisionFlag();
};