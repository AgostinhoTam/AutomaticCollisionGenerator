#include "Main\main.h"
#include "GameObject\gameobject.h"
#include "Manager\inputManager.h"
#include "System\Renderer\renderer.h"
#include "System\Renderer\animationModel.h"

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

	// �v���~�e�B�u�g�|���W�ݒ�
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::SetMaterial(material);

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
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer[m], &stride, &offset);

		// �C���f�b�N�X�o�b�t�@�ݒ�
		Renderer::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// �|���S���`��
		Renderer::GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
	}
}

void AnimationModel::Load(const char* FileName)
{
	const std::string modelPath(FileName);

	m_AiScene = aiImportFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	assert(m_AiScene);

	m_VertexBuffer = new ID3D11Buffer * [m_AiScene->mNumMeshes];
	m_IndexBuffer = new ID3D11Buffer * [m_AiScene->mNumMeshes];


	//�ό`�㒸�_�z�񐶐�
	m_DeformVertex = new std::vector<DEFORM_VERTEX>[m_AiScene->mNumMeshes];

	//	�ċA�I�Ƀ{�[������
	CreateBone(m_AiScene->mRootNode);


	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];

		// ���_�o�b�t�@����
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



		//�ό`�㒸�_�f�[�^������
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


		//�{�[���f�[�^������
		for (unsigned int b = 0; b < mesh->mNumBones; b++)
		{
			aiBone* bone = mesh->mBones[b];

			m_Bone[bone->mName.C_Str()].OffsetMatrix = bone->mOffsetMatrix;

			//�ό`�㒸�_�Ƀ{�[���f�[�^�i�[
			for (unsigned int w = 0; w < bone->mNumWeights; w++)
			{
				aiVertexWeight weight = bone->mWeights[w];

				int num = m_DeformVertex[m][weight.mVertexId].BoneNum;

				m_DeformVertex[m][weight.mVertexId].BoneWeight[num] = weight.mWeight;
				m_DeformVertex[m][weight.mVertexId].BoneName[num] = bone->mName.C_Str();
				m_DeformVertex[m][weight.mVertexId].BoneNum++;

				assert(m_DeformVertex[m][weight.mVertexId].BoneNum <= 4);
			}
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
			LoadFromWICMemory(aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);	//	�������ɕۑ��ł���
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

	//	�}�g���N�X�̏�Z���Ԃɒ���
	aiMatrix4x4 worldMatrix{};

	worldMatrix *= matrix;		//�e�̃}�g���N�X
	worldMatrix *= bone->AnimationMatrix;	//�����̃}�g���N�X

	bone->worldMatrix = TransformToXMMATRIX(worldMatrix);


	bone->Matrix = worldMatrix;
	bone->Matrix *= bone->OffsetMatrix;

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

void AnimationModel::SetNextAnimation(const std::string& AnimationName)
{
	m_NextAnimation = AnimationName;
	m_IsTransitioning = true;
	m_NextFrame = 0;

}

void AnimationModel::Update()
{
	if (m_Animation.count(m_CurrentAnimation) == 0)return;
	if (!m_Animation[m_CurrentAnimation]->HasAnimations())return;
	if (m_Animation.count(m_NextAnimation) == 0)return;
	if (!m_Animation[m_NextAnimation]->HasAnimations())return;


	//	�A�j���[�V�����f�[�^����
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

		bone->AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rot, pos);
	}

	//	�ċA�I�Ƀ{�[��matrix
	aiMatrix4x4 rootMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f),
		aiQuaternion((float)AI_MATH_PI, 0.0f, 0.0f),
		aiVector3D(0.0f, 0.0f, 0.0f));
	UpdateBoneMatrix(m_AiScene->mRootNode, rootMatrix);

	//	���_�ϊ�(CPU�X�L�j���O)
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
			matrix[0] = m_Bone[deformVertex->BoneName[0]].Matrix;	//�֐߂͕����̒��_����Ȃ��ƃ_���A�Ƃ肠����4�ő����
			matrix[1] = m_Bone[deformVertex->BoneName[1]].Matrix;
			matrix[2] = m_Bone[deformVertex->BoneName[2]].Matrix;
			matrix[3] = m_Bone[deformVertex->BoneName[3]].Matrix;

			//	���X�͂���Ōv�Z�ł���Aaimp�Ȃ�����o���o���ɂ���
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

			//	�@���ϊ��p�Ɉړ��������폜
			outMatrix.a4 = 0.0f;
			outMatrix.b4 = 0.0f;
			outMatrix.c4 = 0.0f;

			deformVertex->Normal = mesh->mNormals[v];
			deformVertex->Normal *= outMatrix;

			//�@���_�o�b�t�@�֏�������
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
