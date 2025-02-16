#pragma once
#include "System\Enum\modelEnum.h"
#include "System\Renderer/renderer.h"
#include <string>
#include <unordered_map>

class AnimationModel;
class AnimationRendererManager
{
private:
	static std::unordered_map<MODEL_NAME, AnimationModel*> m_AnimationModelPool;

public:

	static void UnloadAll();

	static AnimationModel* LoadAnimationModel(const MODEL_NAME& Model);

};