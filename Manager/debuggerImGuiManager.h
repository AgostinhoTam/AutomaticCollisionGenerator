#pragma once
#include <vector>
#include "System\Enum\gameObjectEnum.h"
class GameObject;
class Scene;
class DebuggerImGuiManager
{
private:
	static GameObject* m_TargetObject;
	static std::vector<GameObject*> m_GameObjectList;
	static std::vector<std::string> m_FileList;
public:
	static void SetGameObject(GameObject* object);
	static void LoadCurrentSceneGameObjectList(Scene* scene);
	static void Init();
	static void Render(std::vector<GameObject*>(&ObjectList)[static_cast<int>(GAMEOBJECT_TYPE::MAX_TYPE)]);
	static void Uninit();
	static void LoadCSVFiles(const std::string& Path, std::vector<const char*>& CSVFileName);
};

