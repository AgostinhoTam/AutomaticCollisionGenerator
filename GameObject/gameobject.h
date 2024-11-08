#pragma once
#include "Main/main.h"
#include "Enum/gameObjectEnum.h"
class GameObject 
{
public:
	GameObject(const GAMEOBJECT_TYPE& type):m_ObjectType(type){}
	virtual ~GameObject(){}
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Uninit() = 0;
	XMFLOAT3 GetForward()const;
	XMFLOAT3 GetRight()const;
	XMFLOAT3 GetUp()const;
	const GAMEOBJECT_TYPE& GetGameObjectType() const{ return m_ObjectType; }
protected:
	XMFLOAT3 m_Position = {0.0f,0.0f,0.0f};
	XMFLOAT3 m_Scale = { 0.0f,0.0f,0.0f };
	XMFLOAT3 m_Rotation = { 0.0f,0.0f,0.0f };
	GAMEOBJECT_TYPE m_ObjectType = GAMEOBJECT_TYPE::NONE;

	ID3D11Buffer* m_VertexBuffer = NULL;
	ID3D11ShaderResourceView* m_Texture = NULL;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_VertexLayout;
};