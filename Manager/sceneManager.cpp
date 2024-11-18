#include "Manager/gameObjectManager.h"
#include "Manager/sceneManager.h"
#include "Manager/shaderManager.h"
#include "Renderer/renderer.h"
#include "Scene/gameScene.h"

SceneManager* SceneManager::m_Instance = nullptr;
GameObjectManager* SceneManager::m_GameObjectManager = nullptr;;
Scene* SceneManager::m_CurrentScene = nullptr;

SceneManager* SceneManager::GetInstance()
{
	if (!m_Instance) {
		m_Instance = new SceneManager();
	}
	return m_Instance;
}

void SceneManager::Init()
{
	Renderer::Init();
	ShaderManager::Init();
	m_GameObjectManager = new GameObjectManager;
	if (!m_GameObjectManager)return;
	m_CurrentScene = new GameScene;
	if (m_CurrentScene) {
		m_CurrentScene->Init();
	}
}

void SceneManager::Update(const float& DeltaTime)
{
	if (!m_GameObjectManager)return;
	if (!m_CurrentScene)return;
	m_GameObjectManager->Update(DeltaTime);
	m_CurrentScene->Update(DeltaTime);
}

void SceneManager::Draw()
{
	if (!m_GameObjectManager)return;
	if (!m_CurrentScene)return;
	Renderer::Begin();
	m_GameObjectManager->Draw();
	Renderer::End();
}

void SceneManager::Uninit()
{
	m_CurrentScene->Uninit();
	delete m_CurrentScene;
	m_GameObjectManager->Uninit();
	delete m_GameObjectManager;
	ShaderManager::Uninit();
}