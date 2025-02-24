#pragma once

//#include "Main\main.h"
#include <unordered_map>
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "assimp/Importer.hpp"
#pragma comment (lib, "assimp-vc143-mt.lib")

constexpr int MAX_BONES = 256;
//�ό`�㒸�_�\����
struct DEFORM_VERTEX
{
	aiVector3D Position;
	aiVector3D Normal;
	int				BoneNum{};
	std::string		BoneName[4];//�{���̓{�[���C���f�b�N�X�ŊǗ�����ׂ�
	float			BoneWeight[4]{};
};

//�{�[���\����
struct BONE
{
	XMMATRIX	worldMatrix{};
	XMMATRIX	localMatrix{};
	aiMatrix4x4 Matrix;		//	assimp�pmatrix
	aiMatrix4x4 AnimationMatrix;
	aiMatrix4x4 OffsetMatrix;
	XMFLOAT3	HeadPosition{};
	float		Radius{};
};
struct CB_BONES
{
	XMMATRIX BoneMatrices[MAX_BONES];
};

class GameObject;

class AnimationModelResource
{
private:
	const aiScene* m_AiScene = nullptr;

	Assimp::Importer* m_Importer{};

	ID3D11Buffer** m_VertexBuffer{};
	ID3D11Buffer** m_IndexBuffer{};
	ID3D11Buffer*  m_BoneMatricesBuffer{};

	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_Texture;
	std::vector<BONE> m_Bones;
	std::unordered_map<std::string, int> m_BoneIndexMap;	//�{�[���f�[�^
	std::vector<DEFORM_VERTEX>* m_DeformVertex{};//�ό`�㒸�_�f�[�^

	GameObject* m_Owner{};
public:
	void Load( const char *FileName,GameObject* Owner);
	void Uninit();
	void Draw();
	void Update();
	XMMATRIX TransformToXMMATRIX(aiMatrix4x4&);

	
	//	�{�[���֘A
	int GetBoneIndexByName(const std::string& BoneName);
	XMMATRIX GetBoneMatrix(const std::string& BoneName);
	const std::unordered_map<std::string, int>& GetBoneIndexMap() const{ return m_BoneIndexMap; }
	const std::vector<BONE>& GetBones()const { return m_Bones; }
	const BONE GetBone(int Index);
	XMFLOAT3 GetBonePosition(const int BoneIndex,const XMMATRIX& PlayerMatrix);
	const float CalculateCapsuleRadius(const std::string& HeadName,const std::string& TailName);
	const float DistancePointLineSegment(const XMFLOAT3& Point, const XMFLOAT3& Start, const XMFLOAT3& End);
	void UpdateBoneMatrixToGPU();
	void CreateBone(aiNode* node);
	void UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix);
};