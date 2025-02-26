#include <numeric> 
#include "Main\main.h"
#include "Manager\inputManager.h"
#include "GameObject\gameobject.h"
#include "assimp\Importer.hpp"
//#include "System\Collision\sphereCollision.h"
#include "System\Renderer\renderer.h"
#include "Manager\animationModelManager.h"
#include "System\Renderer\animationModelResource.h"
#include "System\Renderer\animator.h"
#include "System\Renderer\animationModelInstance.h"

constexpr float RADIUS_ADJUSTMENT = 0.7f;

void AnimationModelInstance::Update()
{
	if (!m_Animator)return;
	const std::unordered_map<std::string, const aiScene*>& animationMap = m_AnimationResource->GetAnimationMap();
	const std::string& currenmtAnimation = m_Animator->GetCurrentAnimationName();
	const std::string& nexttAnimation = m_Animator->GetNextAnimationName();
	
	//	アニメーション更新
	m_Animator->Update(animationMap.at(currenmtAnimation), animationMap.at(nexttAnimation));

	//	再帰的にボーンmatrix
	aiMatrix4x4 rootMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f),
		aiQuaternion((float)AI_MATH_PI, 0.0f, 0.0f),
		aiVector3D(0.0f, 0.0f, 0.0f));
	UpdateBoneMatrix(m_AnimationResource->GetAiScene()->mRootNode, rootMatrix);

	UpdateBoneMatrixToGPU();

}

void AnimationModelInstance::Draw()
{

	if (!m_Owner)return;

	XMMATRIX world, scale, rot, trans;
	const XMFLOAT3& objPosition = m_Owner->GetPosition();
	const XMFLOAT3& objScale = m_Owner->GetScale();
	const XMFLOAT3& objRotation = m_Owner->GetRotation();
	const Shader* Shader = m_Owner->GetShader();

	scale = XMMatrixScaling(objScale.x, objScale.y, objScale.z);

	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(objRotation.x, objRotation.y, objRotation.z);
	quaternion = XMQuaternionNormalize(quaternion);
	rot = XMMatrixRotationQuaternion(quaternion);

	trans = XMMatrixTranslation(objPosition.x, objPosition.y, objPosition.z);
	world = scale * rot * trans;
	Renderer::SetWorldMatrix(world);

	Renderer::GetDeviceContext()->IASetInputLayout(Shader->m_VertexLayout);
	Renderer::GetDeviceContext()->VSSetShader(Shader->m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(Shader->m_PixelShader, NULL, 0);

	// プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::SetMaterial(material);
	const aiScene* aiScene = m_AnimationResource->GetAiScene();
	const std::unordered_map<std::string, ID3D11ShaderResourceView*>& textureMap = m_AnimationResource->GetTexture();
	for (unsigned int m = 0; m < aiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = aiScene->mMeshes[m];


		// マテリアル設定
		aiString texture;
		aiColor3D diffuse;
		float opacity;

		aiMaterial* aimaterial = aiScene->mMaterials[mesh->mMaterialIndex];
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);
		aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		aimaterial->Get(AI_MATKEY_OPACITY, opacity);

		if (texture == aiString(""))
		{
			material.TextureEnable = false;
		}
		else
		{
			Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &textureMap.at(texture.data));
			material.TextureEnable = true;
		}

		material.Diffuse = XMFLOAT4(diffuse.r, diffuse.g, diffuse.b, opacity);

		material.Ambient = material.Diffuse;
		Renderer::SetMaterial(material);
		ID3D11Buffer** vertexBuffer = m_AnimationResource->GetVertexBUffer();
		ID3D11Buffer** indexBuffer = m_AnimationResource->GetIndexBUffer();

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D_SKIN);
		UINT offset = 0;
		Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer[m], &stride, &offset);

		// インデックスバッファ設定
		Renderer::GetDeviceContext()->IASetIndexBuffer(indexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// ポリゴン描画
		Renderer::GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
	}
	Renderer::SetDepthEnable(true);
	Renderer::SetBlendState(BLEND_MODE::BLEND_MODE_NONE);
}

AnimationModelInstance::AnimationModelInstance(const MODEL_NAME& Model, GameObject* Owner)
{
	m_Owner = Owner;
	m_AnimationResource = AnimationModelManager::LoadAnimationModel(Model);
	m_Animator = new Animator(this,m_Bones,m_BoneIndexMap);
	//	ボーンの定数バッファ作成
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(CB_BONES);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		Renderer::GetDevice()->CreateBuffer(&bd, nullptr, &m_BoneMatricesBuffer);
	}
}


void AnimationModelInstance::Uninit()
{
	delete[] m_DeformVertex;
	if (m_BoneMatricesBuffer)m_BoneMatricesBuffer->Release();

}


void AnimationModelInstance::UpdateBoneMatrix(const aiNode* node, const aiMatrix4x4& matrix)
{
	std::string bName = node->mName.C_Str();
	aiMatrix4x4 localMatrix{};
	auto it = m_BoneIndexMap.find(bName);
	if (it != m_BoneIndexMap.end())
	{
		int index = it->second;

		BONE* bone = &m_Bones[index];

		aiMatrix4x4 worldMatrix = matrix * bone->AnimationMatrix;		//	親からのマトリクスと自信のアニメーション変換
		bone->localMatrix = TransformToXMMATRIX(worldMatrix);		//	DX用ローカルマトリクス

		// **スキニング用の最終行列 (ワールド行列 × オフセット行列)**
		bone->Matrix = worldMatrix * bone->OffsetMatrix;			//	各頂点に適用するマトリクス

		// **DirectX用の行列に変換**
		bone->worldMatrix = TransformToXMMATRIX(bone->Matrix);		//	DX用に変換

		localMatrix = worldMatrix;									//	アニメーション適用後のマトリクスを次に渡す準備

	}
	else
	{
		localMatrix = matrix;
	}
	for (unsigned int n = 0; n < node->mNumChildren; ++n)
	{
		UpdateBoneMatrix(node->mChildren[n], localMatrix);
	}


}

void AnimationModelInstance::SetNextAnimation(const std::string& AnimationName)
{
	m_Animator->SetNextAnimation(AnimationName);
}

const bool AnimationModelInstance::GetIsTransitioning()const
{
	return m_Animator->GetIsTransitioning();
}

void AnimationModelInstance::SetNextAnimationFrame(unsigned int Frame)
{
	m_Animator->SetNextAnimationFrame(Frame);
}

XMMATRIX AnimationModelInstance::TransformToXMMATRIX(aiMatrix4x4& world)
{
	return XMMATRIX(
		world.a1, world.b1, world.c1, world.d1,
		world.a2, world.b2, world.c2, world.d2,
		world.a3, world.b3, world.c3, world.d3,
		world.a4, world.b4, world.c4, world.d4
	);
}


const BONE AnimationModelInstance::GetBone(int Index)
{
	if (Index <= m_Bones.size())
	{
		return m_Bones[Index];
	}
	else
	{
		return m_Bones[0];
	}
}

XMFLOAT3 AnimationModelInstance::GetBonePosition(const int BoneIndex, const XMMATRIX& PlayerMatrix)
{
	//	範囲外早期リターン
	if (BoneIndex < 0 || BoneIndex >= m_Bones.size())return XMFLOAT3{};

	XMMATRIX& headMatrix = m_Bones[BoneIndex].localMatrix;

	XMMATRIX worldMatrix = XMMatrixMultiply(headMatrix, PlayerMatrix);

	XMVECTOR trans, rot, scaleVec;
	XMMatrixDecompose(&scaleVec, &rot, &trans, worldMatrix);

	return XMFLOAT3(XMVectorGetX(trans), XMVectorGetY(trans), XMVectorGetZ(trans));

}


const float AnimationModelInstance::CalculateCapsuleRadius(const std::string& HeadName, const std::string& TailName)
{
	auto headit = m_BoneIndexMap.find(HeadName);
	auto tailit = m_BoneIndexMap.find(TailName);

	if (headit == m_BoneIndexMap.end() || tailit == m_BoneIndexMap.end())
	{
		return 0.0f;
	}

	int headIndex = headit->second;
	int tailIndex = tailit->second;

	// HeadとTailの位置をメッシュ空間で取得
	aiMatrix4x4 offsetHeadInv = m_Bones[headIndex].OffsetMatrix;
	offsetHeadInv.Inverse();
	aiVector3D headPosMesh = offsetHeadInv * aiVector3D(0.0f, 0.0f, 0.0f);

	aiMatrix4x4 offsetTailInv = m_Bones[tailIndex].OffsetMatrix;
	offsetTailInv.Inverse();
	aiVector3D tailPosMesh = offsetTailInv * aiVector3D(0.0f, 0.0f, 0.0f);

	// 線分 [S, E] をメッシュ空間で定義
	XMFLOAT3 S(headPosMesh.x, headPosMesh.y, headPosMesh.z);
	XMFLOAT3 E(tailPosMesh.x, tailPosMesh.y, tailPosMesh.z);

	// 2) 頂点をメッシュ空間で直接計算
	float maxDistSq = 0.0f;
	std::vector<float> radiusList;

	const aiScene* aiScene = m_AnimationResource->GetAiScene();

	for (unsigned int m = 0; m < aiScene->mNumMeshes; ++m)
	{
		aiMesh* mesh = aiScene->mMeshes[m];
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			aiVector3D bindPos = mesh->mVertices[v]; // メッシュ空間の頂点

			DEFORM_VERTEX& deformVertex = m_DeformVertex[m][v];
			int boneCount = deformVertex.BoneNum;

			for (int i = 0; i < boneCount; i++)
			{
				const std::string& boneName = deformVertex.BoneName[i];

				// 頂点がHeadまたはTailボーンの影響を受けている場合のみ計算
				if (boneName != HeadName && boneName != TailName)
					continue;

				// メッシュ空間の頂点位置を使用
				XMFLOAT3 p(bindPos.x, bindPos.y, bindPos.z);

				// 点と線分の最短距離
				float dist = DistancePointLineSegment(p, S, E);
				radiusList.emplace_back(dist);
			}
		}
	}

	if (radiusList.empty()) return 0.0f;

	// 中央値を半径として使用
	size_t size = radiusList.size();
	std::nth_element(radiusList.begin(), radiusList.begin() + size / 2, radiusList.end());
	float median = radiusList[size / 2];

	return median;

}

const float AnimationModelInstance::DistancePointLineSegment(const XMFLOAT3& Point, const XMFLOAT3& Start, const XMFLOAT3& End)
{
	XMVECTOR vP = XMLoadFloat3(&Point);
	XMVECTOR vS = XMLoadFloat3(&Start);
	XMVECTOR vE = XMLoadFloat3(&End);

	XMVECTOR vSE = XMVectorSubtract(vE, vS); // 線分ベクトル E - S
	XMVECTOR vSP = XMVectorSubtract(vP, vS); // p - s

	// SE の長さ^2
	float segLenSq = XMVectorGetX(XMVector3Dot(vSE, vSE));
	if (segLenSq < 1e-6f)
	{
		// S と E がほぼ同じ座標なら、Sとの距離を返す
		return XMVectorGetX(XMVector3Length(vSP));
	}

	// 点PをSEに射影したときのパラメータ t (0 <= t <= 1)
	float dot = XMVectorGetX(XMVector3Dot(vSP, vSE));
	float t = dot / segLenSq;

	// 線分範囲内にクランプ
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;

	// 最近接点 C = S + t*(E - S)
	XMVECTOR vClosest = XMVectorAdd(vS, XMVectorScale(vSE, t));

	// P-C の距離
	XMVECTOR vDiff = XMVectorSubtract(vP, vClosest);
	float dist = XMVectorGetX(XMVector3Length(vDiff));
	return dist;
}

void AnimationModelInstance::UpdateBoneMatrixToGPU()
{
	CB_BONES* cbBones = new CB_BONES;

	int boneCount = static_cast<int>(m_Bones.size());

	//	バッファに入れる
	for (int i = 0; i < boneCount; ++i)
	{
		cbBones->BoneMatrices[i] = m_Bones[i].worldMatrix;
		cbBones->BoneMatrices[i] = XMMatrixTranspose(cbBones->BoneMatrices[i]);

	}
	//	残りはリセット
	for (int i = boneCount; i < MAX_BONES; ++i)
	{
		cbBones->BoneMatrices[i] = XMMatrixIdentity();
	}

	// 定数バッファ更新
	D3D11_MAPPED_SUBRESOURCE ms;
	Renderer::GetDeviceContext()->Map(m_BoneMatricesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, cbBones, sizeof(CB_BONES));
	Renderer::GetDeviceContext()->Unmap(m_BoneMatricesBuffer, 0);

	// VSにセット
	Renderer::GetDeviceContext()->VSSetConstantBuffers(6, 1, &m_BoneMatricesBuffer);

	delete cbBones;
}


