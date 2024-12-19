#pragma once
#include <vector>
#include "System\Enum\gameObjectEnum.h"
class GameObject;
class Scene;
class DebuggerImGuiManager
{
public:
	static void SetGameObject(GameObject* object);
	static void LoadCurrentSceneGameObjectList(Scene* scene);
	static void Init();
	static void Render(const std::vector<GameObject*>(&ObjectList)[static_cast<int>(GAMEOBJECT_TYPE::MAX_TYPE)]);
	static void Uninit();
private:

	static GameObject* m_TargetObject;
	static std::vector<GameObject*> m_GameObjectList;

};

