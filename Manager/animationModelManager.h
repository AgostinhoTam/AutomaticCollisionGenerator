#pragma once
#include "System\Enum\modelEnum.h"
#include "System\Renderer/renderer.h"
#include <string>
#include <unordered_map>

class GameObject;
class AnimationModel;
class AnimationModelResource;
class AnimationModelManager
{
private:
	static std::unordered_map<MODEL_NAME, AnimationModelResource*> m_AnimationModelPool;

public:

	static void UnloadAll();

	static AnimationModelResource* LoadAnimationModel(const MODEL_NAME& Model);

};