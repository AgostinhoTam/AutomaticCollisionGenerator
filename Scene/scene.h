#pragma once
class SceneManager;
class Scene
{
public:
	Scene(){}
	virtual ~Scene(){}
	virtual void Init(){}
	virtual void Update(const float& DeltaTime){}
	virtual void Draw(){}
	virtual void Uninit(){}
	GameObjectManager* GetGameObjectManager() { return m_GameObjectManager; }
protected:
	GameObjectManager* m_GameObjectManager{};
};
