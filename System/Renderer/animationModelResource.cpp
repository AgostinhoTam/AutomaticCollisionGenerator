#include <numeric> 
#include "Main\main.h"
#include "Manager\inputManager.h"
#include "GameObject\gameobject.h"
#include "assimp\Importer.hpp"
//#include "System\Collision\sphereCollision.h"
#include "System\Renderer\renderer.h"
#include "System\Renderer\animationModelResource.h"

constexpr float RADIUS_ADJUSTMENT = 0.7f;


void AnimationModelResource::Load(const char* FileName)
{
	const std::string modelPath(FileName);
	m_Importer = new Assimp::Importer;

	m_Importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

	m_AiScene = m_Importer->ReadFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	assert(m_AiScene);

	m_VertexBuffer = new ID3D11Buffer * [m_AiScene->mNumMeshes];
	m_IndexBuffer = new ID3D11Buffer * [m_AiScene->mNumMeshes];

	m_Bones.clear();
	m_BoneIndexMap.clear();

	//変形後頂点配列生成
	m_DeformVertex = new std::vector<DEFORM_VERTEX>[m_AiScene->mNumMeshes];

	//	再帰的にボーン生成
	CreateBone(m_AiScene->mRootNode);

	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; ++m)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];

		m_DeformVertex[m].resize(mesh->mNumVertices);

		//	頂点情報だけ記録
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			DEFORM_VERTEX dv;
			dv.Position = mesh->mVertices[v];
			dv.Normal = mesh->mNormals[v];
			dv.BoneNum = 0;
			for (int i = 0; i < 4; ++i)
			{
				dv.BoneName[i] = "";
				dv.BoneWeight[i] = 0.0f;
			}
			m_DeformVertex[m][v] = dv;
		}


		//	メッシュに含めるボーン情報
		for (unsigned int b = 0; b < mesh->mNumBones; ++b)
		{
			aiBone* bone = mesh->mBones[b];	//　ボーン取り出す
			if (!bone)continue;
			std::string boneName = bone->mName.C_Str();
			if (boneName.length() == 0)continue;	//	名前が空っぽだったら次へ

			int newIndex;

			auto it = m_BoneIndexMap.find(boneName);

			//	もしボーンがまだ存在しないなら作成
			if (it == m_BoneIndexMap.end())
			{
				newIndex = static_cast<int>(m_Bones.size());	//	ボーンにインデックス付ける

				BONE newBone;
				newBone.OffsetMatrix = bone->mOffsetMatrix;
				m_Bones.emplace_back(newBone);
				m_BoneIndexMap[boneName] = newIndex;				//	ボーン名前とインデックスのmap
			}
			else
			{

				m_Bones[it->second].OffsetMatrix = bone->mOffsetMatrix;
			}


			//	各頂点ごとのWeightを管理
			for (unsigned int w = 0; w < bone->mNumWeights; ++w)
			{
				aiVertexWeight vw = bone->mWeights[w];	//	bone weight取り出す
				int vID = vw.mVertexId;
				float wgt = vw.mWeight;

				DEFORM_VERTEX& dv = m_DeformVertex[m][vID];		//　シーンの何番目頂点

				//	ボーンの数が4以下だったら
				if (dv.BoneNum < 4)
				{
					dv.BoneName[dv.BoneNum] = boneName;
					dv.BoneWeight[dv.BoneNum] = wgt;
					++dv.BoneNum;
				}
				else
				{
					int minIndex = 0;
					float minWeight = dv.BoneWeight[0];

					//	4本のボーン走査
					for (int i = 1; i < 4; ++i)
					{
						if (dv.BoneWeight[i] < minWeight)
						{
							minWeight = dv.BoneWeight[i];
							minIndex = i;
						}
					}

					//　情報入れる
					if (wgt > minWeight)
					{
						dv.BoneName[minIndex] = boneName;
						dv.BoneWeight[minIndex] = wgt;
					}
				}
			}
		}

		//	頂点バッファ生成
		{
			auto* vertexData = new VERTEX_3D_SKIN[mesh->mNumVertices];

			for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
			{
				DEFORM_VERTEX& dv = m_DeformVertex[m][v];
				VERTEX_3D_SKIN vs;

				vs.Position = XMFLOAT3(dv.Position.x, dv.Position.y, dv.Position.z);
				vs.Normal = XMFLOAT3(dv.Normal.x, dv.Normal.y, dv.Normal.z);
				if (mesh->HasTextureCoords(0))
				{
					vs.TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				}
				else
				{
					vs.TexCoord = XMFLOAT2(0, 0);
				}
				vs.Diffuse = XMFLOAT4(1, 1, 1, 1);

				//	インデックス取る
				for (int i = 0; i < 4; ++i)
				{
					std::string bName = dv.BoneName[i];
					int boneIndex = -1;
					if (!bName.empty())
					{
						auto it = m_BoneIndexMap.find(bName);
						if (it != m_BoneIndexMap.end())
						{
							boneIndex = it->second;
						}
					}
					vs.BoneIndex[i] = (boneIndex >= 0) ? boneIndex : 0;
					vs.BoneWeight[i] = dv.BoneWeight[i];

				}
				vertexData[v] = vs;
			}
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(VERTEX_3D_SKIN) * mesh->mNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = vertexData;

			HRESULT hr = Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer[m]);
			assert(SUCCEEDED(hr));

			delete[] vertexData;
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

}

void AnimationModelResource::LoadAnimation(const char* FileName, const char* Name)
{
	m_Animation[Name] = aiImportFile(FileName, aiProcess_ConvertToLeftHanded);
	assert(m_Animation[Name]);
}

void AnimationModelResource::Uninit()
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
		if (pair.second)pair.second->Release();
	}

	aiReleaseImport(m_AiScene);

	if (m_Importer)delete m_Importer;
}


void AnimationModelResource::CreateBone(aiNode* node)
{

	std::string boneName = node->mName.C_Str();

	if (m_BoneIndexMap.find(boneName) == m_BoneIndexMap.end())
	{
		int index = static_cast<int>(m_Bones.size());
		m_BoneIndexMap[boneName] = index;
		m_Bones.emplace_back();
	}
	for (unsigned int n = 0; n < node->mNumChildren; ++n)
	{
		CreateBone(node->mChildren[n]);
	}
}



XMMATRIX AnimationModelResource::TransformToXMMATRIX(aiMatrix4x4& world)
{
	return XMMATRIX(
		world.a1, world.b1, world.c1, world.d1,
		world.a2, world.b2, world.c2, world.d2,
		world.a3, world.b3, world.c3, world.d3,
		world.a4, world.b4, world.c4, world.d4
	);
}

XMMATRIX AnimationModelResource::GetBoneMatrix(const std::string& BoneName)
{
	auto it = m_BoneIndexMap.find(BoneName);
	if (it != m_BoneIndexMap.end())
	{
		int index = it->second;
		return m_Bones[index].worldMatrix;
	}
	return XMMATRIX{};
}

int AnimationModelResource::GetBoneIndexByName(const std::string& BoneName)
{
	auto it = m_BoneIndexMap.find(BoneName);
	if (it != m_BoneIndexMap.end())
	{
		return it->second;
	}
	return 0;
}


const BONE AnimationModelResource::GetBone(int Index)
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

XMFLOAT3 AnimationModelResource::GetBonePosition(const int BoneIndex, const XMMATRIX& PlayerMatrix)
{
	//	範囲外早期リターン
	if (BoneIndex < 0 || BoneIndex >= m_Bones.size())return XMFLOAT3{};

	XMMATRIX& headMatrix = m_Bones[BoneIndex].localMatrix;

	XMMATRIX worldMatrix = XMMatrixMultiply(headMatrix, PlayerMatrix);

	XMVECTOR trans, rot, scaleVec;
	XMMatrixDecompose(&scaleVec, &rot, &trans, worldMatrix);

	return XMFLOAT3(XMVectorGetX(trans), XMVectorGetY(trans), XMVectorGetZ(trans));

}






