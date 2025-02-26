#include <numeric> 
#include "Main\main.h"
#include "Manager\inputManager.h"
#include "GameObject\gameobject.h"
#include "assimp\Importer.hpp"
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
	//	�A�j���[�V�����f�[�^����
	aiAnimation* animation1 = m_Animation[m_CurrentAnimation]->mAnimations[0];
	aiAnimation* animation2 = m_Animation[m_NextAnimation]->mAnimations[0];

	for (auto& kv : m_BoneIndexMap)
	{
		const std::string& boneName = kv.first;

		int index = kv.second;

		BONE& bone = m_Bones[index];

		aiNodeAnim* nodeAnim1 = nullptr;
		aiNodeAnim* nodeAnim2 = nullptr;

		for (unsigned int c = 0; c < animation1->mNumChannels; ++c)
		{
			if (animation1->mChannels[c]->mNodeName == aiString(boneName))
			{
				nodeAnim1 = animation1->mChannels[c];
				break;
			}
		}
		for (unsigned int c = 0; c < animation2->mNumChannels; ++c)
		{
			if (animation2->mChannels[c]->mNodeName == aiString(boneName))
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
			f = m_CurrentFrame % nodeAnim1->mNumRotationKeys;	//�ȈՎ���
			rot1 = nodeAnim1->mRotationKeys[f].mValue;
			f = m_CurrentFrame % nodeAnim1->mNumPositionKeys;	//�ȈՎ���
			pos1 = nodeAnim1->mPositionKeys[f].mValue;
		}
		if (nodeAnim2)
		{
			f = m_NextFrame % nodeAnim2->mNumRotationKeys;	//�ȈՎ���
			rot2 = nodeAnim2->mRotationKeys[f].mValue;
			f = m_NextFrame % nodeAnim2->mNumPositionKeys;	//�ȈՎ���
			pos2 = nodeAnim2->mPositionKeys[f].mValue;
		}
		aiVector3D pos;
		aiQuaternion rot;
		pos = pos1 * (1.0f - m_BlendRatio) + pos2 * m_BlendRatio;	//���`���
		aiQuaternion::Interpolate(rot, rot1, rot2, m_BlendRatio);	//���ʐ��`���

		bone.AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rot, pos);
	}

	//	�ċA�I�Ƀ{�[��matrix
	aiMatrix4x4 rootMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f),
		aiQuaternion((float)AI_MATH_PI, 0.0f, 0.0f),
		aiVector3D(0.0f, 0.0f, 0.0f));
	UpdateBoneMatrix(m_AiScene->mRootNode, rootMatrix);

	UpdateBoneMatrixToGPU();

}

void AnimationModel::Draw()
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

	// �v���~�e�B�u�g�|���W�ݒ�
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	UpdateBoneMatrixToGPU();
	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];


		// �}�e���A���ݒ�
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


		// ���_�o�b�t�@�ݒ�
		UINT stride = sizeof(VERTEX_3D_SKIN);
		UINT offset = 0;
		Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer[m], &stride, &offset);

		// �C���f�b�N�X�o�b�t�@�ݒ�
		Renderer::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// �|���S���`��
		Renderer::GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
	}
	Renderer::SetDepthEnable(true);
	Renderer::SetBlendState(BLEND_MODE::BLEND_MODE_NONE);
}

void AnimationModel::Load(const char* FileName, GameObject* Owner)
{
	if (!Owner)return;
	m_Owner = Owner;

	const std::string modelPath(FileName);
	m_Importer = new Assimp::Importer;

	m_Importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

	m_AiScene = m_Importer->ReadFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	assert(m_AiScene);

	m_VertexBuffer = new ID3D11Buffer * [m_AiScene->mNumMeshes];
	m_IndexBuffer = new ID3D11Buffer * [m_AiScene->mNumMeshes];

	m_Bones.clear();
	m_BoneIndexMap.clear();

	//�ό`�㒸�_�z�񐶐�
	m_DeformVertex = new std::vector<DEFORM_VERTEX>[m_AiScene->mNumMeshes];

	//	�ċA�I�Ƀ{�[������
	CreateBone(m_AiScene->mRootNode);

	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; ++m)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];

		m_DeformVertex[m].resize(mesh->mNumVertices);

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
				dv.BoneIndex[i] = -1;
				dv.BoneWeight[i] = 0.0f;
			}
			m_DeformVertex[m][v] = dv;
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
				
				DEFORM_VERTEX& dv = m_DeformVertex[m][vID];		//�@�V�[���̉��Ԗڒ��_

				//	�{�[���̐���4�ȉ���������
				if (dv.BoneNum < 4)
				{
					if (dv.BoneNum == 3)
					{
						getchar();
					}
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
		if (pair.second)pair.second->Release();
	}

	aiReleaseImport(m_AiScene);

	for (std::pair<const std::string, const aiScene*> pair : m_Animation)
	{
		aiReleaseImport(pair.second);
	}

	if (m_BoneMatricesBuffer)m_BoneMatricesBuffer->Release();

}

void AnimationModel::LoadAnimation(const char* FileName, const char* Name)
{
	m_Animation[Name] = aiImportFile(FileName, aiProcess_ConvertToLeftHanded);
	assert(m_Animation[Name]);
}

void AnimationModel::CreateBone(aiNode* node)
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

void AnimationModel::UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix)
{
	std::string bName = node->mName.C_Str();
	aiMatrix4x4 localMatrix{};
	auto it = m_BoneIndexMap.find(bName);
	if (it != m_BoneIndexMap.end())
	{
		int index = it->second;

		BONE* bone = &m_Bones[index];

		aiMatrix4x4 worldMatrix = matrix * bone->AnimationMatrix;		//	�e����̃}�g���N�X�Ǝ��M�̃A�j���[�V�����ϊ�
		bone->localMatrix = TransformToXMMATRIX(worldMatrix);		//	DX�p���[�J���}�g���N�X

		// **�X�L�j���O�p�̍ŏI�s�� (���[���h�s�� �~ �I�t�Z�b�g�s��)**
		bone->Matrix = worldMatrix * bone->OffsetMatrix;			//	�e���_�ɓK�p����}�g���N�X

		// **DirectX�p�̍s��ɕϊ�**
		bone->worldMatrix = TransformToXMMATRIX(bone->Matrix);		//	DX�p�ɕϊ�

		localMatrix = worldMatrix;									//	�A�j���[�V�����K�p��̃}�g���N�X�����ɓn������

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

XMMATRIX AnimationModel::TransformToXMMATRIX(aiMatrix4x4& world)
{
	return XMMATRIX(
		world.a1, world.b1, world.c1, world.d1,
		world.a2, world.b2, world.c2, world.d2,
		world.a3, world.b3, world.c3, world.d3,
		world.a4, world.b4, world.c4, world.d4
	);
}

XMMATRIX AnimationModel::GetBoneMatrix(const std::string& BoneName)
{
	auto it = m_BoneIndexMap.find(BoneName);
	if (it != m_BoneIndexMap.end())
	{
		int index = it->second;
		return m_Bones[index].worldMatrix;
	}
	return XMMATRIX{};
}

int AnimationModel::GetBoneIndexByName(const std::string& BoneName)
{
	auto it = m_BoneIndexMap.find(BoneName);
	if (it != m_BoneIndexMap.end())
	{
		return it->second;
	}
	return 0;
}

double AnimationModel::GetAnimationDuration(const std::string& AnimationName)
{
	if (m_Animation.count(AnimationName) == 0)return 0;
	if (!m_Animation[AnimationName]->HasAnimations())return 0;
	return (m_Animation[AnimationName]->mAnimations[0]->mDuration);
}

const BONE AnimationModel::GetBone(int Index)
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

XMFLOAT3 AnimationModel::GetBonePosition(const int BoneIndex, const XMMATRIX& PlayerMatrix)
{
	//	�͈͊O�������^�[��
	if (BoneIndex < 0 || BoneIndex >= m_Bones.size())return XMFLOAT3{};

	XMMATRIX& headMatrix = m_Bones[BoneIndex].localMatrix;

	XMMATRIX worldMatrix = XMMatrixMultiply(headMatrix, PlayerMatrix);

	XMVECTOR trans, rot, scaleVec;
	XMMatrixDecompose(&scaleVec, &rot, &trans, worldMatrix);

	return XMFLOAT3(XMVectorGetX(trans), XMVectorGetY(trans), XMVectorGetZ(trans));

}

void AnimationModel::UpdateAnimationBlend()
{

	//	�J�ڒ���������
	if (m_IsTransitioning)
	{
		AddBlendRatio();
		AddCurrentAnimationFrame();
		AddNextAnimationFrame();
	}
	//	���ʂ̍Đ�
	else
	{
		AddCurrentAnimationFrame();
	}
	//	�J�ڊ���������
	if (m_BlendRatio >= 1)
	{
		m_IsTransitioning = false;
		SetCurrentAnimation(m_NextAnimation);
		m_CurrentFrame = m_NextFrame;
		m_BlendRatio = 0;
	}
	//	�{�[���A���b�V���X�V
	Update();
}

const float AnimationModel::CalculateCapsuleRadius(const std::string& HeadName, const std::string& TailName)
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

const float AnimationModel::DistancePointLineSegment(const XMFLOAT3& Point, const XMFLOAT3& Start, const XMFLOAT3& End)
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

void AnimationModel::UpdateBoneMatrixToGPU()
{
	CB_BONES* cbBones = new CB_BONES;

	int boneCount = static_cast<int>(m_Bones.size());

	//	�o�b�t�@�ɓ����
	for (int i = 0; i < boneCount; ++i)
	{
		cbBones->BoneMatrices[i] = m_Bones[i].worldMatrix;
		cbBones->BoneMatrices[i] = XMMatrixTranspose(cbBones->BoneMatrices[i]);

	}
	//	�c��̓��Z�b�g
	for (int i = boneCount; i < MAX_BONES; ++i)
	{
		cbBones->BoneMatrices[i] = XMMatrixIdentity();
	}

	// �萔�o�b�t�@�X�V
	D3D11_MAPPED_SUBRESOURCE ms;
	Renderer::GetDeviceContext()->Map(m_BoneMatricesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, cbBones, sizeof(CB_BONES));
	Renderer::GetDeviceContext()->Unmap(m_BoneMatricesBuffer, 0);

	// VS�ɃZ�b�g
	Renderer::GetDeviceContext()->VSSetConstantBuffers(6, 1, &m_BoneMatricesBuffer);

	if(cbBones)delete cbBones;
}

void AnimationModel::SetNextAnimation(const std::string& AnimationName)
{
	m_NextAnimation = AnimationName;
	m_IsTransitioning = true;
	m_NextFrame = 0;
}



