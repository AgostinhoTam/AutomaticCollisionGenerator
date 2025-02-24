#pragma once
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "assimp/Importer.hpp"
#pragma comment (lib, "assimp-vc143-mt.lib")

#include "System\Enum\modelEnum.h"
#include "System\Renderer/renderer.h"
#include <string>
#include <unordered_map>

constexpr int MAX_BONES = 256;
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
struct CB_BONES
{
	XMMATRIX BoneMatrices[MAX_BONES];
};

class AnimationModelsResource
{
private:
	Assimp::Importer* m_Importer{};
	const aiScene* m_AiScene{};
	std::unordered_map<std::string, const aiScene*> m_Animation;

	ID3D11Buffer** m_VertexBuffer;
	ID3D11Buffer** m_IndexBuffer;
	ID3D11Buffer* m_BoneMatricesBuffer;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_Texture;
	std::vector<BONE> m_Bones;
	std::unordered_map<std::string, int> m_BoneIndexMap;	//ボーンデータ

	bool m_IsDebugMode = false;
public:
	void UnloadAll();

	AnimationModel* LoadAnimationModel(const MODEL_NAME& Model);

	void Load(const char* FileName);
	void Uninit();
	void Draw();
	void Update();
	XMMATRIX TransformToXMMATRIX(aiMatrix4x4&);

	//	アニメーション関連
	void LoadAnimation(const char* FileName, const char* Name);

	//	ボーン関連
	int GetBoneIndexByName(const std::string& BoneName);
	XMMATRIX GetBoneMatrix(std::string);
	const std::unordered_map<std::string, int>& GetBoneIndexMap() const { return m_BoneIndexMap; }
	const std::vector<BONE>& GetBones()const { return m_Bones; }
	const BONE GetBone(int Index);
	XMFLOAT3 GetBonePosition(const int BoneIndex, const XMMATRIX& PlayerMatrix);
	const float CalculateCapsuleRadius(const std::string& HeadName, const std::string& TailName);
	const float DistancePointLineSegment(const XMFLOAT3& Point, const XMFLOAT3& Start, const XMFLOAT3& End);
	void UpdateBoneMatrixToGPU();
	void CreateBone(aiNode* node);
	void UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix);

};