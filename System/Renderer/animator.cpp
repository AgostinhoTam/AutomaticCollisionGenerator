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

	//	�A�j���[�V�����f�[�^����
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
}


void Animator::SetNextAnimation(const std::string& AnimationName)
{
	m_NextAnimation = AnimationName;
	m_IsTransitioning = true;
	m_NextFrame = 0;
}



