
#include "Renderer/renderer.h"
#include "GameObject/2DPolygon/polygon2D.h"
#include "GameObject/Field/field.h"
#include "GameObject/Camera/camera.h"
#include "Manager/gameObjectManager.h"

void GameObjectManager::Init()
{
	Renderer::Init();

}


void GameObjectManager::Uninit()
{

	Renderer::Uninit();
}

void GameObjectManager::Update(const float& DeltaTime)
{

}


void GameObjectManager::Draw()
{
	Renderer::Begin();

	Renderer::End();
}

