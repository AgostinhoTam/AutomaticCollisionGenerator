#pragma once
#include "Main/main.h"
#include "Enum/gameObjectEnum.h"
#include "Structure/shaderStructure.h"
class GameObject 
{
public:
	GameObject(){}
	virtual ~GameObject(){}
	virtual void Init() = 0;
	virtual void Update(const float& DeltaTime) = 0;
	virtual void Draw() = 0;
	virtual void Uninit() = 0;
	XMFLOAT3 GetForward()const;
	XMFLOAT3 GetRight()const;
	XMFLOAT3 GetUp()const;
	const XMFLOAT3& GetPosition() const{ return m_Position;}
	const XMFLOAT3& GetRotation() const{ return m_Rotation; }
	const XMFLOAT3& GetScale() const{ return m_Scale; }
	const XMFLOAT4& GetQuaternion() const{ return m_Quaternion; }
	const GAMEOBJECT_TYPE& GetGameObjectType() const{ return m_ObjectType; }
	const Shader* GetShader()const { return m_Shader; }
protected:
	XMFLOAT3 m_Position = {0.0f,0.0f,0.0f};
	XMFLOAT3 m_Scale = { 1.0f,1.0f,1.0f };
	XMFLOAT3 m_Rotation = { 0.0f,0.0f,0.0f };

	XMFLOAT4 m_Quaternion{};
	GAMEOBJECT_TYPE m_ObjectType = GAMEOBJECT_TYPE::NONE;
	Shader* m_Shader{};
	ID3D11Buffer* m_VertexBuffer = NULL;
	ID3D11ShaderResourceView* m_Texture = NULL;
};