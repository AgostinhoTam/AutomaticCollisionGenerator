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
		if (m_IsDebugMode)
		{
			m_IsDebugMode = false;
		}
		else
		{
			m_IsDebugMode = true;
		}
	}
	//	アニメーションデータから
	aiAnimation* animation1 = m_Animation[m_CurrentAnimation]->mAnimations[0];
	aiAnimation* animation2 = m_Animation[m_NextAnimation]->mAnimations[0];

	for (auto pair : m_Bone)
	{
		BONE* bone = &m_Bone[pair.first];

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

		bone->AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rot, pos);
	}

	//	再帰的にボーンmatrix
	aiMatrix4x4 rootMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f),
		aiQuaternion((float)AI_MATH_PI, 0.0f, 0.0f),
		aiVector3D(0.0f, 0.0f, 0.0f));
	UpdateBoneMatrix(m_AiScene->mRootNode, rootMatrix);

	//	頂点変換(CPUスキニング)
	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; ++m)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];

		D3D11_MAPPED_SUBRESOURCE ms;
		Renderer::GetDeviceContext()->Map(m_VertexBuffer[m], 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

		VERTEX_3D* vertex = (VERTEX_3D*)ms.pData;

		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			DEFORM_VERTEX* deformVertex = &m_DeformVertex[m][v];

			aiMatrix4x4 matrix[4];
			aiMatrix4x4 outMatrix;
			matrix[0] = m_Bone[deformVertex->BoneName[0]].Matrix;	//関節は複数の頂点じゃないとダメ、とりあえず4つで足りる
			matrix[1] = m_Bone[deformVertex->BoneName[1]].Matrix;
			matrix[2] = m_Bone[deformVertex->BoneName[2]].Matrix;
			matrix[3] = m_Bone[deformVertex->BoneName[3]].Matrix;

			//	元々はこれで計算できる、aimpないからバラバラにした
			// outMatrix = matrix[0] * deformVertex->BoneWeight[0]+
			//				matrix[1] * deformVertex->BoneWeight[1]+
			//				matrix[2] * deformVertex->BoneWeight[2]+
			//				matrix[3] * deformVertex->BoneWeight[3]+
			{
				outMatrix.a1 = matrix[0].a1 * deformVertex->BoneWeight[0] +
					matrix[1].a1 * deformVertex->BoneWeight[1] +
					matrix[2].a1 * deformVertex->BoneWeight[2] +
					matrix[3].a1 * deformVertex->BoneWeight[3];

				outMatrix.a2 = matrix[0].a2 * deformVertex->BoneWeight[0] +
					matrix[1].a2 * deformVertex->BoneWeight[1] +
					matrix[2].a2 * deformVertex->BoneWeight[2] +
					matrix[3].a2 * deformVertex->BoneWeight[3];

				outMatrix.a3 = matrix[0].a3 * deformVertex->BoneWeight[0] +
					matrix[1].a3 * deformVertex->BoneWeight[1] +
					matrix[2].a3 * deformVertex->BoneWeight[2] +
					matrix[3].a3 * deformVertex->BoneWeight[3];

				outMatrix.a4 = matrix[0].a4 * deformVertex->BoneWeight[0] +
					matrix[1].a4 * deformVertex->BoneWeight[1] +
					matrix[2].a4 * deformVertex->BoneWeight[2] +
					matrix[3].a4 * deformVertex->BoneWeight[3];

				outMatrix.b1 = matrix[0].b1 * deformVertex->BoneWeight[0] +
					matrix[1].b1 * deformVertex->BoneWeight[1] +
					matrix[2].b1 * deformVertex->BoneWeight[2] +
					matrix[3].b1 * deformVertex->BoneWeight[3];

				outMatrix.b2 = matrix[0].b2 * deformVertex->BoneWeight[0] +
					matrix[1].b2 * deformVertex->BoneWeight[1] +
					matrix[2].b2 * deformVertex->BoneWeight[2] +
					matrix[3].b2 * deformVertex->BoneWeight[3];

				outMatrix.b3 = matrix[0].b3 * deformVertex->BoneWeight[0] +
					matrix[1].b3 * deformVertex->BoneWeight[1] +
					matrix[2].b3 * deformVertex->BoneWeight[2] +
					matrix[3].b3 * deformVertex->BoneWeight[3];

				outMatrix.b4 = matrix[0].b4 * deformVertex->BoneWeight[0] +
					matrix[1].b4 * deformVertex->BoneWeight[1] +
					matrix[2].b4 * deformVertex->BoneWeight[2] +
					matrix[3].b4 * deformVertex->BoneWeight[3];

				outMatrix.c1 = matrix[0].c1 * deformVertex->BoneWeight[0] +
					matrix[1].c1 * deformVertex->BoneWeight[1] +
					matrix[2].c1 * deformVertex->BoneWeight[2] +
					matrix[3].c1 * deformVertex->BoneWeight[3];

				outMatrix.c2 = matrix[0].c2 * deformVertex->BoneWeight[0] +
					matrix[1].c2 * deformVertex->BoneWeight[1] +
					matrix[2].c2 * deformVertex->BoneWeight[2] +
					matrix[3].c2 * deformVertex->BoneWeight[3];

				outMatrix.c3 = matrix[0].c3 * deformVertex->BoneWeight[0] +
					matrix[1].c3 * deformVertex->BoneWeight[1] +
					matrix[2].c3 * deformVertex->BoneWeight[2] +
					matrix[3].c3 * deformVertex->BoneWeight[3];

				outMatrix.c4 = matrix[0].c4 * deformVertex->BoneWeight[0] +
					matrix[1].c4 * deformVertex->BoneWeight[1] +
					matrix[2].c4 * deformVertex->BoneWeight[2] +
					matrix[3].c4 * deformVertex->BoneWeight[3];

				outMatrix.d1 = matrix[0].d1 * deformVertex->BoneWeight[0] +
					matrix[1].d1 * deformVertex->BoneWeight[1] +
					matrix[2].d1 * deformVertex->BoneWeight[2] +
					matrix[3].d1 * deformVertex->BoneWeight[3];

				outMatrix.d2 = matrix[0].d2 * deformVertex->BoneWeight[0] +
					matrix[1].d2 * deformVertex->BoneWeight[1] +
					matrix[2].d2 * deformVertex->BoneWeight[2] +
					matrix[3].d2 * deformVertex->BoneWeight[3];

				outMatrix.d3 = matrix[0].d3 * deformVertex->BoneWeight[0] +
					matrix[1].d3 * deformVertex->BoneWeight[1] +
					matrix[2].d3 * deformVertex->BoneWeight[2] +
					matrix[3].d3 * deformVertex->BoneWeight[3];

				outMatrix.d4 = matrix[0].d4 * deformVertex->BoneWeight[0] +
					matrix[1].d4 * deformVertex->BoneWeight[1] +
					matrix[2].d4 * deformVertex->BoneWeight[2] +
					matrix[3].d4 * deformVertex->BoneWeight[3];
			}
			deformVertex->Position = mesh->mVertices[v];
			deformVertex->Position *= outMatrix;

			//	法線変換用に移動成分を削除
			outMatrix.a4 = 0.0f;
			outMatrix.b4 = 0.0f;
			outMatrix.c4 = 0.0f;

			deformVertex->Normal = mesh->mNormals[v];
			deformVertex->Normal *= outMatrix;

			//　頂点バッファへ書き込む
			vertex[v].Position.x = deformVertex->Position.x;
			vertex[v].Position.y = deformVertex->Position.y;
			vertex[v].Position.z = deformVertex->Position.z;

			vertex[v].Normal.x = deformVertex->Normal.x;
			vertex[v].Normal.y = deformVertex->Normal.y;
			vertex[v].Normal.z = deformVertex->Normal.z;

			if (mesh->HasTextureCoords(0))
			{

				vertex[v].TexCoord.x = mesh->mTextureCoords[0][v].x;
				vertex[v].TexCoord.y = mesh->mTextureCoords[0][v].y;
				vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			}
		}
		Renderer::GetDeviceContext()->Unmap(m_VertexBuffer[m], 0);
	}
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

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);	
	//if (!m_IsDebugMode)
	//{
	//}
	//else
	//{
	//	Renderer::SetDepthEnable(false);
	//	Renderer::SetBlendState(BLEND_MODE::BLEND_MODE_ATC);
	//	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	//}
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

		// 頂点バッファ生成
		{
			VERTEX_3D* vertex = new VERTEX_3D[mesh->mNumVertices];

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				vertex[v].Position = XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
				vertex[v].Normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
				vertex[v].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * mesh->mNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = vertex;

			Renderer::GetDevice()->CreateBuffer(&bd, &sd,
				&m_VertexBuffer[m]);

			delete[] vertex;
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



		//変形後頂点データ初期化
		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			DEFORM_VERTEX deformVertex;
			deformVertex.Position = mesh->mVertices[v];
			deformVertex.Normal = mesh->mNormals[v];
			deformVertex.BoneNum = 0;

			for (unsigned int b = 0; b < 4; b++)
			{
				deformVertex.BoneName[b] = "";
				deformVertex.BoneWeight[b] = 0.0f;
			}

			m_DeformVertex[m].push_back(deformVertex);
		}


		//ボーンデータ初期化
		for (unsigned int b = 0; b < mesh->mNumBones; b++)
		{
			aiBone* bone = mesh->mBones[b];

			m_Bone[bone->mName.C_Str()].OffsetMatrix = bone->mOffsetMatrix;

			//変形後頂点にボーンデータ格納
			for (unsigned int w = 0; w < bone->mNumWeights; w++)
			{
				aiVertexWeight weight = bone->mWeights[w];

				int num = m_DeformVertex[m][weight.mVertexId].BoneNum;

				m_DeformVertex[m][weight.mVertexId].BoneWeight[num] = weight.mWeight;
				m_DeformVertex[m][weight.mVertexId].BoneName[num] = bone->mName.C_Str();	//　関連する頂点にボーン名前入れる
				m_DeformVertex[m][weight.mVertexId].BoneNum++;

				assert(m_DeformVertex[m][weight.mVertexId].BoneNum <= 4);	
			}
		}
	}


	// TODO Textureを再利用できるように
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
			LoadFromWICMemory(aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);	//	メモリに保存できる
			CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
			assert(texture);
			m_Texture[aitexture->mFilename.data] = texture;
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

	BONE bone;

	m_Bone[node->mName.C_Str()] = bone;

	for (unsigned int n = 0; n < node->mNumChildren; ++n)
	{
		CreateBone(node->mChildren[n]);
	}
}

void AnimationModel::UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix)
{
	BONE* bone = &m_Bone[node->mName.C_Str()];

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
	auto it = m_Bone.find(name);
	if (it != m_Bone.end())
	{
		BONE* bone = &(it->second);
		return bone->worldMatrix;
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

XMFLOAT3 AnimationModel::GetHeadPosition(const std::string& BoneName, const XMFLOAT3& Scale, const XMMATRIX& PlayerMatrix)
{
	auto it = m_Bone.find(BoneName);
	if (it != m_Bone.end())
	{

		XMMATRIX& headMatrix = it->second.localMatrix;

		XMMATRIX scale = XMMatrixScaling(1.0f/Scale.x, 1.0f/Scale.y, 1.0f/Scale.z);	//　OwnerのScaleに合わせる
		
		XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(0, 0, 0);
		
		XMMATRIX transMatrix = XMMatrixTranslation(0, 0, 0);

		XMMATRIX worldMatrix = XMMatrixMultiply(rotMatrix, transMatrix);
		worldMatrix = XMMatrixMultiply(worldMatrix, scale);
		worldMatrix = XMMatrixMultiply(worldMatrix, headMatrix);
		worldMatrix = XMMatrixMultiply(worldMatrix, PlayerMatrix);
		
		
		XMVECTOR trans, rot, scaleVec;
		XMMatrixDecompose(&scaleVec, &rot, &trans, worldMatrix);

		return XMFLOAT3(XMVectorGetX(trans),XMVectorGetY(trans),XMVectorGetZ(trans));
	}
	return XMFLOAT3(0,0,0);
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
	// 1-1) Tailボーンの "OffsetMatrix" を逆行列化 → Tailボーン空間原点を "メッシュ空間" へ
	aiMatrix4x4 offsetTailInv = m_Bone[TailName].OffsetMatrix;
	offsetTailInv.Inverse();
	// メッシュ空間における "Tailボーン原点"
	aiVector3D tailPosMesh = offsetTailInv * aiVector3D(0.f, 0.f, 0.f);

	// 1-2) Headボーンの "OffsetMatrix" を使って メッシュ空間 → Headボーン空間 へ変換
	aiMatrix4x4 offsetHead = m_Bone[HeadName].OffsetMatrix;
	aiVector3D tailPosHead = offsetHead * tailPosMesh;

	// 1-3) 線分 [S, E] を定義
	//     S = (0,0,0) in HeadNameボーン空間
	//     E = tailPosHead
	XMFLOAT3 S(0.f, 0.f, 0.f);
	XMFLOAT3 E(tailPosHead.x, tailPosHead.y, tailPosHead.z);

	// ---------------------------------------------
	// 2) 頂点を HeadNameボーン空間へ変換 → 線分との最短距離
	// ---------------------------------------------
	float maxDistSq = 0.0f;  // 距離^2 の最大値
	std::vector<float> radiusList;

	// メッシュごと
	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; ++m)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];
		// 頂点ごと
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			aiVector3D bindPos = mesh->mVertices[v]; // バインドポーズ時点(メッシュ空間)
			DEFORM_VERTEX& deformVertex = m_DeformVertex[m][v];

			// この頂点が影響を受けるボーンの数 (最大4)
			int boneCount = deformVertex.BoneNum;

			// 影響ボーンをチェック
			for (int i = 0; i < boneCount; i++)
			{
				const std::string& boneName = deformVertex.BoneName[i];

				// "HeadName" ボーンだけを対象に計算 (TailName は単に軸の終点)
				if (boneName != HeadName)
				{
					continue;
				}

				// 頂点を "HeadName" ボーン空間へ変換
				aiMatrix4x4 offsetHeadMat = m_Bone[HeadName].OffsetMatrix;
				aiVector3D localPos = offsetHeadMat * bindPos; // メッシュ空間→Headボーン空間

				// 点と線分の最短距離
				XMFLOAT3 p(localPos.x, localPos.y, localPos.z);
				float dist = DistancePointLineSegment(p, S, E);

				radiusList.emplace_back(dist);

			}
		}
	}

	// ---------------------------------------------
	// 3) 半径 
	// ---------------------------------------------
	if (radiusList.empty()) return 0.0f;
	// 中央値を計算
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



