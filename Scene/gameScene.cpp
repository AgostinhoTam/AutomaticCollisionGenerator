#include "Manager/gameObjectManager.h"
#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Field/field.h"
#include "GameObject/Camera/camera.h"
#include "GameObject\Bullet\projection.h"
#include "GameObject\Character\Enemy\enemy.h"
#include "System\Enum\enemyTypeEnum.h"
#include "System\Enum/gameObjectEnum.h"
#include "gameScene.h"
constexpr int MAX_PROJECTION = 10;
void GameScene::Init()
{
	m_GameObjectManager = new GameObjectManager;
	if (m_GameObjectManager)
	{
		m_GameObjectManager->AddGameObject<Camera>(GAMEOBJECT_TYPE::CAMERA);
		m_GameObjectManager->AddGameObjectArg<Field>(GAMEOBJECT_TYPE::FIELD, XMFLOAT3(0, 0, 0), XMFLOAT2(50, 50));
		m_GameObjectManager->AddGameObject<Player>(GAMEOBJECT_TYPE::PLAYER);
		m_GameObjectManager->AddGameObjectArg<Enemy>(GAMEOBJECT_TYPE::ENEMY,ENEMY_TYPE::ENEMY)->SetPosition(XMFLOAT3(-20.0f, 0.0f, 0.0f));
		m_GameObjectManager->AddGameObjectArg<Enemy>(GAMEOBJECT_TYPE::ENEMY, ENEMY_TYPE::MONSTER)->SetPosition(XMFLOAT3(-20.0f, 0.0f, 10.0f));
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
