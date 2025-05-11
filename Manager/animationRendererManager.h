#pragma once
#include "System/Enum/modelEnum.h"
#include "System/Renderer/renderer.h"
#include <string>
#include <unordered_map>

class GameObject;
class AnimationModel;
class AnimationRendererManager
{
private:
	static std::unordered_map<Model_Name, AnimationModel*> m_AnimationModelPool;

public:

	static void UnloadAll();

	static AnimationModel* LoadAnimationModel(const Model_Name& Model, GameObject* Owner);

};
