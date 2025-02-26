#pragma once

//#include "Main\main.h"
#include <unordered_map>
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "assimp/Importer.hpp"
#include "System\Enum\modelEnum.h"
#include "System\Renderer\animationModelResource.h"
#pragma comment (lib, "assimp-vc143-mt.lib")


struct CB_BONES
{
	XMMATRIX BoneMatrices[MAX_BONES];
};

class GameObject;
class AnimationModelResource;
class Animator;
class AnimationModelInstance
{
private:
	//	モデル
	ID3D11Buffer* m_BoneMatricesBuffer{};
	AnimationModelResource* m_AnimationResource{};
	Animator* m_Animator;
	std::vector<BONE> m_Bones;
	std::unordered_map<std::string, int> m_BoneIndexMap;	//ボーンデータ
	std::vector<DEFORM_VERTEX>* m_DeformVertex{};//変形後頂点データ
	GameObject* m_Owner{};
	bool m_IsDebugMode = false;
public:
	AnimationModelInstance(const MODEL_NAME& Model,GameObject* Owner);
	void Uninit();
	void Draw();
	void Update();
	XMMATRIX TransformToXMMATRIX(aiMatrix4x4&);
	
	//	ボーン関連
	const std::unordered_map<std::string, int>& GetBoneIndexMap() const{ return m_BoneIndexMap; }
	const std::vector<BONE>& GetBones()const { return m_Bones; }
	const BONE GetBone(int Index);
	XMFLOAT3 GetBonePosition(const int BoneIndex,const XMMATRIX& PlayerMatrix);
	const float CalculateCapsuleRadius(const std::string& HeadName,const std::string& TailName);
	const float DistancePointLineSegment(const XMFLOAT3& Point, const XMFLOAT3& Start, const XMFLOAT3& End);
	void UpdateBoneMatrixToGPU();
	void UpdateBoneMatrix(const aiNode* node, const aiMatrix4x4& matrix);

	//	アニメーション関連
	Animator*& GetAnimator() { return m_Animator; }
	void SetNextAnimation(const std::string& AnimationName);
	const bool GetIsTransitioning()const;
	void SetNextAnimationFrame(unsigned int Frame);
};