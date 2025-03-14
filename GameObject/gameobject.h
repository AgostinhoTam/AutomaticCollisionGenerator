#pragma once
#include "Main/main.h"
#include "System\Enum/gameObjectEnum.h"
#include "System\Structure/shaderStructure.h"
#include <string>
class Collision;
class GameObject 
{
protected:
	std::string m_Name;
	XMFLOAT3 m_Position = {0.0f,0.0f,0.0f};
	XMFLOAT3 m_Scale = { 1.0f,1.0f,1.0f };
	XMFLOAT3 m_Rotation = { 0.0f,0.0f,0.0f };
	GAMEOBJECT_TYPE m_ObjectType = GAMEOBJECT_TYPE::NONE;
	Shader* m_Shader{};
	ID3D11Buffer* m_VertexBuffer = NULL;
	ID3D11ShaderResourceView* m_Texture = NULL;
	bool	m_IsUsed = false;
public:
	GameObject(){}
	virtual ~GameObject(){}
	virtual void Init() = 0;
	virtual void Update(const float& DeltaTime) = 0;
	virtual void Draw() = 0;
	virtual void Uninit() = 0;
	const XMFLOAT3 GetForward()const;
	const XMFLOAT3 GetRight()const;
	const XMFLOAT3 GetUp()const;
	const XMFLOAT3& GetPosition() const{ return m_Position;}
	const XMFLOAT3& GetRotation() const{ return m_Rotation; }
	const XMFLOAT3& GetScale() const{ return m_Scale; }
	const GAMEOBJECT_TYPE& GetGameObjectType() const{ return m_ObjectType; }
	const Shader* GetShader()const { return m_Shader; }
	const std::string GetName() const{ return m_Name; }
	XMFLOAT3 XMQuaternionToEulerAngle(XMVECTOR Quat);
	void	SetRotation(const XMFLOAT3& Rotation) { m_Rotation = Rotation; }
	void	SetRotationY(const float Rotation) { m_Rotation.y = Rotation; }
	void	SetPosition(const XMFLOAT3& Position) { m_Position = Position; }
};