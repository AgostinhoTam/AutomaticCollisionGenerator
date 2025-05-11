
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>


#include "Main/main.h"
#include "System/Renderer/animationModel.h"
#include "Manager/animationRendererManager.h"


std::unordered_map<Model_Name, AnimationModel*> AnimationRendererManager::m_AnimationModelPool;

void AnimationRendererManager::UnloadAll()
{
	for (std::pair<Model_Name, AnimationModel*> pair : m_AnimationModelPool)
	{
		pair.second->Uninit();
		delete pair.second;
	}
	m_AnimationModelPool.clear();
}

AnimationModel* AnimationRendererManager::LoadAnimationModel(const Model_Name& Model,GameObject* Owner)
{
	if (m_AnimationModelPool.find(Model) != m_AnimationModelPool.end())
	{
		return m_AnimationModelPool[Model];
	}

	AnimationModel* animationModel = new AnimationModel;
	if (!animationModel)return nullptr;

	switch (Model)
	{
	case Model_Name::Player:
		animationModel->Load("asset\\model\\player.fbx",Owner);
		animationModel->LoadAnimation("asset\\model\\player_Idle.fbx", "Player_Idle");
		animationModel->LoadAnimation("asset\\model\\player_Run.fbx", "Player_Run");
		m_AnimationModelPool.try_emplace(Model, animationModel);
		break;
	case Model_Name::Enemy_Humanoid:
		animationModel->Load("asset\\model\\enemy.fbx", Owner);
		animationModel->LoadAnimation("asset\\model\\enemy_Idle.fbx","Enemy_Idle");
		animationModel->LoadAnimation("asset\\model\\enemy_Run.fbx","Enemy_Run");
		animationModel->LoadAnimation("asset\\model\\enemy_Kick.fbx","Enemy_Kick");
		m_AnimationModelPool.try_emplace(Model, animationModel);
		break;
	case Model_Name::Enemy_Monster:
		animationModel->Load("asset\\model\\monster.fbx", Owner);
		animationModel->LoadAnimation("asset\\model\\monster_Idle.fbx", "Monster_Idle");
		animationModel->LoadAnimation("asset\\model\\monster_Run.fbx", "Monster_Run");
		animationModel->LoadAnimation("asset\\model\\monster_Attack.fbx", "Monster_Attack");
		m_AnimationModelPool.try_emplace(Model, animationModel);
		break;
	default:
		animationModel->Load("asset\\model\\default.fbx", Owner);
		break;
	}
	return m_AnimationModelPool[Model];
}

