#pragma once
class Scene;
class GameObjectManager;

class SceneManager
{
public:
	SceneManager(){}
	~SceneManager(){}
	void Init();
	void Update(const float& DeltaTime);
	void Draw();
	void Uninit();
	Scene* GetScene();
private:
	Scene* m_CurrentScene;
	GameObjectManager* m_GameObjectManager;
};