#pragma once
class Scene;

class SceneManager
{
public:
	static SceneManager* GetInstance();
	~SceneManager(){}
	static void Init();
	static void Update(const float& DeltaTime);
	static void Draw();
	static void Uninit();
	static Scene* GetCurrentScene() { return m_CurrentScene; }
private:
	SceneManager() {}
	static SceneManager* m_Instance;
	static Scene* m_CurrentScene;
};