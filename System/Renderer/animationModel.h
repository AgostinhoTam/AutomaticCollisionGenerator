#pragma once

//#include "Main\main.h"
#include <unordered_map>
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#pragma comment (lib, "assimp-vc143-mt.lib")
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
	aiMatrix4x4 Matrix;
	aiMatrix4x4 AnimationMatrix;
	aiMatrix4x4 OffsetMatrix;
	XMFLOAT3	HeadPosition{};
	float		Radius{};
};

class GameObject;

class AnimationModel
{
private:
	const aiScene* m_AiScene = nullptr;
	std::unordered_map<std::string, const aiScene*> m_Animation;

	ID3D11Buffer** m_VertexBuffer{};
	ID3D11Buffer** m_IndexBuffer{};
	ID3D11Buffer* m_BoneConstantBuffer{};

	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_Texture;
	std::unordered_map<std::string, int> m_BoneIndexMap;
	std::vector<BONE> m_Bones;
	

	std::vector<DEFORM_VERTEX>* m_DeformVertex{};//変形後頂点データ
	int m_CurrentFrame = 0;
	int m_NextFrame = 0;
	float m_BlendRatio = 0.0f;
	std::string m_CurrentAnimation = "Idle";
	std::string m_NextAnimation = "Idle";
	bool m_IsTransitioning = false;
	bool m_IsDebugMode = false;
public:
	void Load( const char *FileName);
	void Uninit();
	void Draw(GameObject* Object);
	void Update();
	void LoadAnimation(const char* FileName, const char* Name);
	void CreateBone(aiNode* node);
	void UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix);
	XMMATRIX TransformToXMMATRIX(aiMatrix4x4&);
	void SetNextAnimation(const std::string& AnimationName);
	void SetCurrentAnimation(const std::string& AnimationName) { m_CurrentAnimation = AnimationName; }
	void SetIsTransitioning(const bool flag) { m_IsTransitioning = flag; }
	void SetBlendRatio(const float BlendRatio) { m_BlendRatio = BlendRatio; }
	void SetCurrentAnimationFrame(const unsigned int frame) { m_CurrentFrame = frame; }
	void SetNextAnimationFrame(const unsigned int frame) { m_NextFrame = frame; }
	XMMATRIX GetBoneMatrix(std::string);
	double GetAnimationDuration(const std::string& AnimationName);
	const std::string& GetCurrentAnimationName() { return m_CurrentAnimation; }
	const std::string& GetNextAnimationName() { return m_NextAnimation; }
	int GetCurrentAnimationFrame() const{ return m_CurrentFrame; }
	int GetNextAnimationFrame() const{ return m_NextFrame; }
	bool GetIsTransitioning()const { return m_IsTransitioning; }
	const float GetBlendRatio() const{ return m_BlendRatio; }
	void AddBlendRatio(const float BlendRatio = 0.1f){ m_BlendRatio += BlendRatio; }
	void AddCurrentAnimationFrame(const unsigned int frame = 1) { m_CurrentFrame += frame; }
	void AddNextAnimationFrame(const unsigned int frame = 1) { m_NextFrame += frame; }
	std::unordered_map<std::string, int>& GetBoneMap() { return m_BoneIndexMap; }
	XMFLOAT3& GetHeadPosition(const std::string& BoneName,const XMFLOAT3& Scale,const XMMATRIX& PlayerMatrix);
	void UpdateAnimationBlend();
	const float CalculateCapsuleRadius(const std::string& HeadName,const std::string& TailName);
	const float DistancePointLineSegment(const XMFLOAT3& Point, const XMFLOAT3& Start, const XMFLOAT3& End);
};