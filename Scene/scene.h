#pragma once
#include <list>
#include <vector>
#include "GameObject/gameobject.h"

class Scene
{
public:
	Scene();
	~Scene();



protected:
	std::list<GameObject*> m_GameObject[static_cast<int>(GAMEOBJECT_TYPE::MAX_TYPE)];
};

Scene::Scene()
{
}

Scene::~Scene()
{
}