/*===================================================================================

ゲームオブジェクトマネージャー(gameObjectManager.cpp)

====================================================================================*/
#include "GameObject/gameobject.h"
#include "Manager/gameObjectManager.h"
#include "Manager/debuggerImGuiManager.h"

//	===================ゲームオブジェクトマネージャー初期化======================
void GameObjectManager::Init()
{
	//	ゲームオブジェクト全部初期化
	for (int type = 0; type < static_cast<int>(GameObject_Type::Max_Type); ++type)
	{
		for (GameObject* object : m_GameObjectList[type])
		{
			if (!object)continue;
			object->Init();
		}
	}

}

//	===================ゲームオブジェクトマネージャーUninit======================
void GameObjectManager::Uninit()
{
	//	ゲームオブジェクト全部Uninit
	for (int type = 0; type < static_cast<int>(GameObject_Type::Max_Type); ++type)
	{
		for (GameObject* object : m_GameObjectList[type])
		{
			delete object;
			object = nullptr;
		}
	}
}

//	===================ゲームオブジェクトマネージャー更新======================
//	DeltaTime	:	float	デルタタイム
void GameObjectManager::Update(const float& DeltaTime)
{
	//	ゲームオブジェクト全部更新
	for (int type = 0; type < static_cast<int>(GameObject_Type::Max_Type); ++type)
	{
		for (GameObject* object : m_GameObjectList[type])
		{
			if (!object)continue;
			object->Update(DeltaTime);
		}
	}

}

//	===================ゲームオブジェクトマネージャー描画======================
void GameObjectManager::Draw()
{
	//	ゲームオブジェクト全部描画
	for (int type = 0; type < static_cast<int>(GameObject_Type::Max_Type); ++type)
	{
		for (GameObject* object : m_GameObjectList[type])
		{
			if (!object)continue;
			object->Draw();
		}
	}
	//	ImGui描画
	DebuggerImGuiManager::Render(m_GameObjectList);
}

