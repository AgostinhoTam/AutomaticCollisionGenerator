#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "System/Enum/gameObjectEnum.h"

class GameObject;

class GameObjectManager
{
private:
	std::vector<GameObject*> m_GameObjectList[static_cast<int>(GameObject_Type::Max_Type)];

public:
	GameObjectManager(){}
	~GameObjectManager(){}
	void Init();
	void Update(const float& DeltaTime);
	void Draw();
	void Uninit();
	template<typename T>
	T* AddGameObject(const GameObject_Type& Type)
	{
		auto& list = m_GameObjectList[static_cast<int>(Type)];
		size_t id = list.size();
		T* gameObject = new T();
		if (gameObject)
		{
			list.emplace_back(gameObject);
			gameObject->SetName(std::to_string(id));
			return gameObject;
		}
		return nullptr;
	}

	template<typename T, typename... Args>
	T* AddGameObjectArg(const GameObject_Type& Type, Args&&... args)
	{
		auto& list = m_GameObjectList[static_cast<int>(Type)];
		size_t id = list.size();
		T* gameObject = new T(std::forward<Args>(args)...);
		if (gameObject)
		{
			list.emplace_back(gameObject);
			gameObject->SetName(std::to_string(id));
			return gameObject;
		}
		return nullptr;
	}

	//　リスト指定で処理が早い
	template<typename T>
	void GetGameObjectsByLayer(std::vector<T*>& targetlist, const GameObject_Type& layer)
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
	T* GetGameObject(const GameObject_Type& layer)
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
