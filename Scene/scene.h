#pragma once
class SceneManager;
class GameObjectManager;
class Scene
{
protected:
	GameObjectManager* m_GameObjectManager{};
public:
	Scene(){}
	virtual ~Scene(){}
	virtual void Init(){}
	virtual void Update(const float& DeltaTime){}
	virtual void Draw(){}
	virtual void Uninit(){}
	GameObjectManager* GetGameObjectManager() const { return m_GameObjectManager; }

};
