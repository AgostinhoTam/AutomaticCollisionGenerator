
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>


#include "Main/main.h"
#include "System\Renderer\animationModel.h"
#include "Manager/animationModelsResource.h"



void AnimationModelsResource::UnloadAll()
{
	for (std::pair<MODEL_NAME, AnimationModel*> pair : m_AnimationModelPool)
	{
		pair.second->Uninit();
		delete pair.second;
	}
	m_AnimationModelPool.clear();
}

AnimationModel* AnimationModelsResource::LoadAnimationModel(const MODEL_NAME& Model)
{
	if (m_AnimationModelPool.find(Model) != m_AnimationModelPool.end())
	{
		return m_AnimationModelPool[Model];
	}

	AnimationModel* animationModel = new AnimationModel;
	if (!animationModel)return nullptr;
	switch (Model)
	{
	case MODEL_NAME::PLAYER:
		Load("asset\\model\\player.fbx");
		animationModel->LoadAnimation("asset\\model\\player_Idle.fbx", "Idle");
		animationModel->LoadAnimation("asset\\model\\player_Run.fbx", "Run");
		m_AnimationModelPool.try_emplace(Model, animationModel);
		break;
	case MODEL_NAME::ENEMY:
		Load("asset\\model\\enemy.fbx");
		animationModel->LoadAnimation("asset\\model\\enemy_Idle.fbx","Idle");
		animationModel->LoadAnimation("asset\\model\\enemy_Run.fbx","Run");

		animationModel->LoadAnimation("asset\\model\\enemy_Kick.fbx","Kick");
		m_AnimationModelPool.try_emplace(Model, animationModel);
		break;
	case MODEL_NAME::MONSTER:
		Load("asset\\model\\monster.fbx");
		animationModel->LoadAnimation("asset\\model\\monster_Idle.fbx", "Idle");
		animationModel->LoadAnimation("asset\\model\\monster_Run.fbx", "Run");
		animationModel->LoadAnimation("asset\\model\\monster_Attack.fbx", "Attack");
		m_AnimationModelPool.try_emplace(Model, animationModel);
		break;
	default:
		Load("asset\\model\\default.fbx");
		break;
	}
	return m_AnimationModelPool[Model];
}

//	���f���ǂݍ���
void AnimationModelsResource::Load(const char* FileName)
{
	m_Importer = new Assimp::Importer;

	m_Importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

	const std::string modelPath(FileName);

	m_AiScene = m_Importer->ReadFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	assert(m_AiScene);

	m_VertexBuffer = new ID3D11Buffer * [m_AiScene->mNumMeshes];
	m_IndexBuffer = new ID3D11Buffer * [m_AiScene->mNumMeshes];

	m_Bones.clear();
	m_BoneIndexMap.clear();

	//�ό`�㒸�_�z�񐶐�
	std::vector<DEFORM_VERTEX>* DeformVertex = new std::vector<DEFORM_VERTEX>[m_AiScene->mNumMeshes];

	//	�ċA�I�Ƀ{�[������
	CreateBone(m_AiScene->mRootNode);

	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; ++m)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];

		DeformVertex[m].resize(mesh->mNumVertices);

		//	���_��񂾂��L�^
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
			DeformVertex[m][v] = dv;
		}


		//	���b�V���Ɋ܂߂�{�[�����
		for (unsigned int b = 0; b < mesh->mNumBones; ++b)
		{
			aiBone* bone = mesh->mBones[b];	//�@�{�[�����o��
			if (!bone)continue;
			std::string boneName = bone->mName.C_Str();
			if (boneName.length() == 0)continue;	//	���O������ۂ������玟��

			int newIndex;

			auto it = m_BoneIndexMap.find(boneName);

			//	�����{�[�����܂����݂��Ȃ��Ȃ�쐬
			if (it == m_BoneIndexMap.end())
			{
				newIndex = static_cast<int>(m_Bones.size());	//	�{�[���ɃC���f�b�N�X�t����

				BONE newBone;
				newBone.OffsetMatrix = bone->mOffsetMatrix;
				m_Bones.emplace_back(newBone);
				m_BoneIndexMap[boneName] = newIndex;				//	�{�[�����O�ƃC���f�b�N�X��map
			}
			else
			{

				m_Bones[it->second].OffsetMatrix = bone->mOffsetMatrix;
			}


			//	�e���_���Ƃ�Weight���Ǘ�
			for (unsigned int w = 0; w < bone->mNumWeights; ++w)
			{
				aiVertexWeight vw = bone->mWeights[w];	//	bone weight���o��
				int vID = vw.mVertexId;
				float wgt = vw.mWeight;

				DEFORM_VERTEX& dv = DeformVertex[m][vID];		//�@�V�[���̉��Ԗڒ��_

				//	�{�[���̐���4�ȉ���������
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

					//	4�{�̃{�[������
					for (int i = 1; i < 4; ++i)
					{
						if (dv.BoneWeight[i] < minWeight)
						{
							minWeight = dv.BoneWeight[i];
							minIndex = i;
						}
					}

					//�@�������
					if (wgt > minWeight)
					{
						dv.BoneName[minIndex] = boneName;
						dv.BoneWeight[minIndex] = wgt;
					}
				}
			}
		}

		//	���_�o�b�t�@����
		{
			auto* vertexData = new VERTEX_3D_SKIN[mesh->mNumVertices];

			for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
			{
				DEFORM_VERTEX& dv = DeformVertex[m][v];
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

				//	�C���f�b�N�X���
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
		// �C���f�b�N�X�o�b�t�@����
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

	//�e�N�X�`���ǂݍ���
	for (unsigned int i = 0; i < m_AiScene->mNumTextures; i++)
	{
		aiTexture* aitexture = m_AiScene->mTextures[i];
		if (aitexture->mHeight == 0)
		{

			ID3D11ShaderResourceView* texture;

			// �e�N�X�`���ǂݍ���
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

	//	�{�[���̒萔�o�b�t�@�쐬
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

//	�A�j���[�V�����f�[�^�ǂݍ���
void AnimationModelsResource::LoadAnimation(const char* FileName, const char* Name)
{
	m_Animation[Name] = aiImportFile(FileName, aiProcess_ConvertToLeftHanded);
	assert(m_Animation[Name]);
}

const BONE AnimationModelsResource::GetBone(int Index)
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

XMFLOAT3 AnimationModelsResource::GetBonePosition(const int BoneIndex, const XMMATRIX& PlayerMatrix)
{
	//	�͈͊O�������^�[��
	if (BoneIndex < 0 || BoneIndex >= m_Bones.size())return XMFLOAT3{};

	XMMATRIX& headMatrix = m_Bones[BoneIndex].localMatrix;

	XMMATRIX worldMatrix = XMMatrixMultiply(headMatrix, PlayerMatrix);

	XMVECTOR trans, rot, scaleVec;
	XMMatrixDecompose(&scaleVec, &rot, &trans, worldMatrix);

	return XMFLOAT3(XMVectorGetX(trans), XMVectorGetY(trans), XMVectorGetZ(trans));
}

const float AnimationModelsResource::CalculateCapsuleRadius(const std::string& HeadName, const std::string& TailName)
{
	auto headit = m_BoneIndexMap.find(HeadName);
	auto tailit = m_BoneIndexMap.find(TailName);

	if (headit == m_BoneIndexMap.end() || tailit == m_BoneIndexMap.end())
	{
		return 0.0f;
	}

	int headIndex = headit->second;
	int tailIndex = tailit->second;

	// Head��Tail�̈ʒu�����b�V����ԂŎ擾
	aiMatrix4x4 offsetHeadInv = m_Bones[headIndex].OffsetMatrix;
	offsetHeadInv.Inverse();
	aiVector3D headPosMesh = offsetHeadInv * aiVector3D(0.0f, 0.0f, 0.0f);

	aiMatrix4x4 offsetTailInv = m_Bones[tailIndex].OffsetMatrix;
	offsetTailInv.Inverse();
	aiVector3D tailPosMesh = offsetTailInv * aiVector3D(0.0f, 0.0f, 0.0f);

	// ���� [S, E] �����b�V����ԂŒ�`
	XMFLOAT3 S(headPosMesh.x, headPosMesh.y, headPosMesh.z);
	XMFLOAT3 E(tailPosMesh.x, tailPosMesh.y, tailPosMesh.z);

	// 2) ���_�����b�V����ԂŒ��ڌv�Z
	float maxDistSq = 0.0f;
	std::vector<float> radiusList;

	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; ++m)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			aiVector3D bindPos = mesh->mVertices[v]; // ���b�V����Ԃ̒��_

			DEFORM_VERTEX& deformVertex = m_DeformVertex[m][v];
			int boneCount = deformVertex.BoneNum;

			for (int i = 0; i < boneCount; i++)
			{
				const std::string& boneName = deformVertex.BoneName[i];

				// ���_��Head�܂���Tail�{�[���̉e�����󂯂Ă���ꍇ�̂݌v�Z
				if (boneName != HeadName && boneName != TailName)
					continue;

				// ���b�V����Ԃ̒��_�ʒu���g�p
				XMFLOAT3 p(bindPos.x, bindPos.y, bindPos.z);

				// �_�Ɛ����̍ŒZ����
				float dist = DistancePointLineSegment(p, S, E);
				radiusList.emplace_back(dist);
			}
		}
	}

	if (radiusList.empty()) return 0.0f;

	// �����l�𔼌a�Ƃ��Ďg�p
	size_t size = radiusList.size();
	std::nth_element(radiusList.begin(), radiusList.begin() + size / 2, radiusList.end());
	float median = radiusList[size / 2];

	return median;
}

const float AnimationModelsResource::DistancePointLineSegment(const XMFLOAT3& Point, const XMFLOAT3& Start, const XMFLOAT3& End)
{
	XMVECTOR vP = XMLoadFloat3(&Point);
	XMVECTOR vS = XMLoadFloat3(&Start);
	XMVECTOR vE = XMLoadFloat3(&End);

	XMVECTOR vSE = XMVectorSubtract(vE, vS); // �����x�N�g�� E - S
	XMVECTOR vSP = XMVectorSubtract(vP, vS); // p - s

	// SE �̒���^2
	float segLenSq = XMVectorGetX(XMVector3Dot(vSE, vSE));
	if (segLenSq < 1e-6f)
	{
		// S �� E ���قړ������W�Ȃ�AS�Ƃ̋�����Ԃ�
		return XMVectorGetX(XMVector3Length(vSP));
	}

	// �_P��SE�Ɏˉe�����Ƃ��̃p�����[�^ t (0 <= t <= 1)
	float dot = XMVectorGetX(XMVector3Dot(vSP, vSE));
	float t = dot / segLenSq;

	// �����͈͓��ɃN�����v
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;

	// �ŋߐړ_ C = S + t*(E - S)
	XMVECTOR vClosest = XMVectorAdd(vS, XMVectorScale(vSE, t));

	// P-C �̋���
	XMVECTOR vDiff = XMVectorSubtract(vP, vClosest);
	float dist = XMVectorGetX(XMVector3Length(vDiff));
	return dist;
}

void AnimationModelsResource::CreateBone(aiNode* node)
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
