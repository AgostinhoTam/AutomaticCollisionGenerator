#pragma once
#include <vector>
#include <unordered_map>
#include "System\Enum/gameObjectEnum.h"

class GameObject;

class GameObjectManager
{
private:
	std::vector<GameObject*> m_GameObjectList[static_cast<int>(GAMEOBJECT_TYPE::MAX_TYPE)];

public:
	GameObjectManager(){}
	~GameObjectManager(){}
	void Init();
	void Update(const float& DeltaTime);
	void Draw();
	void Uninit();
	template<typename T>
	T* AddGameObject(const GAMEOBJECT_TYPE& Type)
	{
		T* object = new T();
		if (object)
		{
			m_GameObjectList[static_cast<int>(Type)].emplace_back(object);
			return object;
		}
		return nullptr;
	}

	template<typename T, typename... Args>
	T* AddGameObjectArg(const GAMEOBJECT_TYPE& Type, Args&&... args)
	{
		T* gameObject = new T(std::forward<Args>(args)...);
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

	//　リスト指定で処理が早い
	template<typename T>
	T* GetGameObject(const GAMEOBJECT_TYPE& layer)
	{
		for (GameObject* object : m_GameObjectList[static_cast<int>(layer)])
		{
			if (!object)continue;
			T* gameObject = dynamic_cast<T*>(object);
			if (gameObject)
			{
				return gameObject;
			}
		}
		return nullptr;
	}

	const std::vector<GameObject*>& GetGameObjectsList(const int type) { return m_GameObjectList[type]; }

};
