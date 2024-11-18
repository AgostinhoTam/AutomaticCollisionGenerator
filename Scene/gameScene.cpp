#include "Manager/gameObjectManager.h"
#include "Manager/sceneManager.h"
#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Field/field.h"
#include "GameObject/Camera/camera.h"
#include "Enum/gameObjectEnum.h"
#include "gameScene.h"
void GameScene::Init()
{
	GameObjectManager* ObjectManager{};
	m_SceneManager = SceneManager::GetInstance();
	if (m_SceneManager)
	{
		ObjectManager = m_SceneManager->GetGameObjectManager();
	}
	if (ObjectManager)
	{
		ObjectManager->AddGameObject<Camera>(GAMEOBJECT_TYPE::CAMERA);
		ObjectManager->AddGameObjectArg<Field>(GAMEOBJECT_TYPE::FIELD, XMFLOAT3(0, 0, 0), XMFLOAT2(50, 50));
		ObjectManager->Init();
	}

}

void GameScene::Update(const float& DeltaTime)
{

}

void GameScene::Draw()
{
	
}

void GameScene::Uninit()
{
	
}
