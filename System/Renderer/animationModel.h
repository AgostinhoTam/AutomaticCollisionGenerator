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

class GameObject;

class AnimationModel
{
private:
	const aiScene* m_AiScene = nullptr;

	Assimp::Importer* m_Importer{};
	std::unordered_map<std::string, const aiScene*> m_Animation;

	ID3D11Buffer** m_VertexBuffer{};
	ID3D11Buffer** m_IndexBuffer{};
	ID3D11Buffer*  m_BoneMatricesBuffer{};

	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_Texture;
	std::vector<BONE> m_Bones;
	std::unordered_map<std::string, int> m_BoneIndexMap;	//ボーンデータ

	std::vector<DEFORM_VERTEX>* m_DeformVertex{};//変形後頂点データ	todo
	int m_CurrentFrame = 0;
	int m_NextFrame = 0;
	float m_BlendRatio = 0.0f;
	std::string m_CurrentAnimation = "Idle";
	std::string m_NextAnimation = "Idle";
	bool m_IsTransitioning = false;
	bool m_IsDebugMode = false;
	GameObject* m_Owner{};
public:
	void Load( const char *FileName,GameObject* Owner);
	void Uninit();
	void Draw();
	void Update();
	XMMATRIX TransformToXMMATRIX(aiMatrix4x4&);

	//	アニメーション関連
	void LoadAnimation(const char* FileName, const char* Name);
	void UpdateAnimationBlend();
	void SetNextAnimation(const std::string& AnimationName);
	void SetCurrentAnimation(const std::string& AnimationName) { m_CurrentAnimation = AnimationName; }
	void SetIsTransitioning(const bool flag) { m_IsTransitioning = flag; }
	void SetBlendRatio(const float BlendRatio) { m_BlendRatio = BlendRatio; }
	void SetCurrentAnimationFrame(const unsigned int frame) { m_CurrentFrame = frame; }
	void SetNextAnimationFrame(const unsigned int frame) { m_NextFrame = frame; }
	double GetAnimationDuration(const std::string& AnimationName);
	const std::string& GetCurrentAnimationName() { return m_CurrentAnimation; }
	const std::string& GetNextAnimationName() { return m_NextAnimation; }
	int GetCurrentAnimationFrame() const{ return m_CurrentFrame; }
	int GetNextAnimationFrame() const{ return m_NextFrame; }
	XMMATRIX GetBoneMatrix(const std::string& BoneName);
	const float GetBlendRatio() const{ return m_BlendRatio; }
	bool GetIsTransitioning()const { return m_IsTransitioning; }
	int GetBoneIndexByName(const std::string& BoneName);
	void AddBlendRatio(const float BlendRatio = 0.1f){ m_BlendRatio += BlendRatio; }
	void AddCurrentAnimationFrame(const unsigned int frame = 1) { m_CurrentFrame += frame; }
	void AddNextAnimationFrame(const unsigned int frame = 1) { m_NextFrame += frame; }
	
	//	ボーン関連
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