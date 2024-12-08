#include "Manager/gameObjectManager.h"
#include "Manager/sceneManager.h"
#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Field/field.h"
#include "GameObject/Camera/camera.h"
#include "Enum/gameObjectEnum.h"
#include "gameScene.h"
void GameScene::Init()
{
	m_GameObjectManager = new GameObjectManager;
	if (m_GameObjectManager)
	{
		m_GameObjectManager->AddGameObject<Camera>(GAMEOBJECT_TYPE::CAMERA);
		m_GameObjectManager->AddGameObjectArg<Field>(GAMEOBJECT_TYPE::FIELD, XMFLOAT3(0, 0, 0), XMFLOAT2(50, 50));
		Player* player = m_GameObjectManager->AddGameObject<Player>(GAMEOBJECT_TYPE::PLAYER);
		m_GameObjectManager->Init();
	}
	

}

void GameScene::Update(const float& DeltaTime)
{
	m_GameObjectManager->Update(DeltaTime);
}

void GameScene::Draw()
{
	m_GameObjectManager->Draw();
}

void GameScene::Uninit()
{
	m_GameObjectManager->Uninit();
	delete m_GameObjectManager;
}
