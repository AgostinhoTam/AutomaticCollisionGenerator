#include "Manager/gameObjectManager.h"
#include "Manager/sceneManager.h"
#include "Renderer/renderer.h"

void SceneManager::Init()
{
	Renderer::Init();
	m_GameObjectManager = new GameObjectManager;
	if (m_GameObjectManager)
	{
		m_GameObjectManager->Init();
	}
}

void SceneManager::Update(const float& DeltaTime)
{
	m_GameObjectManager->Update(DeltaTime);
}

void SceneManager::Draw()
{
	Renderer::Begin();
	m_GameObjectManager->Draw();
	Renderer::End();
}

void SceneManager::Uninit()
{
	m_GameObjectManager->Uninit();
	delete m_GameObjectManager;
}
