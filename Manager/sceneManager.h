#pragma once
class Scene;
class GameObjectManager;
class SceneManager
{
private:
	SceneManager() {}
	static SceneManager* m_Instance;
	static Scene* m_CurrentScene;
	static bool	m_IsDebugMode;
public:
	static SceneManager* GetInstance();
	~SceneManager(){}
	static void Init();
	static void Update(const float& DeltaTime);
	static void Draw();
	static void Uninit();
	static Scene* GetCurrentScene() { return m_CurrentScene; }
	static bool GetIsDebugMode() { return m_IsDebugMode; }
	static void SetIsDebugMode(bool flag) { m_IsDebugMode = flag; }
	static GameObjectManager* GetGameObjectManager();
};