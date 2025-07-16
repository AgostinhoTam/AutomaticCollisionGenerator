/*===================================================================================

球体コリジョン処理(SphereCollision.cpp)

====================================================================================*/
#include <vector>
#include "GameObject/gameobject.h"
#include "System/Renderer/renderer.h"
#include "System/Collision/capsuleCollision.h"
#include "Manager/sceneManager.h"
#include "sphereCollision.h"

constexpr int DEBUG_LINE_SEGMENTS = 32;	//デバッグ用の線の分割数

//	===================球体コリジョン初期化======================
//	Position	:	XMFLOAT3	位置
//	Offset		:	XMFLOAT3	オフセット値
//	Radius		:	float		球体半径
SphereCollision::SphereCollision(const std::string& CollisionName,const XMFLOAT3& Position, const XMFLOAT3& Offset, float Radius) :Collision(CollisionName,Position, Offset), m_Radius(Radius)
{
	Init();
}

//	===================当たり判定処理======================
//	Collision	:	Collision*	対象物のコリジョンポインタ
bool SphereCollision::IsCollisionOverlapping(const Collision* Collision) 
{
	if (!Collision)return false;
	//	相手のコリジョンに応じて当たり判定取る方法を選択
	const SphereCollision* sphere = dynamic_cast<const SphereCollision*>(Collision);
	if (sphere)
	{
		return IsCollisionOverlapping(sphere);
	}
	const CapsuleCollision* capsule = dynamic_cast<const CapsuleCollision*>(Collision);
	if (capsule)
	{
		return IsCollisionOverlapping(capsule);
	}
	return false;
}

//	===================球体と球体の場合======================
//	Collision	:	SphereCollision*	相手の球体コリジョン
bool SphereCollision::CheckSphereToSphere(const SphereCollision* Collision) 
{
	if (!Collision)return false;
	XMVECTOR ownerPosition = XMLoadFloat3(&m_Position);
	XMVECTOR otherPosition = XMLoadFloat3(&Collision->m_Position);
	float radiusSum = m_Radius + Collision->m_Radius;

	XMVECTOR positionSubtract = XMVectorSubtract(ownerPosition, otherPosition);
	float distance = XMVectorGetX(XMVector3LengthSq(positionSubtract));

	return (distance <= (radiusSum * radiusSum));

}

//	===================球体コリジョン更新======================
//	Position	:	XMFLOAT3	位置
void SphereCollision::UpdateCollision(const XMFLOAT3& Position)
{
	m_Position = { Position.x + m_Offset.x,Position.y + m_Offset.y, Position.z + m_Offset.z };
}

//	===================球体コリジョン初期化======================
void SphereCollision::Init()
{
	UpdateCollision(m_Position);
	CreateLineVertex(m_SphereLineVertices);
}

//	===================球体コリジョン描画======================
void SphereCollision::Draw()
{
	//	マトリクス設定
	XMMATRIX world, scale, rot, trans;

	scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	quaternion = XMQuaternionNormalize(quaternion);
	rot = XMMatrixRotationQuaternion(quaternion);

	trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	world = scale * rot * trans;
	Renderer::SetWorldMatrix(world);


	Renderer::GetDeviceContext()->IASetInputLayout(m_Shader->m_VertexLayout);


	Renderer::GetDeviceContext()->VSSetShader(m_Shader->m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_Shader->m_PixelShader, NULL, 0);

	// 頂点バッファ設定
	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);


	Renderer::GetDeviceContext()->Draw(static_cast<UINT>(m_SphereLineVertices.size()), 0);

}

//	===================デバッグの線の頂点情報保存======================
void SphereCollision::CreateLineVertex(std::vector<XMFLOAT3>& SphereLineVertices)
{
	SphereLineVertices.clear();

	SphereLineVertices.emplace_back(XMFLOAT3(0, 0, 0));
	//	XZ平面
	for (int i = 0; i < DEBUG_LINE_SEGMENTS; ++i)
	{
		float angle = XM_2PI * i / DEBUG_LINE_SEGMENTS;
		float x = m_Radius * cosf(angle);
		float z = m_Radius * sinf(angle);
		SphereLineVertices.emplace_back(XMFLOAT3{ x,0.0f, z });
		if (i == DEBUG_LINE_SEGMENTS - 1)
		{
			SphereLineVertices.emplace_back(SphereLineVertices[SphereLineVertices.size() - DEBUG_LINE_SEGMENTS]);
		}
	}

	SphereLineVertices.emplace_back(XMFLOAT3(0, 0, 0));
	//	YZ平面
	for (int i = 0; i < DEBUG_LINE_SEGMENTS; ++i)
	{
		float angle = XM_2PI * i / DEBUG_LINE_SEGMENTS;
		float z = m_Radius * cosf(angle);
		float y = m_Radius * sinf(angle);
		SphereLineVertices.emplace_back(XMFLOAT3{ 0.0f,y,z });
		if (i == DEBUG_LINE_SEGMENTS - 1)
		{
			SphereLineVertices.emplace_back(SphereLineVertices[SphereLineVertices.size() - DEBUG_LINE_SEGMENTS]);
		}

	}
	SphereLineVertices.emplace_back(XMFLOAT3(0, 0, 0));
	//	XY平面
	for (int i = 0; i < DEBUG_LINE_SEGMENTS; ++i)
	{
		float angle = XM_2PI * i / DEBUG_LINE_SEGMENTS;
		float y = m_Radius * cosf(angle);
		float x = m_Radius * sinf(angle);
		SphereLineVertices.emplace_back(XMFLOAT3{ x,y,0.0f });
		if (i == DEBUG_LINE_SEGMENTS - 1)
		{
			SphereLineVertices.emplace_back(SphereLineVertices[SphereLineVertices.size() - DEBUG_LINE_SEGMENTS]);
		}
	}

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(XMFLOAT3) * SphereLineVertices.size());
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = SphereLineVertices.data();

	Renderer::GetDevice()->CreateBuffer(&bufferDesc, &sd, &m_VertexBuffer);
}
