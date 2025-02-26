#pragma once

//#include "Main\main.h"
#include <unordered_map>
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "assimp/Importer.hpp"
#pragma comment (lib, "assimp-vc143-mt.lib")
#include "System\Renderer\renderer.h"

//変形後頂点構造体
struct DEFORM_VERTEX
{
	aiVector3D Position;
	aiVector3D Normal;
	int				BoneNum{};
	std::string		BoneName[4];//本来はボーンインデックスで管理するべき
	float			BoneWeight[4]{};
};

//ボーン構造体
struct BONE
{
	XMMATRIX	worldMatrix{};
	XMMATRIX	localMatrix{};
	aiMatrix4x4 Matrix;		//	assimp用matrix
	aiMatrix4x4 AnimationMatrix;
	aiMatrix4x4 OffsetMatrix;
	XMFLOAT3	HeadPosition{};
	float		Radius{};
};


class AnimationModelResource
{
private:
	const aiScene* m_AiScene = nullptr;
	Assimp::Importer* m_Importer{};
	std::unordered_map<std::string, const aiScene*> m_Animation;
	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_Texture;
	std::vector<BONE> m_Bones;
	std::unordered_map<std::string, int> m_BoneIndexMap;	//ボーンデータ
	std::vector<DEFORM_VERTEX>* m_DeformVertex{};//変形後頂点データ
	ID3D11Buffer** m_VertexBuffer{};
	ID3D11Buffer** m_IndexBuffer{};

public:
	void Load( const char *FileName);
	void LoadAnimation(const char* FileName, const char* Name);
	void Uninit();
	XMMATRIX TransformToXMMATRIX(aiMatrix4x4&);

	//	ボーン関連
	const aiScene* GetAiScene() { return m_AiScene; }
	int GetBoneIndexByName(const std::string& BoneName);
	XMMATRIX GetBoneMatrix(const std::string& BoneName);
	const std::unordered_map<std::string, int>& GetBoneIndexMap() const{ return m_BoneIndexMap; }
	const std::vector<BONE>& GetBones()const { return m_Bones; }
	const BONE GetBone(int Index);
	XMFLOAT3 GetBonePosition(const int BoneIndex,const XMMATRIX& PlayerMatrix);
	ID3D11Buffer** GetVertexBUffer() { return m_VertexBuffer; }
	ID3D11Buffer** GetIndexBUffer() { return m_VertexBuffer; }
	const std::unordered_map<std::string, const aiScene*>& GetAnimationMap() { return m_Animation; }
	void CreateBone(aiNode* node);
	const std::unordered_map<std::string, ID3D11ShaderResourceView*>& GetTexture() { return m_Texture; }
};