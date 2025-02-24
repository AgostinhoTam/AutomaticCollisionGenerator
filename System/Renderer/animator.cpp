#include <numeric> 
#include "Main\main.h"
#include "Manager\inputManager.h"
#include "GameObject\gameobject.h"
#include "assimp\Importer.hpp"
//#include "System\Collision\sphereCollision.h"
#include "System\Renderer\renderer.h"
#include "System\Renderer\animator.h"

constexpr float RADIUS_ADJUSTMENT = 0.7f;

void Animator::Update(aiScene* CurrentAnimation, aiScene* NextAnimation)
{

	if (!CurrentAnimation->HasAnimations())return;
	if (!NextAnimation->HasAnimations())return;

	//	遷移中だったら
	if (m_IsTransitioning)
	{
		AddBlendRatio();
		AddCurrentAnimationFrame();
		AddNextAnimationFrame();
	}
	//	普通の再生
	else
	{
		AddCurrentAnimationFrame();
	}
	//	遷移完成したら
	if (m_BlendRatio >= 1)
	{
		m_IsTransitioning = false;
		SetCurrentAnimation(m_NextAnimation);
		m_CurrentFrame = m_NextFrame;
		m_BlendRatio = 0;
	}

	//	アニメーションデータから
	aiAnimation* animation1 = CurrentAnimation->mAnimations[0];
	aiAnimation* animation2 = NextAnimation->mAnimations[0];

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
			f = m_CurrentFrame % nodeAnim1->mNumRotationKeys;	//簡易実装
			rot1 = nodeAnim1->mRotationKeys[f].mValue;
			f = m_CurrentFrame % nodeAnim1->mNumPositionKeys;	//簡易実装
			pos1 = nodeAnim1->mPositionKeys[f].mValue;
		}
		if (nodeAnim2)
		{
			f = m_NextFrame % nodeAnim2->mNumRotationKeys;	//簡易実装
			rot2 = nodeAnim2->mRotationKeys[f].mValue;
			f = m_NextFrame % nodeAnim2->mNumPositionKeys;	//簡易実装
			pos2 = nodeAnim2->mPositionKeys[f].mValue;
		}
		aiVector3D pos;
		aiQuaternion rot;
		pos = pos1 * (1.0f - m_BlendRatio) + pos2 * m_BlendRatio;	//線形補間
		aiQuaternion::Interpolate(rot, rot1, rot2, m_BlendRatio);	//球面線形補間

		bone.AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rot, pos);
	}
}


void Animator::SetNextAnimation(const std::string& AnimationName)
{
	m_NextAnimation = AnimationName;
	m_IsTransitioning = true;
	m_NextFrame = 0;
}



