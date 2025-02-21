#include <numeric> 
#include "Main\main.h"
#include "Manager\inputManager.h"
#include "GameObject\gameobject.h"
//#include "System\Collision\sphereCollision.h"
#include "System\Renderer\renderer.h"
#include "System\Renderer\animationModel.h"

constexpr float RADIUS_ADJUSTMENT = 0.7f;

void AnimationModel::Update()
{
	if (m_Animation.count(m_CurrentAnimation) == 0)return;
	if (!m_Animation[m_CurrentAnimation]->HasAnimations())return;
	if (m_Animation.count(m_NextAnimation) == 0)return;
	if (!m_Animation[m_NextAnimation]->HasAnimations())return;

	if (InputManager::GetKeyTrigger('J'))
	{
		m_IsDebugMode = !m_IsDebugMode;
	}
	//	アニメーションデータから
	aiAnimation* animation1 = m_Animation[m_CurrentAnimation]->mAnimations[0];
	aiAnimation* animation2 = m_Animation[m_NextAnimation]->mAnimations[0];

	for (auto pair : m_BoneIndexMap)
	{
		int boneIndex = pair.second;
		BONE& bone = m_Bones[boneIndex];

		aiNodeAnim* nodeAnim1 = nullptr;
		aiNodeAnim* nodeAnim2 = nullptr;

		for (unsigned int c = 0; c < animation1->mNumChannels; ++c)
		{
			if (animation1->mChannels[c]->mNodeName == aiString(pair.first))
			{
				nodeAnim1 = animation1->mChannels[c];
				break;
			}
		}
		for (unsigned int c = 0; c < animation2->mNumChannels; ++c)
		{
			if (animation2->mChannels[c]->mNodeName == aiString(pair.first))
			{
				nodeAnim2 = animation2->mChannels[c];
				break;
			}
		}

		int f;

		aiQuaternion rot1;
		aiVector3D pos1;
		aiQuaternion rot2;
		aiVector3D pos2;


		if (nodeAnim1)
		{
			f = m_CurrentFrame % nodeAnim1->mNumRotationKeys;	//簡易実装
			rot1 = nodeAnim1->mRotationKeys[f].mValue;
			f = m_CurrentFrame % nodeAnim1->mNumPositionKeys;	//簡易実装
			pos1 = nodeAnim1->mPositionKeys[f].mValue;
		}
		if (nodeAnim2)
		{
			f = m_NextFrame % nodeAnim2->mNumRotationKeys;	//簡易実装
			rot2 = nodeAnim2->mRotationKeys[f].mValue;
			f = m_NextFrame % nodeAnim2->mNumPositionKeys;	//簡易実装
			pos2 = nodeAnim2->mPositionKeys[f].mValue;
		}
		aiVector3D pos;
		aiQuaternion rot;
		pos = pos1 * (1.0f - m_BlendRatio) + pos2 * m_BlendRatio;	//線形補間
		aiQuaternion::Interpolate(rot, rot1, rot2, m_BlendRatio);	//球面線形補間

		bone.AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rot, pos);
	}

	//	再帰的にボーンmatrix
	aiMatrix4x4 rootMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f),
		aiQuaternion((float)AI_MATH_PI, 0.0f, 0.0f),
		aiVector3D(0.0f, 0.0f, 0.0f));
	UpdateBoneMatrix(m_AiScene->mRootNode, rootMatrix);

	std::vector<XMMATRIX> boneMatrices(m_Bones.size());
	for (size_t i = 0; i < m_Bones.size(); ++i)
	{
		boneMatrices[i] = m_Bones[i].worldMatrix;
	}
	Renderer::GetDeviceContext()->UpdateSubresource(m_BoneConstantBuffer, 0, nullptr, boneMatrices.data(), 0, 0);

}

void AnimationModel::Draw(GameObject* Object)
{

	XMMATRIX world, scale, rot, trans;
	const XMFLOAT3& objPosition = Object->GetPosition();
	const XMFLOAT3& objScale = Object->GetScale();
	const XMFLOAT3& objRotation = Object->GetRotation();
	const Shader* Shader = Object->GetShader();

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
	
	Renderer::GetDeviceContext()->VSSetConstantBuffers(6, 1, &m_BoneConstantBuffer);
	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];


		// マテリアル設定
		aiString texture;
		aiColor3D diffuse;
		float opacity;

		aiMaterial* aimaterial = m_AiScene->mMaterials[mesh->mMaterialIndex];
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);
		aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		aimaterial->Get(AI_MATKEY_OPACITY, opacity);

		if (texture == aiString(""))
		{
			material.TextureEnable = false;
		}
		else
		{
			Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[texture.data]);
			material.TextureEnable = true;
		}

		material.Diffuse = XMFLOAT4(diffuse.r, diffuse.g, diffuse.b, opacity);

		material.Ambient = material.Diffuse;
		Renderer::SetMaterial(material);


		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer[m], &stride, &offset);

		// インデックスバッファ設定
		Renderer::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// ポリゴン描画
		Renderer::GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
	}
	Renderer::SetDepthEnable(true);
	Renderer::SetBlendState(BLEND_MODE::BLEND_MODE_NONE);
}

void AnimationModel::Load(const char* FileName)
{
	const std::string modelPath(FileName);

	m_AiScene = aiImportFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	assert(m_AiScene);

	m_VertexBuffer = new ID3D11Buffer * [m_AiScene->mNumMeshes];
	m_IndexBuffer = new ID3D11Buffer * [m_AiScene->mNumMeshes];


	//変形後頂点配列生成
	m_DeformVertex = new std::vector<DEFORM_VERTEX>[m_AiScene->mNumMeshes];

	//	再帰的にボーン生成
	CreateBone(m_AiScene->mRootNode);

	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];
		std::vector<VERTEX_3D_SKINNING> vertices;
		vertices.resize(mesh->mNumVertices);

		{
			for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
			{
				vertices[v].Position = XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
				vertices[v].Normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
				if (mesh->HasTextureCoords(0))
				{
					vertices[v].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				}
				else
				{
					vertices[v].TexCoord = XMFLOAT2(0, 0);
				}
				vertices[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				for (int i = 0; i < 4; ++i)
				{
					vertices[v].BoneIndex[i] = 0;
					vertices[v].BoneWeight[i] = 0.0f;
				}
			}
		}

		//	メッシュボーン
		for (unsigned int b = 0; b < mesh->mNumBones; ++b)
		{
		
			aiBone* bone = mesh->mBones[b];
			std::string boneName = bone->mName.C_Str();

			int boneIndex = static_cast<int>(m_Bones.size());

			auto it = m_BoneIndexMap.find(boneName);
			if (it == m_BoneIndexMap.end())
			{
				BONE newBone;
				newBone.OffsetMatrix = bone->mOffsetMatrix;
				newBone.Matrix = aiMatrix4x4();
				newBone.AnimationMatrix = aiMatrix4x4();
				m_Bones.push_back(newBone);
				m_BoneIndexMap[boneName] = boneIndex;
			}
			else
			{
				boneIndex = it->second;
				m_Bones[boneIndex].OffsetMatrix = bone->mOffsetMatrix;
			}

			for (unsigned int w = 0; w < bone->mNumWeights; ++w)
			{
				unsigned int vID = bone->mWeights[w].mVertexId;
				float weight = bone->mWeights[w].mWeight;

				for (int i = 0; i < 4; ++i)
				{
					if (vertices[vID].BoneWeight[i] == 0.0f)
					{
						vertices[vID].BoneIndex[i] = boneIndex;
						vertices[vID].BoneWeight[i] = weight;
					}
				}
			}
			{

				D3D11_BUFFER_DESC bd;
				ZeroMemory(&bd, sizeof(bd));
				bd.Usage = D3D11_USAGE_DEFAULT;
				bd.ByteWidth = sizeof(VERTEX_3D_SKINNING) * mesh->mNumVertices;
				bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bd.CPUAccessFlags = 0;

				D3D11_SUBRESOURCE_DATA sd;
				ZeroMemory(&sd, sizeof(sd));
				sd.pSysMem = vertices.data();

				Renderer::GetDevice()->CreateBuffer(&bd, &sd,
					&m_VertexBuffer[m]);
			}
		}

		// インデックスバッファ生成
		{
			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

			for (unsigned int f = 0; f < mesh->mNumFaces; f++)
			{
				const aiFace* face = &mesh->mFaces[f];

				assert(face->mNumIndices == 3);

				index[f * 3 + 0] = face->mIndices[0];
				index[f * 3 + 1] = face->mIndices[1];
				index[f * 3 + 2] = face->mIndices[2];
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = index;

			Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer[m]);

			delete[] index;
		}

		//テクスチャ読み込み
		for (unsigned int i = 0; i < m_AiScene->mNumTextures; i++)
		{
			aiTexture* aitexture = m_AiScene->mTextures[i];
			if (aitexture->mHeight == 0)
			{

				ID3D11ShaderResourceView* texture;

				// テクスチャ読み込み
				TexMetadata metadata;
				ScratchImage image;
				std::string filename(aitexture->mFilename.C_Str());
				std::string extension = filename.substr(filename.find_last_of('.') + 1);
				if (extension == "tga" || extension == "TGA")
				{
					LoadFromTGAMemory(aitexture->pcData, aitexture->mWidth, &metadata, image);
				}
				else if (extension == "dds" || extension == "DDS")
				{
					LoadFromDDSMemory(aitexture->pcData, aitexture->mWidth, DDS_FLAGS_NONE, &metadata, image);
				}
				else
				{
					LoadFromWICMemory(aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);
				}
				CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
				assert(texture);
				m_Texture[aitexture->mFilename.data] = texture;
			}


		}

		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(XMMATRIX) * 256; // とりあえず256個分確保
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;
			HRESULT hr = Renderer::GetDevice()->CreateBuffer(&bd, nullptr, &m_BoneConstantBuffer);
			assert(SUCCEEDED(hr));
		}

	}
}

void AnimationModel::Uninit()
{
	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; m++)
	{
		m_VertexBuffer[m]->Release();
		m_IndexBuffer[m]->Release();
	}

	delete[] m_VertexBuffer;
	delete[] m_IndexBuffer;

	delete[] m_DeformVertex;


	for (std::pair<const std::string, ID3D11ShaderResourceView*> pair : m_Texture)
	{
		pair.second->Release();
	}



	aiReleaseImport(m_AiScene);


	for (std::pair<const std::string, const aiScene*> pair : m_Animation)
	{
		aiReleaseImport(pair.second);
	}

	if (m_BoneConstantBuffer)
	{
		m_BoneConstantBuffer->Release();
		m_BoneConstantBuffer = nullptr;
	}
}

void AnimationModel::LoadAnimation(const char* FileName, const char* Name)
{
	m_Animation[Name] = aiImportFile(FileName, aiProcess_ConvertToLeftHanded);
	assert(m_Animation[Name]);
}

void AnimationModel::CreateBone(aiNode* node)
{

	if (std::string(node->mName.C_Str()).find("$AssimpFbx$") != std::string::npos)
	{
		return; // 補助ボーンスキップ
	}

	//	index作成
	if (m_BoneIndexMap.find(node->mName.C_Str()) == m_BoneIndexMap.end())
	{
		BONE bone;
		m_Bones.emplace_back(bone);
		int index = static_cast<int>(m_Bones.size());
		m_BoneIndexMap[node->mName.C_Str()] = index - 1;
	}

	for (unsigned int n = 0; n < node->mNumChildren; ++n)
	{
		CreateBone(node->mChildren[n]);
	}
}

void AnimationModel::UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix)
{
	auto it = m_BoneIndexMap.find(node->mName.C_Str());
	if (it == m_BoneIndexMap.end())return;

	int boneIndex = it->second;
	BONE* bone = &m_Bones[boneIndex];

	aiMatrix4x4 worldMatrix = matrix * bone->AnimationMatrix;		//マトリクス
	bone->localMatrix = TransformToXMMATRIX(worldMatrix);

	// **スキニング用の最終行列 (ワールド行列 × オフセット行列)**
	bone->Matrix = worldMatrix * bone->OffsetMatrix;

	// **DirectX用の行列に変換**
	bone->worldMatrix = TransformToXMMATRIX(bone->Matrix);

	for (unsigned int n = 0; n < node->mNumChildren; ++n)
	{
		UpdateBoneMatrix(node->mChildren[n], worldMatrix);
	}
}

XMMATRIX AnimationModel::TransformToXMMATRIX(aiMatrix4x4& world)
{
	return XMMATRIX(
		world.a1, world.b1, world.c1, world.d1,
		world.a2, world.b2, world.c2, world.d2,
		world.a3, world.b3, world.c3, world.d3,
		world.a4, world.b4, world.c4, world.d4
	);
}

XMMATRIX AnimationModel::GetBoneMatrix(std::string name)
{
	auto it = m_BoneIndexMap.find(name);
	if (it != m_BoneIndexMap.end())
	{
		int index = it->second;
		return m_Bones[index].worldMatrix;
	}
	else
	{
		return XMMATRIX{};
	}
}

double AnimationModel::GetAnimationDuration(const std::string& AnimationName)
{
	if (m_Animation.count(AnimationName) == 0)return 0;
	if (!m_Animation[AnimationName]->HasAnimations())return 0;
	return (m_Animation[AnimationName]->mAnimations[0]->mDuration);
}

XMFLOAT3& AnimationModel::GetHeadPosition(const std::string& BoneName, const XMFLOAT3& Scale, const XMMATRIX& PlayerMatrix)
{
	auto it = m_BoneIndexMap.find(BoneName);
	if (it != m_BoneIndexMap.end())
	{
		int index = it->second;


		XMMATRIX& headMatrix = m_Bones[index].localMatrix;

		XMMATRIX scale = XMMatrixScaling(1.0f / Scale.x, 1.0f / Scale.y, 1.0f / Scale.z);	//　OwnerのScaleに合わせる

		XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(0, 0, 0);

		XMMATRIX transMatrix = XMMatrixTranslation(0, 0, 0);

		XMMATRIX worldMatrix = XMMatrixMultiply(rotMatrix, transMatrix);
		worldMatrix = XMMatrixMultiply(worldMatrix, scale);
		worldMatrix = XMMatrixMultiply(worldMatrix, headMatrix);
		worldMatrix = XMMatrixMultiply(worldMatrix, PlayerMatrix);


		XMVECTOR trans, rot, scaleVec;
		XMMatrixDecompose(&scaleVec, &rot, &trans, worldMatrix);

		return XMFLOAT3(XMVectorGetX(trans), XMVectorGetY(trans), XMVectorGetZ(trans));
	}
	return XMFLOAT3(0, 0, 0);
}

void AnimationModel::UpdateAnimationBlend()
{

	//	遷移中だったら
	if (m_IsTransitioning)
	{
		AddBlendRatio();
		AddCurrentAnimationFrame();
		AddNextAnimationFrame();
	}
	//	普通の再生
	else
	{
		AddCurrentAnimationFrame();
	}
	//	遷移完成したら
	if (m_BlendRatio >= 1)
	{
		m_IsTransitioning = false;
		SetCurrentAnimation(m_NextAnimation);
		m_CurrentFrame = m_NextFrame;
		m_BlendRatio = 0;
	}
	//	ボーン、メッシュ更新
	Update();
}

const float AnimationModel::CalculateCapsuleRadius(const std::string& HeadName, const std::string& TailName)
{
	int headIndex = m_BoneIndexMap[HeadName];
	int tailIndex = m_BoneIndexMap[TailName];

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

	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; ++m)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];
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

const float AnimationModel::DistancePointLineSegment(const XMFLOAT3& Point, const XMFLOAT3& Start, const XMFLOAT3& End)
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

void AnimationModel::SetNextAnimation(const std::string& AnimationName)
{
	m_NextAnimation = AnimationName;
	m_IsTransitioning = true;
	m_NextFrame = 0;
}



