#include "collision.h"
#include "Manager\shaderManager.h"
Collision::Collision(const XMFLOAT3& Position, const XMFLOAT3& Offset) :m_Position(Position), m_Offset(Offset)
{
	m_Shader = ShaderManager::LoadShader(SHADER_NAME::UNLIT_TEXTURE);
}


void Collision::ResetAllCollisionFlag()
{
	m_IsHit = false;
	m_IsSelected = false;
}
