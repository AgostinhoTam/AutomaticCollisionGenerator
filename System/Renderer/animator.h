#pragma once

//#include "Main\main.h"
#include <unordered_map>
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "assimp/Importer.hpp"
#pragma comment (lib, "assimp-vc143-mt.lib")

constexpr int MAX_BONES = 256;

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

class GameObject;

class Animator
{
private:
	//	���̃f�[�^���Q��
	std::vector<BONE>& m_Bones;
	const std::unordered_map<std::string, int>& m_BoneIndexMap;		//	index�����ύX�s�\�ɂ���

	int m_CurrentFrame = 0;
	int m_NextFrame = 0;
	float m_BlendRatio = 0.0f;
	std::string m_CurrentAnimation = "";
	std::string m_NextAnimation = "";
	bool m_IsTransitioning = false;

public:
	Animator(std::vector<BONE>& Bones, std::unordered_map<std::string, int>& BoneIndexMap) :m_Bones(Bones), m_BoneIndexMap(BoneIndexMap){}
	void Update(aiScene* CurrentAnimation,aiScene* NextAnimation);

	//	�A�j���[�V�����֘A
	void SetNextAnimation(const std::string& AnimationName);
	void SetCurrentAnimation(const std::string& AnimationName) { m_CurrentAnimation = AnimationName; }
	void SetIsTransitioning(const bool flag) { m_IsTransitioning = flag; }
	void SetBlendRatio(const float BlendRatio) { m_BlendRatio = BlendRatio; }
	void SetCurrentAnimationFrame(const unsigned int frame) { m_CurrentFrame = frame; }
	void SetNextAnimationFrame(const unsigned int frame) { m_NextFrame = frame; }
	const std::string& GetCurrentAnimationName() { return m_CurrentAnimation; }
	const std::string& GetNextAnimationName() { return m_NextAnimation; }
	int GetCurrentAnimationFrame() const{ return m_CurrentFrame; }
	int GetNextAnimationFrame() const{ return m_NextFrame; }
	const float GetBlendRatio() const{ return m_BlendRatio; }
	bool GetIsTransitioning()const { return m_IsTransitioning; }
	void AddBlendRatio(const float BlendRatio = 0.1f){ m_BlendRatio += BlendRatio; }
	void AddCurrentAnimationFrame(const unsigned int frame = 1) { m_CurrentFrame += frame; }
	void AddNextAnimationFrame(const unsigned int frame = 1) { m_NextFrame += frame; }

};