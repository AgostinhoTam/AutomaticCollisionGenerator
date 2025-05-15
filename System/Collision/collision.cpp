/*===================================================================================

コリジョン基底クラス(collision.cpp)

====================================================================================*/
#include "collision.h"
#include "Manager/shaderManager.h"
//	===================コリジョン基底クラスコンストラクタ======================
//	Position	:	XMFLOAT3	位置
//	Offset		:	XMFLOAT3	オフセット値
Collision::Collision(const XMFLOAT3& Position, const XMFLOAT3& Offset) :m_Position(Position), m_Offset(Offset)
{
	m_Shader = ShaderManager::LoadShader(Shader_Type::Unlit_Texture);
}

//	===================コリジョン描画用のフラグリセット======================
void Collision::ResetAllCollisionFlag()
{
	m_IsHit = false;
	m_IsSelected = false;
}
