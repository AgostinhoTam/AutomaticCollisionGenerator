#pragma once
#include <vector>
#include <unordered_map>
#include "Enum/gameObjectEnum.h"

class GameObject;

class GameObjectManager
{
public:
	GameObjectManager(){}
	~GameObjectManager(){}
	void Init();
	void Update(const float& deltaTime);
	void Draw();
	void Uninit();
	template<typename T>
	T* AddGameObject(const GAMEOBJECT_TYPE& Type)
	{
		T* object = new T(Type);
		if (object)
		{
			m_GameObjectList[Type]->emplaced_back(object);
			return object;
		}
		return nullptr;
	}

	template<typename T, typename... Args>
	T* AddGameObjectArg(const GAMEOBJECT_TYPE& Type, Args&&... args)
	{
		T* gameObject = new T(Type,std::forward<Args>(args)...);
		if (gameObject)
		{
			m_GameObjectList[static_cast<int>(Type)].emplace_back(gameObject);
			return gameObject;
		}
		return nullptr;
	}

	//　リスト指定で処理が早い
	template<typename T>
	void GetGameObjectsByLayer(std::vector<T*>& targetlist, const GAMEOBJECT_TYPE& layer)
	{
		for (GameObject* object : m_GameObjectList[static_cast<int>(layer)])
		{
			T* gameObject = dynamic_cast<T*>(object);
			if (gameObject != nullptr)
			{
				targetlist.emplace_back(gameObject);
			}
		}
	}
private:
	std::vector<GameObject*> m_GameObjectList[static_cast<int>(GAMEOBJECT_TYPE::MAX_TYPE)];
};
