#include <vector>
#include "GameObject\gameobject.h"
#include "Manager\shaderManager.h"
#include "System\Renderer\renderer.h"
#include "System\Collision\sphereCollision.h"
#include "System\Collision\characterBoneCollision.h"
#include "sphereCollision.h"
#include <algorithm>

constexpr int DEBUG_LINE_SEGMENTS = 18;	//デバッグ用の線の分割数
constexpr int DEBUG_LINE_CONNECTION = 4;
constexpr XMFLOAT4 DEBUG_LINE_COLOR = XMFLOAT4(0.1f, 1.0f, 0.1f, 1.0f);
constexpr XMFLOAT4 DEBUG_HITTED_LINE_COLOR = XMFLOAT4(1.0f, 0.1f, 0.1f, 1.0f);
constexpr XMFLOAT4 DEBUG_SELECTED_LINE_COLOR = XMFLOAT4(0.5f, 0.5f, 0.1f, 1.0f);

CharacterBoneCollision::CharacterBoneCollision(const int HeadBoneIndex, const int TailBoneIndex, const XMFLOAT3& Start, const XMFLOAT3& End, const XMFLOAT3& Offset, float Radius) :Collision(Start, Offset), m_Radius(Radius),m_StartPosition(Start),m_EndPosition(End),m_HeadBoneIndex(HeadBoneIndex),m_TailBoneIndex(TailBoneIndex)
{
	Init();
}

bool CharacterBoneCollision::IsCollisionOverlapping(const Collision* Collision) 
{
	if (!Collision)return false;
	const CharacterBoneCollision* bone = dynamic_cast<const CharacterBoneCollision*>(Collision);
	if (bone)
	{
		return CheckCapsuleToCapsule(bone);
	}
	return false;
}

bool CharacterBoneCollision::CheckCapsuleToCapsule(const CharacterBoneCollision* Collision)
{
	if (!Collision) return false;
	XMVECTOR S1 = XMLoadFloat3(&m_StartPosition);
	XMVECTOR E1 = XMLoadFloat3(&m_EndPosition);
	XMVECTOR S2 = XMLoadFloat3(&Collision->m_StartPosition);
	XMVECTOR E2 = XMLoadFloat3(&Collision->m_EndPosition);

	float shortestDistance = CheckDistanceSegmentToSegment(S1, E1, S2, E2);
	float radiusSum = m_Radius + Collision->m_Radius;
	return shortestDistance <= radiusSum;
}

float CharacterBoneCollision::CheckDistanceSegmentToSegment(const XMVECTOR& Start1, const XMVECTOR& End1, const XMVECTOR& Start2, const XMVECTOR& End2)
{
	//	最短距離公式	s = S1 + s(E1-S1)   t = S2 + t(E2-S2)

	//	線分の方向ベクトルを計算
	XMVECTOR vSE1 = XMVectorSubtract(End1, Start1);
	XMVECTOR vSE2 = XMVectorSubtract(End2, Start2);
	XMVECTOR vSS = XMVectorSubtract(Start1, Start2);

	//	各ベクトルの長さと内積を求める
	float lengthSE1 = XMVectorGetX(XMVector3Dot(vSE1, vSE1));	
	float dSE12 = XMVectorGetX(XMVector3Dot(vSE1, vSE2));
	float lengthSE2 = XMVectorGetX(XMVector3Dot(vSE2, vSE2));
	float dSS1 = XMVectorGetX(XMVector3Dot(vSE1, vSS));
	float eSS2 = XMVectorGetX(XMVector3Dot(vSE2, vSS));

	float denom = lengthSE1 * lengthSE2 - dSE12 * dSE12;
	float s, t;

	//	0に近い
	if (denom < 1e-6f)
	{
		s = 0.0f;
		t = dSS1 / dSE12;
	}
	else
	{
		s = (dSE12 * eSS2 - lengthSE2 * dSS1) / denom;
		t = (lengthSE1 * eSS2 - dSE12 * dSS1) / denom;
	}

	//	範囲外にならないように
	s = std::clamp(s, 0.0f, 1.0f);
	t = std::clamp(t, 0.0f, 1.0f);

	XMVECTOR closest1 = XMVectorAdd(Start1, XMVectorScale(vSE1, s));
	XMVECTOR closest2 = XMVectorAdd(Start2, XMVectorScale(vSE2, t));

	XMVECTOR diff = XMVectorSubtract(closest1, closest2);
	return XMVectorGetX(XMVector3Length(diff));
}

bool CharacterBoneCollision::CheckSphereToSphere(const SphereCollision* Collision) 
{
	if (!Collision)return false;
	return false;
}

void CharacterBoneCollision::UpdateCollision(const XMFLOAT3& Position)
{
}

void CharacterBoneCollision::UpdateBonePosition(const int FirstIndex, const int SecondIndex, const XMFLOAT3& HeadPos,const XMFLOAT3& TailPos)
{
	//	コリジョンの位置を変える
	XMVECTOR headPos = XMLoadFloat3(&HeadPos);
	XMVECTOR tailPos = XMLoadFloat3(&TailPos);
	XMVECTOR offset = XMLoadFloat3(&m_Offset);

	XMVECTOR startPos = XMVectorAdd(headPos, offset);
	XMVECTOR endPos = XMVectorAdd(tailPos, offset);

	if (m_HeadBoneIndex == FirstIndex)
	{
		XMStoreFloat3(&m_StartPosition,startPos);
	}
	if (m_TailBoneIndex == SecondIndex)
	{
		XMStoreFloat3(&m_EndPosition, endPos);
	}
}
void CharacterBoneCollision::Init()
{
	m_Shader = ShaderManager::LoadShader(SHADER_NAME::DEBUG_LINE);
	CreateCylinderLine(XMFLOAT4(1.0f,1.0f,1.0f,1.0f), m_CylinderLineVertices);
	CreateSphereLine(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),m_StartSphereVertices);
	CreateSphereLine(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),m_EndSphereVertices);
	CreateBufferVertices(m_CylinderLineVertices, m_CylinderBuffer);
	CreateBufferVertices(m_StartSphereVertices, m_StartSphereBuffer);
	CreateBufferVertices(m_EndSphereVertices, m_EndSphereBuffer);
}

void CharacterBoneCollision::Draw()
{

	Renderer::GetDeviceContext()->IASetInputLayout(m_Shader->m_VertexLayout);

	Renderer::GetDeviceContext()->VSSetShader(m_Shader->m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_Shader->m_PixelShader, NULL, 0);

	XMFLOAT4 color;
	//	描画用色
	if (m_IsHit)
	{
		color = DEBUG_HITTED_LINE_COLOR;
	}
	else
	{
		color = DEBUG_LINE_COLOR;
	}
	if (m_IsSelected && !m_IsHit)
	{
		color = DEBUG_SELECTED_LINE_COLOR;
	}

	Renderer::SetColor(color);

	// プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// 頂点バッファ設定
	UINT stride = sizeof(LINE_VERTEX);
	UINT offset = 0;
	MakeSphereMatrix(m_StartPosition);
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_StartSphereBuffer, &stride, &offset);
	Renderer::GetDeviceContext()->Draw(static_cast<UINT>(m_StartSphereVertices.size()), 0);
	MakeSphereMatrix(m_EndPosition);
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_EndSphereBuffer, &stride, &offset);
	Renderer::GetDeviceContext()->Draw(static_cast<UINT>(m_EndSphereVertices.size()), 0);
	MakeCapsuleMatrix(m_StartPosition, m_EndPosition);
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_CylinderBuffer, &stride, &offset);
	Renderer::GetDeviceContext()->Draw(static_cast<UINT>(m_CylinderLineVertices.size()), 0);

}

//	球体の描画
void CharacterBoneCollision::CreateSphereLine(const XMFLOAT4& Color, std::vector<LINE_VERTEX>& SphereVertices)
{
	XMFLOAT4 color = Color;

	// XZ面
	{
		XMFLOAT3 prev;
		bool first = true;

		for (int i = 0; i < DEBUG_LINE_SEGMENTS; ++i)
		{
			float angle = (XM_2PI * i) / static_cast<float>(DEBUG_LINE_SEGMENTS);
			float x =cosf(angle);
			float z =sinf(angle);
			XMFLOAT3 current = XMFLOAT3(x, 0, z);
			if (!first)
			{
				SphereVertices.emplace_back(LINE_VERTEX{ prev,color });
				SphereVertices.emplace_back(LINE_VERTEX{ current,color });
			}
			else
			{
				first = false;
			}
			prev = current;
		}
	}
	// YZ面
	{
		XMFLOAT3 prev;
		bool first = true;
		for (int i = 0; i <= DEBUG_LINE_SEGMENTS; i++)
		{
			float angle = (XM_2PI * i) / static_cast<float>(DEBUG_LINE_SEGMENTS);
			float y = cosf(angle);
			float z = sinf(angle);
			XMFLOAT3 current = XMFLOAT3(0, y,  z);

			if (!first)
			{
				SphereVertices.emplace_back(LINE_VERTEX{ prev,color });
				SphereVertices.emplace_back(LINE_VERTEX{ current,color });
			}
			else
			{
				first = false;
			}
			prev = current;
		}
	}
	
	//	XY面
	{
		XMFLOAT3 prev;
		bool first = true;
		for (int i = 0; i <= DEBUG_LINE_SEGMENTS; i++)
		{
			float angle = (XM_2PI * i) / static_cast<float>(DEBUG_LINE_SEGMENTS);
			float x = cosf(angle);
			float y = sinf(angle);
			XMFLOAT3 current = XMFLOAT3(x, y, 0);

			if (!first)
			{
				SphereVertices.emplace_back(LINE_VERTEX{ prev,color });
				SphereVertices.emplace_back(LINE_VERTEX{ current,color });
			}
			else
			{
				first = false;
			}
			prev = current;
		}
	}
}


//	円柱の描画
void CharacterBoneCollision::CreateCylinderLine(const XMFLOAT4& Color, std::vector<LINE_VERTEX>& CylinderVertices)
{

	//	下の円
	{
		XMFLOAT3 prev;
		bool first = true;
		for (int i = 0; i < DEBUG_LINE_SEGMENTS; ++i)
		{
			float angle = (XM_2PI * i) / static_cast<float>(DEBUG_LINE_SEGMENTS);

			float x = cosf(angle);
			float y = sinf(angle);

			XMFLOAT3 current = XMFLOAT3(x,y,0);

			if (!first)
			{
				CylinderVertices.emplace_back(LINE_VERTEX{ prev,Color });
				CylinderVertices.emplace_back(LINE_VERTEX{ current,Color });
			}
			else
			{
				first = false;
			}
			prev = current;
		}
		
	}

	//	上の円
	{
		XMFLOAT3 prev;
		bool first = true;
		for (int i = 0; i <= DEBUG_LINE_SEGMENTS; i++)
		{
			float angle = (XM_2PI * i) / static_cast<float>(DEBUG_LINE_SEGMENTS);
			float x = cosf(angle);
			float y = sinf(angle);

			XMFLOAT3 current = XMFLOAT3(x,y,1);

			if (!first)
			{
				CylinderVertices.emplace_back(LINE_VERTEX{ prev,Color });
				CylinderVertices.emplace_back(LINE_VERTEX{ current,Color });
			}
			else
			{
				first = false;
			}
			prev = current;
		}
	}
	
	// 繋ぐ線（側面）
	for (int i = 0; i < DEBUG_LINE_SEGMENTS; ++i)
	{
		float angle = XM_2PI * static_cast<float>(i) / static_cast<float>(DEBUG_LINE_SEGMENTS);
		float x = cosf(angle);
		float y = sinf(angle);

		XMFLOAT3 bottom = XMFLOAT3( x, y, 0);
		XMFLOAT3 top = XMFLOAT3(x, y, 1);

		CylinderVertices.emplace_back(LINE_VERTEX{ bottom, Color });
		CylinderVertices.emplace_back(LINE_VERTEX{ top, Color });
	}
}

void CharacterBoneCollision::CreateBufferVertices(const std::vector<LINE_VERTEX>& Vertices, ID3D11Buffer*& Buffer)
{
	ID3D11Buffer* vertexBuffer = nullptr;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(LINE_VERTEX) * Vertices.size());
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = Vertices.data();

	Renderer::GetDevice()->CreateBuffer(&bufferDesc, &sd, &Buffer);
}

void CharacterBoneCollision::MakeSphereMatrix(const XMFLOAT3& Position)
{
	XMMATRIX world, scale,trans;
	scale = XMMatrixScaling(m_Radius, m_Radius, m_Radius);

	trans = XMMatrixTranslation(Position.x, Position.y, Position.z);
	world = scale * trans;
	Renderer::SetWorldMatrix(world);
}

void CharacterBoneCollision::MakeCapsuleMatrix(const XMFLOAT3& StartPos, const XMFLOAT3& EndPos)
{
	XMVECTOR startVec = XMLoadFloat3(&StartPos);
	XMVECTOR endVec = XMLoadFloat3(&EndPos);
	XMVECTOR axis = XMVectorSubtract(endVec, startVec);
	float length = XMVectorGetX(XMVector3Length(axis)); //円柱の高さ

	XMMATRIX scale = XMMatrixScaling(m_Radius, m_Radius, length);

	XMFLOAT3 up(0, 0, 1);
	XMVECTOR upVec = XMLoadFloat3(&up);
	XMVECTOR axisNormalize = XMVector3Normalize(axis);
	XMVECTOR cross = XMVector3Cross(upVec, axisNormalize);
	float sinTheta = XMVectorGetX(XMVector3Length(cross));
	float dot = XMVectorGetX(XMVector3Dot(upVec, axisNormalize));
	float angle = acosf(dot);


	XMMATRIX rot = XMMatrixIdentity();
	// 軸が完全に平行/逆向き でなければ回転
	if (fabsf(sinTheta) > 1e-4f)
	{
		cross = XMVector3Normalize(cross);
		rot = XMMatrixRotationAxis(cross, angle);
	}

	// 4) 平行移動
	XMMATRIX trans = XMMatrixTranslation(StartPos.x, StartPos.y, StartPos.z);
	XMMATRIX world = scale * rot * trans;
	Renderer::SetWorldMatrix(world);
}