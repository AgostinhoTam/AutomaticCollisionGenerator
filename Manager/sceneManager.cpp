/*===================================================================================

シーンマネージャー(sceneManager.cpp)

====================================================================================*/
#include "Manager/inputManager.h"
#include "Manager/gameObjectManager.h"
#include "Manager/sceneManager.h"
#include "Manager/shaderManager.h"
#include "Manager/modelRendererManager.h"
#include "Manager\debuggerImGuiManager.h"
#include "System\Renderer/renderer.h"
#include "Scene/gameScene.h"

SceneManager* SceneManager::m_Instance = nullptr;
Scene* SceneManager::m_CurrentScene = nullptr;
bool	SceneManager::m_IsDebugMode = false;
SceneManager* SceneManager::GetInstance()
{
	if (!m_Instance) {
		m_Instance = new SceneManager();
	}
	return m_Instance;
}

//	===================シーンマネージャー初期化======================
void SceneManager::Init()
{
	Renderer::Init();
	ShaderManager::Init();
	InputManager::Init();
	DebuggerImGuiManager::Init();

	//	初期シーン設定
	m_CurrentScene = new GameScene;
	if (m_CurrentScene) {
		m_CurrentScene->Init();
	}
}

//	===================シーンマネージャー更新======================
//	DeltaTime	:	float	デルタタイム
void SceneManager::Update(const float& DeltaTime)
{
	if (!m_CurrentScene)return;
	InputManager::Update();
	m_CurrentScene->Update(DeltaTime);
}
//	===================シーンマネージャー描画======================
void SceneManager::Draw()
{
	if (!m_CurrentScene)return;
	Renderer::Begin();
	m_CurrentScene->Draw();
	Renderer::End();
	InputManager::UpdateLastMousePos();
}

//	===================シーンマネージャーUninit======================
void SceneManager::Uninit()
{
	m_CurrentScene->Uninit();
	delete m_CurrentScene;
	InputManager::Uninit();
	ShaderManager::Uninit();
	ModelRendererManager::UnloadAll();
	DebuggerImGuiManager::Uninit();
}

//	===================シーンマネージャー取得======================
GameObjectManager* SceneManager::GetGameObjectManager()
{
	if (!m_CurrentScene)return nullptr;
	return m_CurrentScene->GetGameObjectManager();
}
