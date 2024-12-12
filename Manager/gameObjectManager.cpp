#include "GameObject/gameobject.h"
#include "Manager/gameObjectManager.h"
#include "Manager\debuggerImGuiManager.h"

void GameObjectManager::Init()
{
	for (int type = 0; type < static_cast<int>(GAMEOBJECT_TYPE::MAX_TYPE); ++type)
	{
		for (GameObject* object : m_GameObjectList[type])
		{
			if (!object)continue;
			object->Init();
		}
	}

}


void GameObjectManager::Uninit()
{
	for (int type = 0; type < static_cast<int>(GAMEOBJECT_TYPE::MAX_TYPE); ++type)
	{
		for (GameObject* object : m_GameObjectList[type])
		{
			delete object;
			object = nullptr;
		}
	}
}


void GameObjectManager::Update(const float& DeltaTime)
{
	for (int type = 0; type < static_cast<int>(GAMEOBJECT_TYPE::MAX_TYPE); ++type)
	{
		for (GameObject* object : m_GameObjectList[type])
		{
			if (!object)continue;
			object->Update(DeltaTime);
		}
	}

}


void GameObjectManager::Draw()
{
	for (int type = 0; type < static_cast<int>(GAMEOBJECT_TYPE::MAX_TYPE); ++type)
	{
		for (GameObject* object : m_GameObjectList[type])
		{
			if (!object)continue;
			object->Draw();
		}
	}
#ifdef _DEBUG
	DebuggerImGuiManager::Render(m_GameObjectList);

#endif // _DEBUG

}

