#include <vector>
#include "GameObject\gameobject.h"
#include "Manager\shaderManager.h"
#include "System\Renderer\renderer.h"
#include "System\Collision\characterBoneCollision.h"
#include "sphereCollision.h"

#ifdef _DEBUG
constexpr int DEBUG_LINE_SEGMENTS = 18;	//デバッグ用の線の分割数
constexpr int DEBUG_LINE_CONNECTION = 4;
constexpr XMFLOAT4 DEBUG_LINE_COLOR = XMFLOAT4(0.1f, 1.0f, 0.1f, 1.0f);
#endif // _DEBUG


CharacterBoneCollision::CharacterBoneCollision(const std::string& HeadBoneName, const std::string& TailBoneName, const XMFLOAT3& Start, const XMFLOAT3& End, const XMFLOAT3& Offset, float Radius) :Collision(Start, Offset), m_Radius(Radius),m_StartPosition(Start),m_EndPosition(End),m_HeadBoneName(HeadBoneName),m_TailBoneName(TailBoneName) // 引数（Ownerポインタ、Offset値、半径）
{
	Init();
}

bool CharacterBoneCollision::IsCollisionOverlapping(const Collision* Collision) const
{
	if (!Collision)return false;
	const CharacterBoneCollision* sphere = dynamic_cast<const CharacterBoneCollision*>(Collision);
	if (sphere)
	{
		return IsCollisionOverlapping(sphere);
	}
	//const CapsuleCollision* capsule = dynamic_cast<const CapsuleCollision*>(Collision);
	//if (capsule)
	//{
	//	// TODO Capsule変更
	//	return IsCollisionOverlapping(capsule);
	//}
	return false;
}

bool CharacterBoneCollision::CheckSphereToSphere(const CharacterBoneCollision* Collision) const
{
	if (!Collision)return false;
	XMVECTOR ownerPosition = XMLoadFloat3(&m_Position);
	XMVECTOR otherPosition = XMLoadFloat3(&Collision->m_Position);
	float radiusSum = m_Radius + Collision->m_Radius;

	XMVECTOR positionSubtract = XMVectorSubtract(ownerPosition, otherPosition);
	float distance = XMVectorGetX(XMVector3LengthSq(positionSubtract));

	return (distance <= (radiusSum * radiusSum));

}

void CharacterBoneCollision::UpdateCollision(const XMFLOAT3& Position)
{
	m_StartPosition = { Position.x + m_Offset.x,Position.y + m_Offset.y, Position.z + m_Offset.z };
}

void CharacterBoneCollision::UpdateBonePosition(const std::string& BoneName,const XMFLOAT3& Position)
{
	if (m_HeadBoneName == BoneName)
	{
		m_StartPosition = Position;
	}
	else if (m_TailBoneName == BoneName)
	{
		m_EndPosition = Position;
	}
}

void CharacterBoneCollision::Init()
{
	m_Shader = ShaderManager::LoadShader(SHADER_NAME::DEBUG_LINE);
	CreateCylinderLine(DEBUG_LINE_COLOR,m_CylinderLineVertices);
	CreateSphereLine(DEBUG_LINE_COLOR,m_StartSphereVertices);
	CreateSphereLine( DEBUG_LINE_COLOR,m_EndSphereVertices);
	CreateBufferVertices(m_CylinderLineVertices, m_CylinderBuffer);
	CreateBufferVertices(m_StartSphereVertices, m_StartSphereBuffer);
	CreateBufferVertices(m_EndSphereVertices, m_EndSphereBuffer);
	//CreateCapsuleLineVertex();
}

void CharacterBoneCollision::Draw()
{
	//XMMATRIX world, scale, rot, trans;
	//const XMFLOAT3& objPosition = m_StartPosition;
	//const XMFLOAT3& objScale = m_Scale;
	//const XMFLOAT3& objRotation = m_Rotation;
	//const Shader* Shader = m_Shader;

	//scale = XMMatrixScaling(objScale.x, objScale.y, objScale.z);

	//XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(objRotation.x, objRotation.y, objRotation.z);
	//quaternion = XMQuaternionNormalize(quaternion);
	//rot = XMMatrixRotationQuaternion(quaternion);

	//trans = XMMatrixTranslation(objPosition.x, objPosition.y, objPosition.z);
	//world = scale * rot * trans;
	//Renderer::SetWorldMatrix(world);

	Renderer::GetDeviceContext()->IASetInputLayout(m_Shader->m_VertexLayout);

	Renderer::GetDeviceContext()->VSSetShader(m_Shader->m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_Shader->m_PixelShader, NULL, 0);

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

//	デバッグの線の頂点情報保存
void CharacterBoneCollision::CreateCapsuleLineVertex()
{

	std::vector<LINE_VERTEX> allVertices;

	allVertices.reserve(m_CylinderLineVertices.size() + m_StartSphereVertices.size() + m_EndSphereVertices.size());
	allVertices.insert(allVertices.end(), m_CylinderLineVertices.begin(), m_CylinderLineVertices.end());
	allVertices.insert(allVertices.end(), m_StartSphereVertices.begin(), m_StartSphereVertices.end());
	allVertices.insert(allVertices.end(), m_EndSphereVertices.begin(), m_EndSphereVertices.end());

	ID3D11Buffer* vertexBuffer = nullptr;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(LINE_VERTEX) * allVertices.size());
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = allVertices.data();

	Renderer::GetDevice()->CreateBuffer(&bufferDesc, &sd, &m_VertexBuffer);
}

void CharacterBoneCollision::CreateSphereLine(const XMFLOAT4& Color, std::vector<LINE_VERTEX>& SphereVertices)
{
	XMFLOAT4 color = DEBUG_LINE_COLOR;

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
