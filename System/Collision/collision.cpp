#include "collision.h"
#include "Manager\shaderManager.h"
Collision::Collision(GameObject* Owner, XMFLOAT3 Offset) :m_Owner(Owner), m_Offset(Offset)
{
	m_Shader = ShaderManager::LoadShader(SHADER_NAME::UNLIT_TEXTURE);
}
