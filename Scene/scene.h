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
protected:
	SceneManager* m_SceneManager;
};
