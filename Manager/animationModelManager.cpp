
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>


#include "Main/main.h"
#include "System\Renderer\animationModelResource.h"
#include "Manager/animationModelManager.h"


std::unordered_map<MODEL_NAME, AnimationModelResource*> AnimationModelManager::m_AnimationModelPool;

void AnimationModelManager::UnloadAll()
{
	for (std::pair<MODEL_NAME, AnimationModelResource*> pair : m_AnimationModelPool)
	{
		pair.second->Uninit();
		delete pair.second;
	}
	m_AnimationModelPool.clear();
}

AnimationModelResource* AnimationModelManager::LoadAnimationModel(const MODEL_NAME& Model)
{
	if (m_AnimationModelPool.find(Model) != m_AnimationModelPool.end())
	{
		return m_AnimationModelPool[Model];
	}

	AnimationModelResource* animationModel = new AnimationModelResource;
	if (!animationModel)return nullptr;

	switch (Model)
	{
	case MODEL_NAME::PLAYER:
		animationModel->Load("asset\\model\\player.fbx");
		animationModel->LoadAnimation("asset\\model\\player_Idle.fbx", "Player_Idle");
		animationModel->LoadAnimation("asset\\model\\player_Run.fbx", "Player_Run");
		m_AnimationModelPool.try_emplace(Model, animationModel);
		break;
	case MODEL_NAME::ENEMY:
		animationModel->Load("asset\\model\\enemy.fbx");
		animationModel->LoadAnimation("asset\\model\\enemy_Idle.fbx","Enemy_Idle");
		animationModel->LoadAnimation("asset\\model\\enemy_Run.fbx","Enemy_Run");
		animationModel->LoadAnimation("asset\\model\\enemy_Kick.fbx","Enemy_Kick");
		m_AnimationModelPool.try_emplace(Model, animationModel);
		break;
	case MODEL_NAME::MONSTER:
		animationModel->Load("asset\\model\\monster.fbx");
		animationModel->LoadAnimation("asset\\model\\monster_Idle.fbx", "Monster_Idle");
		animationModel->LoadAnimation("asset\\model\\monster_Run.fbx", "Monster_Run");
		animationModel->LoadAnimation("asset\\model\\monster_Attack.fbx", "Monster_Attack");
		m_AnimationModelPool.try_emplace(Model, animationModel);
		break;
	default:
		animationModel->Load("asset\\model\\default.fbx");
		break;
	}
	return m_AnimationModelPool[Model];
}

