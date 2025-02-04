#include <vector>
#include "GameObject\gameobject.h"
#include "System\Renderer\renderer.h"
#include "System\Collision\capsuleCollision.h"
#include "Manager\sceneManager.h"
#include "sphereCollision.h"

constexpr int DEBUG_LINE_SEGMENTS = 32;	//デバッグ用の線の分割数



SphereCollision::SphereCollision(const XMFLOAT3& Position, const XMFLOAT3& Offset, float Radius) :Collision(Position, Offset), m_Radius(Radius) // 引数（Ownerポインタ、Offset値、半径）
{

	Init();
}

bool SphereCollision::IsCollisionOverlapping(const Collision* Collision) 
{
	if (!Collision)return false;
	const SphereCollision* sphere = dynamic_cast<const SphereCollision*>(Collision);
	if (sphere)
	{
		return IsCollisionOverlapping(sphere);
	}
	const CapsuleCollision* capsule = dynamic_cast<const CapsuleCollision*>(Collision);
	if (capsule)
	{
		// TODO Capsule変更
		return IsCollisionOverlapping(capsule);
	}
	return false;
}

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

void SphereCollision::UpdateCollision(const XMFLOAT3& Position)
{
	m_Position = { Position.x + m_Offset.x,Position.y + m_Offset.y, Position.z + m_Offset.z };
}

void SphereCollision::Init()
{
	UpdateCollision(m_Position);
	if (SceneManager::GetInstance()->GetIsDebugMode())
	{
		CreateLineVertex(m_SphereLineVertices);
	}
}

void SphereCollision::Draw()
{
	if (!SceneManager::GetInstance()->GetIsDebugMode())return;
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

//	デバッグの線の頂点情報保存
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