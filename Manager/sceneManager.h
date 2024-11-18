#pragma once
class Scene;
class GameObjectManager;

class SceneManager
{
public:
	static SceneManager* GetInstance();
	~SceneManager(){}
	static void Init();
	static void Update(const float& DeltaTime);
	static void Draw();
	static void Uninit();
	Scene* GetCurrentScene() { return m_CurrentScene; }
	GameObjectManager* GetGameObjectManager() { return m_GameObjectManager; }
private:
	SceneManager() {}
	static SceneManager* m_Instance;
	static Scene* m_CurrentScene;
	static GameObjectManager* m_GameObjectManager;
};